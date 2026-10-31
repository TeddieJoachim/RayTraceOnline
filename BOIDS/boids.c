#include <stdlib.h>
#include <stdio.h>
#include "boids.h"
#include <emscripten/emscripten.h>

Boid* themBoids = NULL; //totally safe, production level code
float* themBoidPositions = NULL;
#define NUM_BOIDS 5 // javascript can tell us how many or vice versa

EMSCRIPTEN_KEEPALIVE
float* getBoidPositions()
{
  return &themBoidPositions[0];
}

EMSCRIPTEN_KEEPALIVE
int getNumBoids()
{
  return NUM_BOIDS;
}

// MALLOCS the position and velocity vectors, assigns values to them and all other BOID fields
void setBoidAttributes(Boid* disBoid)
{
  int idx = 0;
  
  disBoid->position = malloc(NUM_DIM * sizeof(float));
  disBoid->velocity = malloc(NUM_DIM * sizeof(float));

  disBoid->accelaration = 0;
  disBoid->visualRange = 1.0; // This will require some experimentation probably.

  // for each x,y,z in the case of 3dim boids, set the initial values of position and velocity
  while (idx < NUM_DIM)
  {
    disBoid->position[idx] = 0; // obviously want the boids to not all start at the origin. or maybe we do?
    disBoid->velocity[idx] = 0; // also probably want the boids to be moving?
    idx++;
  }
}

EMSCRIPTEN_KEEPALIVE
Boid* initializeBoids()
{
  int bI = 0;
  int idx = 0;
  themBoids = malloc(NUM_BOIDS * sizeof(Boid));

  while (bI < NUM_BOIDS)
  {
    themBoids[bI].position = malloc(NUM_DIM * sizeof(float));
    themBoids[bI].velocity = malloc(NUM_DIM * sizeof(float));

    themBoids[bI].accelaration = 0;
    themBoids[bI].visualRange = 1.0; // This will require some experimentation probably.

  // for each x,y,z in the case of 3dim boids, set the initial values of position and velocity
    for (idx = 0; idx < NUM_DIM; idx++)
    {
      themBoids[bI].position[idx] = 0; // obviously want the boids to not all start at the origin. or maybe we do?
      themBoidPositions[bI] = themBoids[bI].position[idx];
      themBoids[bI].velocity[idx] = 0; // also probably want the boids to be moving?
    }
    bI++;
  }
  

  return &themBoids[0];
}

EMSCRIPTEN_KEEPALIVE
Boid* advanceBoids()
{
  int idx = 0;
  int bI = 0;

  // for each x,y,z in the case of 3dim boids, set the initial values of position and velocity
  while (idx < NUM_DIM)
  {
    themBoids[bI].position[idx]++;
    themBoids[bI].velocity[idx]++; 
    idx++;
    printf("added\n");
  }
  return &themBoids[0];
}

EMSCRIPTEN_KEEPALIVE
int howManyBytesInABoid()
{
  return sizeof(Boid);
}


EMSCRIPTEN_KEEPALIVE
void freeBoids()
{
  int idx = 0;

  while (idx < NUM_BOIDS) // dont free when killed, translate to ground
  {
    free(themBoids[idx].position);
    free(themBoids[idx].velocity);
    idx++;
  }
  
  free(themBoids);
}