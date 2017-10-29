#ifndef OUR_RANDOM_H
#define OUR_RANDOM_H

#include <chrono>
#include <random>

class Random {
  public:
    std::default_random_engine generator;
    Random();

    int randInt(int a, int b); // the values are inclusive
};

#endif
