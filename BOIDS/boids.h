#ifndef BOIDS_H
#define BOIDS_H

typedef struct Boid {
  char visualRange;
  float* position;
  float accelaration;
  float* velocity;
} Boid;

#define TRUE 1
#define FALSE 0
#define NUM_DIM 3

Boid* initializeBoids();

Boid* advanceBoids();

float* getBoidPositions();

int getNumBoids();

int howManyBytesInABoid();

void freeBoids();

#endif