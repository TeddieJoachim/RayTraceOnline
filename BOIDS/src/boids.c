#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "boids.h"


// ONLY INCLUDE THIS HEADER IF COMPILED WITH EMCC
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#define NUM_BOIDS 50 // CAN NEVER BE 1!
#define NUM_DIM 3
#define WORLD_MIN -1.0f
#define WORLD_MAX 1.0f

#define MAX_SPEED 1.5f
#define MAX_ACCELERATION 1.0f

#define TARGET_SPEED 1.0f
#define SPEED_MAINTENANCE_WEIGHT 0.5f

#define COLLISION_RADIUS 0.1f
#define PERCEPTION_RADIUS 0.6f
#define INITIAL_VELOCITY 0.3f // start in a random val on range [-IV, IV]
#define COHESION_WEIGHT 0.3f
#define SEPERATION_WEIGHT 0.15f
#define ALIGNMENT_WEIGHT 0.5f


Boid* themBoids = NULL; // totally safe, production level code. the array of Boid structs
float* boidSharedPositions = NULL; // shared heap memory between WebGl and this program

// rand() returns a "random" float from 0 to some big fuckin number. if we want to clamp it between 
// min and max we need to do some arithmetic
float randomInRange(float min, float max) 
{
  float intervalSize = max - min;
  float normalized = (float)rand() / (float)RAND_MAX; // [0.0, 1.0]
  float scaled = normalized * intervalSize; // [0.0, max-min (20)]

  return scaled + min; // shift so that 0.0 -> min (-10)
}

// MALLOCS Boids and their fields as well as shared boidPositions, and assigns initial values. TODO: Check malloc return vals
EMSCRIPTEN_KEEPALIVE
void initializeBoids()
{
  int bI = 0;

  srand((unsigned int)time(NULL)); // seeds rand, have to cast the current time (time(NULL)) to unsigned int
  themBoids = malloc(NUM_BOIDS * sizeof(Boid));
  boidSharedPositions = malloc(NUM_BOIDS * NUM_DIM * sizeof(float));
  
  if (themBoids == NULL || boidSharedPositions == NULL) 
  {
    return; // some kind of panic, free what was alloced
  }

  while (bI < NUM_BOIDS)
  {
    themBoids[bI].myIdx = bI;
    themBoids[bI].position = malloc(NUM_DIM * sizeof(float));
    themBoids[bI].velocity = malloc(NUM_DIM * sizeof(float));
    themBoids[bI].acceleration = malloc(NUM_DIM * sizeof(float));

    if (themBoids[bI].position == NULL || themBoids[bI].velocity == NULL || themBoids[bI].acceleration == NULL) 
    {
      return; // again, some kind of panic, free what was alloced
    }
    // for each x,y,z in the case of 3dim boids, set the initial values of position and velocity
    for (int idx = 0; idx < NUM_DIM; idx++)
    {
      themBoids[bI].position[idx] = randomInRange(WORLD_MIN, WORLD_MAX);
      themBoids[bI].velocity[idx] = randomInRange(-INITIAL_VELOCITY, INITIAL_VELOCITY);
      themBoids[bI].acceleration[idx] = 0;
      // for this boid, write its position values to the shared heap memory as they are initialized
      boidSharedPositions[(bI * NUM_DIM) + idx] = themBoids[bI].position[idx];
    }

    bI++;
  }
}


// TODO make these safe or use someones library
// NOT SAFE, up to the caller.
// Writes result of addition to target. dim must be dim of target, and for addition to 
// be defined, dimension of v1, v2 as well.
void vectorAdd(float* target, const float* v1, const float* v2, int dim){for (int i = 0; i < dim; i++) target[i] = v1[i] + v2[i];}
void vectorSubtract(float* target, const float* v1, const float* v2, int dim){for (int i = 0; i < dim; i++) target[i] = v1[i] - v2[i];}
// Also unsafe for above reasons.
void vectorScalar(float* vec, float scalar, int dim){for (int i = 0; i < dim; i++) vec[i] *= scalar;}
float vecMagnitude(float* vec, int dim)
{
  float res = 0.0;
  for (int i = 0; i < dim; i++) res += vec[i] * vec[i];
  return sqrtf(res);
}
void zeroVec(float* vec, int dim) {for (int i = 0; i < dim; i++) vec[i] = 0.0;}



void calculateAcceleration(Boid* thisBoid, int flockSize)
{
  float v1[NUM_DIM]; // Average position of all other birds
  float v2[NUM_DIM]; // Separation vector, steer us away from other birds
  float distToOthers[NUM_DIM];
  float v3[NUM_DIM]; // Average velocity of flock
  float accelMag, distMag;
  int neighborCnt = 0;

  // Set all vectos to 0 initially
  zeroVec(v1, NUM_DIM);
  zeroVec(v2, NUM_DIM);
  zeroVec(v3, NUM_DIM);

  for (int bI = 0; bI < flockSize; bI++) 
  {
    Boid* currentBird = &themBoids[bI];
    
    if (bI != thisBoid->myIdx) 
    {
      // check if |(other position - my position)| is in my visual range
      vectorSubtract(distToOthers, currentBird->position, thisBoid->position, NUM_DIM);
      distMag = vecMagnitude(distToOthers, NUM_DIM);
      if (distMag < PERCEPTION_RADIUS)
      {
        neighborCnt++;  
        // average position += (average position + current position) 
        vectorAdd(v1, v1, currentBird->position, NUM_DIM);
        // average velocity += (average velocity + current velocity)
        vectorAdd(v3, v3, currentBird->velocity, NUM_DIM);
      }
      // only steer away from birds we are about to hit
      if (distMag < COLLISION_RADIUS)
      {
        // distToOthers =  (myPos - theirPos)
        vectorSubtract(distToOthers, thisBoid->position, currentBird->position,  NUM_DIM);
        // seperation vector = 0 - (myPos - theirPos)
        vectorAdd(v2, v2, distToOthers, NUM_DIM);
      }
    }
  }

  if (neighborCnt > 0)
  {
    // yields direction towards center of mass scaled to 1%
    vectorScalar(v1, 1.0f / neighborCnt, NUM_DIM);
    // this converts v1 into a steering direction, v1 = averagePos - myPos 
    vectorSubtract(v1, v1, thisBoid->position, NUM_DIM);
    vectorScalar(v1, COHESION_WEIGHT, NUM_DIM); // weight impacts cohesion. higher weight -> more flocking

    // divide sum of flock velocity by flock size to get average
    // This yields the difference between my velocity and the others
    vectorScalar(v3, 1.0f / neighborCnt, NUM_DIM);
    // this converts average velocity into a steering direction, v3 = average velocity - my velocity
    vectorSubtract(v3, v3, thisBoid->velocity, NUM_DIM);
    vectorScalar(v3, ALIGNMENT_WEIGHT, NUM_DIM); // Alignment. Higher weight -> more similar directions
  }

  // scale down seperation vector as well
  vectorScalar(v2, SEPERATION_WEIGHT, NUM_DIM);

  // zero out our acceleration from previous steps
  zeroVec(thisBoid->acceleration, NUM_DIM);
  // now add the average position and the steer away vector to our acceleration (we move towards the average but avoid hitting neighbors)
  vectorAdd(thisBoid->acceleration, v1, v2, NUM_DIM);
  // finally our acceleration = our acceleration thus far + the average velocity of the flock (we try to match velocities)
  vectorAdd(thisBoid->acceleration, thisBoid->acceleration, v3, NUM_DIM);

  accelMag = vecMagnitude(thisBoid->acceleration, NUM_DIM);
  if (accelMag > MAX_ACCELERATION)
  {
    vectorScalar(thisBoid->acceleration, MAX_ACCELERATION / accelMag, NUM_DIM);
  }
}



EMSCRIPTEN_KEEPALIVE
void advanceBoids(float delT)
{
  int birdIdx, coordIdx, sharedPosIdx;
  float speed, speedDiff, accelMag;
  Boid thisBoid;

  // calculate every birds acceleration
  for (birdIdx = 0; birdIdx < NUM_BOIDS; birdIdx++)
  {
    calculateAcceleration(&themBoids[birdIdx], NUM_BOIDS);
  }

  // now update their velocity and position. that is, 
  // velocity += acceleration, position += velocity
  for (birdIdx = 0; birdIdx < NUM_BOIDS; birdIdx++)
  {
    thisBoid = themBoids[birdIdx];
    
    speed = vecMagnitude(thisBoid.velocity, NUM_DIM);
    if (speed < TARGET_SPEED)
    {
      speedDiff = TARGET_SPEED - speed; // I gotta catch up!
      for (coordIdx = 0; coordIdx < NUM_DIM; coordIdx++)
      {
        thisBoid.acceleration[coordIdx] += SPEED_MAINTENANCE_WEIGHT * speedDiff * (thisBoid.velocity[coordIdx] / speed);
      }
    }
    // cap their acceleration
    accelMag = vecMagnitude(thisBoid.acceleration, NUM_DIM);

    if (accelMag > MAX_ACCELERATION) 
    {
      vectorScalar(thisBoid.acceleration, MAX_ACCELERATION / accelMag, NUM_DIM);
    }

    // update velocities
    for (coordIdx = 0; coordIdx < NUM_DIM; coordIdx++)
    {
      thisBoid.velocity[coordIdx] += thisBoid.acceleration[coordIdx] * delT;
    }
    
    // check speed after update
    speed = vecMagnitude(thisBoid.velocity, NUM_DIM);
    if (speed > MAX_SPEED) vectorScalar(thisBoid.velocity, MAX_SPEED/speed, NUM_DIM);
    
    // update position and shared positions
    for (coordIdx = 0; coordIdx < NUM_DIM; coordIdx++)
    {
      thisBoid.position[coordIdx] += thisBoid.velocity[coordIdx] * delT;
      // TEMPORARY BOUNDARY HANDLING
      if (thisBoid.position[coordIdx] > WORLD_MAX)
      {
        thisBoid.position[coordIdx] = WORLD_MAX;
        thisBoid.velocity[coordIdx] *= -1.0f;
      }
      else if (thisBoid.position[coordIdx] < WORLD_MIN)
      {
        thisBoid.position[coordIdx] = WORLD_MIN;
        thisBoid.velocity[coordIdx] *= -1.0f;
      }
      sharedPosIdx = (birdIdx * NUM_DIM) + coordIdx;
      boidSharedPositions[sharedPosIdx] = thisBoid.position[coordIdx];
    }
  }
}

EMSCRIPTEN_KEEPALIVE
float* getBoidPositions()
{
  return &boidSharedPositions[0];
}

EMSCRIPTEN_KEEPALIVE
int getNumBoids()
{
  return NUM_BOIDS;
}

EMSCRIPTEN_KEEPALIVE
int getDim()
{
  return NUM_DIM;
}

EMSCRIPTEN_KEEPALIVE
void freeBoids()
{
  int idx = 0;

  while (idx < NUM_BOIDS) // dont free when killed, translate to ground?
  {
    free(themBoids[idx].position);
    free(themBoids[idx].velocity);
    free(themBoids[idx].acceleration);
    idx++;
  }
  
  free(themBoids);
  free(boidSharedPositions);
}