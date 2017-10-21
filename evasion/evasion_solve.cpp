#include "evasion_solve.h"
#include "structs.h"
#include "constants.h"
#include "bresenham.h"
#include "../random/random.h"

#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

static Random r;

Position solvePreyRandom(GameState *state) {
  return { r.randInt(-1, 1), r.randInt(-1, 1) };
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

Position linePointPoint(Position a, Position b, Position c) {
  int lambda = ((b-a)*(c-b))/((b-a)*(b-a));
  return b+(lambda*(b-a));
}

Position solvePreyHeuristic(GameState *state) {
  // First check if we're in danger of dying
  Position closestPoint = linePointPoint(state->hunter, state->hunter + state->hunterDirection, state->prey);
  int dist = closestPoint.x * closestPoint.x + closestPoint.y * closestPoint.y;
  if (dist <= 16) {
    // WE ARE GONNA DIE!!!! ARRRRGGGGHHHH
    Position hunterDif = closestPoint - state->prey;
    if (hunterDif.x == 0 && hunterDif.y == 0) {
      return {-state->hunter.x, -state->hunter.y};
    } else {
      if (hunterDif.x > 0) {
        hunterDif.x = 1;
      } else {
        hunterDif.x = -1;
      }
      if (hunterDif.y > 0) {
        hunterDif.y = 1;
      } else {
        hunterDif.y = -1;
      }
      return {-hunterDif.x, -hunterDif.y};
    }
  }
  if (dist <= 25) {
    // We are too close so just stand still
    return {0, 0};
  }
  // Since we're not in danger of dying we compute our move to middle heuristic
  int highestDiff = 0;
  Position velocityToMove = {0, 0};
  int dx[4] = {1, 0, 1, -1}; // horizontal, vertical, diagonal, counterdiagonal
  int dy[4] = {0, 1, 1, 1};
  for (int dir = 0; dir < 4; dir++) {
    int rayLengthPositive = 0;
    for (int steps = 1; !state->isOccupied({state->prey.x + steps*dx[dir], state->prey.y + steps*dy[dir]}); steps++) rayLengthPositive++;
    int rayLengthNegative = 0;
    for (int steps = 1; !state->isOccupied({state->prey.x - steps * dx[dir], state->prey.y - steps * dy[dir]}); steps++) rayLengthNegative++;
    int diff = rayLengthNegative - rayLengthPositive;
    if (diff < 0) diff = -diff;
    if (diff > highestDiff) {
      highestDiff = diff;
      if (rayLengthPositive > rayLengthNegative) {
        velocityToMove = {dx[dir], dy[dir]};
      } else {
        velocityToMove = {-dx[dir], -dy[dir]};
      }
    }
  }
  return velocityToMove;
}

HunterMove solveHunterHeuristic(GameState *state) {
  int dx = state->prey.x - state->hunter.x;
  bool canBuildVertical = dx != 0 && (dx > 0) == (state->hunterDirection.x > 0);
  int dy = state->prey.y - state->hunter.y;
  bool canBuildHorizontal = dy != 0 && (dy > 0) == (state->hunterDirection.y > 0);
  if (!canBuildVertical && !canBuildHorizontal) {
    // If we aren't going towards the prey don't do anything.
    return {0};
  } else {
    bool shouldBuildVertical = canBuildVertical;
    bool shouldBuildHorizontal = canBuildHorizontal;
    if (shouldBuildHorizontal && shouldBuildVertical) {
      int verticalWidth = 0;
      for (int steps = 1; !state->isOccupied({state->hunter.x, state->hunter.y + steps*state->hunterDirection.y}); steps++) verticalWidth++;
      int horizontalWidth = 0;
      for (int steps = 1; !state->isOccupied({state->hunter.x + steps*state->hunterDirection.x, state->hunter.y}); steps++) horizontalWidth++;
      if (verticalWidth > horizontalWidth) {
        // Vertical width is larger which means we should build a horizontal wall
        shouldBuildVertical = false;
      } else {
        // The opposite
        shouldBuildHorizontal = false;
      }
    }
    if (shouldBuildHorizontal) {
      vector<int> indicesToDelete;
      for (int index = 0; index < state->walls.size(); index++) {
        Wall curWall = state->walls[index];
        if (curWall.start.y == curWall.end.y) {
          // It is a horizontal wall
          int walldy = curWall.start.y - state->hunter.y;
          bool movingAwayFromWall = (walldy > 0) != (state->hunterDirection.y > 0);
          if (movingAwayFromWall) {
            // The wall is now redundant
            indicesToDelete.push_back(index);
            // We can break as we should keep this property incrementally intact
            break;
          }
        }
      }
      return {1, indicesToDelete};
    } else {
      vector<int> indicesToDelete;
      for (int index = 0; index < state->walls.size(); index++) {
        Wall curWall = state->walls[index];
        if (curWall.start.x == curWall.end.x) {
          // It is a vertical wall
          int walldx = curWall.start.x - state->hunter.x;
          bool movingAwayFromWall = (walldx > 0) != (state->hunterDirection.x > 0);
          if (movingAwayFromWall) {
            // The wall is now redundant
            indicesToDelete.push_back(index);
            // We can break as we should keep this property incrementally intact
            break;
          }
        }
      }
      return {2, indicesToDelete};
    }
  }
}

int findWallBetween(GameState *state) {
  Position p1 = state->hunter;
  Position p2 = p1 + state->hunterDirection + state->hunterDirection + state->hunterDirection;
  vector<Position> pointsToConsider = bresenham(p1, p2);
  for (Position point : pointsToConsider) {
    int wallIndex = -1;
    if (state->isOccupied(point, wallIndex) && wallIndex >= 0) {
      return wallIndex;
    }
  }
  return -1;
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
  int wallBetweenIndex = findWallBetween(state);
  if (state->cooldownTimer > 0) {
    GameState newState(*state);
    HunterMove moveForHunter = {0}; // And default vector will be created
    if (wallBetweenIndex != -1) {
      vector<int> indicesToDelete(1, wallBetweenIndex);
      moveForHunter.indicesToDelete = indicesToDelete;
    }
    newState.makeMove(moveForHunter, moveForPrey);
    return miniMax(&newState, currentBest);
  }
  Score bestScore = {{0}, currentBest};
  HunterMove bestMove = {0};
  for (int type = 0; type <= 4; type++) {
    if (((type == 3 && state->hunterDirection.x == state->hunterDirection.y) ||
        (type == 4 && state->hunterDirection.x != state->hunterDirection.y)) &&
        state->bounce(state->hunter, state->hunterDirection).second == state->hunterDirection) {
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
      if (wallBetweenIndex != -1) {
        GameState firstNewState(*state);
        vector<int> indicesToDelete(1, wallBetweenIndex);
        HunterMove firstMoveForHunter = {type, indicesToDelete};
        firstNewState.makeMove(firstMoveForHunter, moveForPrey);
        Score firstCandidateScore = miniMax(&firstNewState, bestScore.score);
        if (firstCandidateScore.score > bestScore.score) {
          bestScore = {firstMoveForHunter, firstCandidateScore.score};
        }
      }
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
