#include "evasion_solve.h"
#include "structs.h"
#include "constants.h"

#include <cstdlib>
#include <ctime>
#include <algorithm>

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

Position solvePreyHeuristic(GameState *state) {
  srand(time(0));
  return { rand() % 2, rand() % 2 };
}

Score miniMax(GameState *state, int currentBest) {
  if (state->gameOver) {
    return {{0}, state->score};
  }
  if (state->score >= currentBest) {
    return {{0}, INF};
  }
  Position moveForPrey = {2, 2};
  if (state->preyMoves) {
    moveForPrey = solvePreyRandom(state);
  }
  if (state->cooldownTimer > 0) {
    GameState newState(*state);
    // Implement something here that checks if there's a wall between the two and then delete it or don't
    HunterMove moveForHunter = {0}; // And default vector will be created
    newState.makeMove(moveForHunter, moveForPrey);
    return miniMax(&newState, currentBest);
  }
  Score bestScore = {{0}, currentBest};
  HunterMove bestMove = {0};
  for (int type = 0; type <= 4; type++) {
    // Check here if there's a wall in front of us and do something

    if (((type == 3 && state->hunterDirection.x == state->hunterDirection.y) ||
        (type == 4 && state->hunterDirection.x != state->hunterDirection.y)) &&
        state->bounce(state->hunter, state->hunterDirection).second == hunterDirection) {
      continue;
    }

    if (state->walls.size() == state->maxWalls && type > 0) {
      // We need to delete a wall
      for (int wallIndex = 0; wallIndex < state->walls.size(); wallIndex++) {
        vector<int> indicesToDelete(1, wallIndex);
        GameState newState(*state);
        HunterMove moveForHunter = {type, indicesToDelete};
        newState.makeMove(moveForHunter, moveForPrey);
        Score candidateScore = miniMax(&newState, bestScore.score);
        if (candidateScore.score > bestScore.score) {
          bestScore = {moveForHunter, candidateScore.score};
        }
      }
    } else {
      GameState newState(*state);
      HunterMove moveForHunter = {type};
      newState.makeMove(moveForHunter, moveForPrey);
      Score candidateScore = miniMax(&newState, bestScore.score);
      if (candidateScore.score > bestScore.score) {
        bestScore = {moveForHunter, candidateScore.score};
      }
    }
  }
  return bestScore;
}
