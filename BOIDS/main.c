#include "boids.h"

int main() 
{
  // Initialize BOIDS as array of BOIDS
  int numBoids = 5;
  float worldBoundary[] = {-1.0, 1.0, 1.0};
  Boid* themBoids = initializeBoids(5, &worldBoundary[0]);

  for (int i = 0; i < numBoids; i++) printBoid(&themBoids[i]);

  freeBoids(&themBoids[0], 5);
  return 0; // Should this return a pointer to the array of BOID positions? questions for webasm part
}