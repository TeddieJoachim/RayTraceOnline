#ifndef BOIDS_H
#define BOIDS_H

typedef struct Boid {
  float visualRange;
  float* position;
  float accelaration;
  float* velocity;
} Boid;

#define NUM_DIM 3
#define WORLD_BOUNDARY 0 // obviously a temp thing, needs to match players FOV?

Boid* initializeBoids(int numBoids);

Boid* advanceBoids();

void printBoid(Boid* thisBoid);

void freeBoids(Boid* toFree, int numBoids);

#endif