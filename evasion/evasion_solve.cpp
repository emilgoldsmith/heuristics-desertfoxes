#include "evasion_solve.h"

#include <cstdlib>
#include <ctime>

using namespace std;

Position solvePreyRandom(GameState *state) {
  srand(time(0));
  return { rand() % 2, rand() % 2 };
}

HunterMove solveHunterRandom(GameState *state) {
  srand(time(0));
  bool rm = rand() % 2 == 1;
  int numWalls = state->walls.size();
  int wallTypeToAdd = rand() % 5;
  short int wallToRemove = (short) (rand() % numWalls);

  if (rm && numWalls > 0) {
    return { wallTypeToAdd, { wallToRemove } };
  }
  return { wallTypeToAdd, {} };
}
