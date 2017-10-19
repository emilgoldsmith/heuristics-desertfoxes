#ifndef EVASION_STRUCTS_H
#define EVASION_STRUCTS_H

#include <vector>

struct Position {
  short int x;
  short int y;

  Position operator+(const Position& a) {
    return {x + a.x, y + a.y};
  }

  bool operator==(const Position& a) {
    return a.x == x && a.y == y;
  }
};

struct Wall {
  Position start;
  Position end;
  Position creationPoint;
};

struct HunterMove {
  short int wallType;
  std::vector<short int> indicesToDelete;
};

#endif
