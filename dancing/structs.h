#ifndef STRUCTS_H
#define STRUCTS_H

#include "geometry.hpp"

#include <vector>

struct Dancer {
  Point position;
  int color;
};

struct DancerMove {
  Point from;
  Point to;
};

struct EndLine {
  Point start;
  Point end;
};

struct ChoreographerMove {
  std::vector<std::vector<DancerMove>> dancerMoves;
  std::vector<EndLine> finalPosition;
};

#endif
