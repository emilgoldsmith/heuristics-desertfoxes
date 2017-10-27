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

struct Pairing {
  // We should only need the point as it's assumed that all colors are distinct and all are represented in a pairing
  std::vector<Point> dancers; // This should be length c
};

struct Position {
  std::vector<Point> placements; // This should be length c
};

#endif
