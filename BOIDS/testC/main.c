#include "../src/boids.h"

int main(void) {
  initializeBoids();

  advanceBoids();

  freeBoids();

  return 0;
}