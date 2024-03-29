#include "random.h"

#include <chrono>
#include <random>

using namespace std;

Random::Random() {
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  generator = default_random_engine(seed);
}

int Random::randInt(int a, int b) {
  uniform_int_distribution<int> distribution(a, b);
  return distribution(generator);
}

double Random::randDouble(double a, double b) {
  uniform_real_distribution<double> distribution(a, b);
  return distribution(generator);
}
