#include <chrono>
#include <random>

using namespace std;

Random::Random() {
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  generator = new default_random_engine(seed);
}

int Random::randInt(int a, int b) {
  uniform_int_distribution<int> distribution(a, b);
  return distribution(generator);
}
