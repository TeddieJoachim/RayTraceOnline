typedef struct Boid {
  float visualRange;
  float* position;
  float accelaration;
  float* velocity;
} Boid;

#define NUM_DIM 3

Boid* initializeBoids(int numBoids, float* worldBoundary);

Boid* advanceBoids();

void printBoid(Boid* thisBoid);

void freeBoids(Boid* toFree, int numBoids);