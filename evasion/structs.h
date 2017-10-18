#ifndef EVASION_STRUCTS_H
#define EVASION_STRUCTS_H

#include <vector>

struct Position {
  short int x;
  short int y;
};

struct Wall {
  Position start;
  Position end;
};

struct HunterMove {
  short int wallType;
  std::vector<short int> indicesToDelete;
};

#endif
