#include <stdlib.h>
#include <stdio.h>
#include "boids.h"
// ONLY INCLUDE THIS HEADER IF COMPILED WITH EMCC
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif


// the array of Boid structs
Boid* themBoids = NULL;
// totally safe, production level code
// shared heap memory between WebGl and this program
float* boidSharedPositions = NULL;
#define NUM_BOIDS 5 // javascript can tell us how many or vice versa
#define NUM_DIM 3 // same for this


// MALLOCS Boids and their fields as well as shared boidPositions, and assigns initial values. TODO: Check malloc return vals
EMSCRIPTEN_KEEPALIVE
void initializeBoids()
{
  int bI = 0;

  themBoids = malloc(NUM_BOIDS * sizeof(Boid));
  boidSharedPositions = malloc(NUM_BOIDS * NUM_DIM * sizeof(float));

  while (bI < NUM_BOIDS)
  {
    themBoids[bI].position = malloc(NUM_DIM * sizeof(float));
    themBoids[bI].velocity = malloc(NUM_DIM * sizeof(float));

    themBoids[bI].accelaration = 0;
    themBoids[bI].visualRange = 1.0; // This will require some experimentation probably.

    // for each x,y,z in the case of 3dim boids, set the initial values of position and velocity
    for (int idx = 0; idx < NUM_DIM; idx++)
    {
      themBoids[bI].position[idx] = bI; 
      themBoids[bI].velocity[idx] = 0;
      // for this boid, write its position values to the shared heap memory as they are initialized
      boidSharedPositions[(bI * NUM_DIM) + idx] = themBoids[bI].position[idx];
    }

    bI++;
  }
}

EMSCRIPTEN_KEEPALIVE
void advanceBoids()
{
  int bI, pI;
  
}

EMSCRIPTEN_KEEPALIVE
int howManyBytesInABoid()
{
  return sizeof(Boid);
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
    idx++;
  }
  
  free(themBoids);
  free(boidSharedPositions);
}