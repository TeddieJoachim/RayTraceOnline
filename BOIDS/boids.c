#include <stdlib.h>
#include <stdio.h>
#include "boids.h"
#include <emscripten/emscripten.h>


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
Boid* initializeBoids(int numBoids)
{
  int idx = 0;
  Boid* themBoids = malloc(numBoids * sizeof(Boid));

  while (idx < numBoids)
  {
    setBoidAttributes(&themBoids[idx]);
    printBoid(&themBoids[idx]);
    idx++;
  }
  

  return &themBoids[0];
}

void printBoid(Boid* thisBoid)
{
  int idx = 0;

  printf("Boid coordinates: \n");
  while (idx < NUM_DIM)
  {
    printf(" %f ", thisBoid->position[idx]);
    idx++;
  }
  printf("\n");
}

void freeBoids(Boid* toFree, int numBoids)
{
  int idx = 0;

  while (idx < numBoids)
  {
    free(toFree[idx].position);
    free(toFree[idx].velocity);
    idx++;
  }
  
  free(toFree);
}