#ifndef EVASION_STRUCTS_H
#define EVASION_STRUCTS_H

#include <vector>

struct Position {
  int x;
  int y;

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
  int wallType;
  std::vector<int> indicesToDelete;
};

struct Score {
  HunterMove move;
  int score;
};

#endif
