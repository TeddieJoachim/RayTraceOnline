#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "boids.h"


// ONLY INCLUDE THIS HEADER IF COMPILED WITH EMCC
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#define NUM_BOIDS 50 
#define NUM_DIM 3
#define WORLD_MIN -1.0f
#define WORLD_MAX 1.0f


Boid* themBoids = NULL; // totally safe, production level code. the array of Boid structs
float* boidSharedPositions = NULL; // shared heap memory between WebGl and this program

// rand() returns a "random" float from 0 to some big fuckin number. if we want to clamp it between 
// min and max we need to do some arithmetic
float randomCoordinate(float min, float max) 
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
    themBoids[bI].visualRange = 1.0; // This will require some experimentation probably.
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
      themBoids[bI].position[idx] = randomCoordinate(WORLD_MIN, WORLD_MAX);
      themBoids[bI].velocity[idx] = 0;
      themBoids[bI].acceleration[idx] = 0;
      // for this boid, write its position values to the shared heap memory as they are initialized
      boidSharedPositions[(bI * NUM_DIM) + idx] = themBoids[bI].position[idx];
    }

    bI++;
  }
}

EMSCRIPTEN_KEEPALIVE
void advanceBoids() // TODO: this is where the logic of BOIDS goes. eventually want advance boids to update both our internal themBoids and the shared memory
{
  int birdIdx, coordIdx, startIdx;

  for (birdIdx = 0; birdIdx < NUM_BOIDS; birdIdx++)
  {
    for (coordIdx = 0; coordIdx < NUM_DIM; coordIdx++)
    {
      startIdx = (birdIdx * NUM_DIM) + coordIdx;
      boidSharedPositions[startIdx] = randomCoordinate(WORLD_MIN, WORLD_MAX);
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

  while (idx < NUM_BOIDS) // dont free when killed, translate to ground
  {
    free(themBoids[idx].position);
    free(themBoids[idx].velocity);
    free(themBoids[idx].acceleration);
    idx++;
  }
  
  free(themBoids);
  free(boidSharedPositions);
}