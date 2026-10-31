#include "boids.h"

int main() 
{
  int numBoids = 5;
  Boid* themBoids = initializeBoids(5); 

  for (int i = 0; i < numBoids; i++) printBoid(&themBoids[i]);

  freeBoids(&themBoids[0], 5); // eventually the javascript should call this
  return 0;
}