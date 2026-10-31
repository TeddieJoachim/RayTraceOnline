#ifndef BOIDS_H
#define BOIDS_H

typedef struct Boid {
  char visualRange;
  float* acceleration;
  float* position;
  float* velocity;
} Boid;

// MALLOCS Boids and their fields as well as shared boidPositions, and assigns initial values. TODO: Check malloc return vals
void initializeBoids();

void advanceBoids();

// returns base pointer to shared heap memory with WebAssembly
float* getBoidPositions();

int getNumBoids();
int getDim();
int howManyBytesInABoid();
void freeBoids();

#endif