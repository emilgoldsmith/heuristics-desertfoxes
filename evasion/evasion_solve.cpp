#include "evasion_solve.h"
#include "structs.h"
#include "constants.h"
#include "bresenham.h"
#include "../random/random.h"

#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <utility>
#include <queue>

using namespace std;

static Random r;

Position solvePreyRandom(GameState *state) {
  return { r.randInt(-1, 1), r.randInt(-1, 1) };
}

HunterMove solveHunterRandom(GameState *state) {
  if (state->cooldownTimer > 0) {
    return {0};
  }
  bool rm = r.randInt(0, 1) == 1;
  int numWalls = state->walls.size();
  int wallTypeToAdd = r.randInt(0, 4);
  int wallToRemove = r.randInt(0, numWalls - 1);

  if (rm && numWalls > 0) {
    return { wallTypeToAdd, { wallToRemove } };
  }
  return { wallTypeToAdd, {} };
}

Position linePointPoint(Position a, Position b, Position c) {
  int lambda = ((b-a)*(c-b))/((b-a)*(b-a));
  return b+(lambda*(b-a));
}

int computeDistance(Position a, Position b) {
  Position vectorDistance = a - b;
  return vectorDistance * vectorDistance;
}

int findWallBetween(GameState *state, Position p1, Position p2) {
  vector<Position> pointsToConsider = bresenham(p1, p2);
  for (Position point : pointsToConsider) {
    int wallIndex = -1;
    if (state->isOccupied(point, wallIndex) && wallIndex >= 0) {
      return wallIndex;
    }
  }
  return -1;
}

pair<int, Position> getPathToCenter(GameState *state, Position p) {
  int highestDiff = 0;
  Position velocityToMove = {0, 0};
  int dx[4] = {1, 0, 1, -1}; // horizontal, vertical, diagonal, counterdiagonal
  int dy[4] = {0, 1, 1, 1};
  for (int dir = 0; dir < 4; dir++) {
    int rayLengthPositive = 0;
    for (int steps = 1; !state->isOccupied({p.x + steps*dx[dir], p.y + steps*dy[dir]}); steps++) rayLengthPositive++;
    int rayLengthNegative = 0;
    for (int steps = 1; !state->isOccupied({p.x - steps * dx[dir], p.y - steps * dy[dir]}); steps++) rayLengthNegative++;
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
  return {highestDiff, velocityToMove};
}

vector<Position> getDeadlyNeighbours(Position a) {
  // Yes this could be done without the checks and just hardcoded but no need to and this is clean
  vector<Position> deadlyNeighbours;
  for (int i = -4; i <= 4; i++) {
    for (int j = -4; j <= 4; j++) {
      Position dif = {i, j};
      int dist = dif * dif;
      if (dist <= 16) {
        deadlyNeighbours.push_back(a + dif);
      }
    }
  }
  return deadlyNeighbours;
}

vector<pair<Position, vector<int>>> getDeadlyPoints(GameState *state, int length) {
  // We simulate length ticks for the hunter to see assuming no extra walls all the deadly positions
  pair<Position, Position> hunterPositionAndVelocity = {state->hunter, state->hunterDirection};
  vector<pair<Position, vector<int>>> deadlyPoints;
  vector<Position> firstNeighbours = getDeadlyNeighbours(hunterPositionAndVelocity.first);
  for (Position neighbour : firstNeighbours) {
    deadlyPoints.push_back({neighbour, {0}});
  }
  for (int stepDistance = 1; stepDistance <= length; stepDistance++) {
    hunterPositionAndVelocity = state->bounce(hunterPositionAndVelocity.first, hunterPositionAndVelocity.second);
    vector<Position> deadlyNeighbours = getDeadlyNeighbours(hunterPositionAndVelocity.first);
    for (Position deadly : deadlyNeighbours) {
      auto it = deadlyPoints.begin();
      for (; it != deadlyPoints.end(); it++) {
        if (it->first == deadly) break;
      }
      if (it == deadlyPoints.end()) { // It is a new deadly point
        deadlyPoints.push_back({deadly, {stepDistance}});
      } else {
        // Record that it was also deadly here
        it->second.push_back(stepDistance);
      }
    }
  }
  return deadlyPoints;
}

pair<Position, int> findSurvivalMove(Position start, vector<pair<Position, vector<int>>> *deadlyPoints, int movesToSearch, GameState *state) {
  bool visited[50][50];
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 50; j++) {
      visited[i][j] = false;
    }
  }
  queue<pair<Position, pair<Position, int>>> q; // pair.second.first is the first move made, pair.second.second is the steps currently taken
  // Initialize the queue with all the possible initial moves, that will then afterwards all be evaluated
  int dx[4] = {1, 0, 1, -1}; // horizontal, vertical, diagonal, counterdiagonal
  int dy[4] = {0, 1, 1, 1};
  for (int parity = -1; parity <= 1; parity += 2) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        Position step = {parity * dx[i], parity * dy[j]};
        visited[step.x + 20][step.y + 20] = true;
        Position realPlace = start + step;
        auto it = deadlyPoints->begin();
        for (; it != deadlyPoints->end(); it++) {
          if (it->first == realPlace) break;
        }
        bool isDead = false;
        if (it != deadlyPoints->end()) { // The current point is deadly
          for (int deadlyTime : it->second) {
            if (deadlyTime == 1 || deadlyTime == 2) {
              isDead = true;
            }
          }
        }
        if (!isDead && !state->isOccupied(realPlace)) {
          q.push({step, {step, 1}});
        }
      }
    }
  }
  pair<Position, int> bestMove = {{0, 0}, 100000};
  // Position pair.second values represent possible escape and distance to center (/ highestDiff of rays)

  while (!q.empty()) {
    pair<Position, pair<Position, int>> cur = q.front();
    q.pop();
    if (cur.second.second == 1 + 2 * movesToSearch || q.empty()) { // include the if q.empty() so that if we're screwed the move that got us the furthest while still alive is considered
      // Time to evaluate the best move
      int distToCenter = getPathToCenter(state, start + cur.first).first;
      if (distToCenter < bestMove.second) {
        bestMove = {cur.second.first, distToCenter};
      }
      // We don't wanna search any further than movesToSearch
      continue;
    }

    for (int parity = -1; parity <= 1; parity += 2) {
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          Position move = {cur.first.x + parity * dx[i], cur.first.y + parity * dy[j]};
          if (visited[move.x + 20][move.y + 20]) continue;
          visited[move.x + 20][move.y + 20] = true;
          Position realPlace = start + move;
          auto it = deadlyPoints->begin();
          for (; it != deadlyPoints->end(); it++) {
            if (it->first == realPlace) break;
          }
          bool isDead = false;
          if (it != deadlyPoints->end()) { // The current point is deadly
            for (int deadlyTime : it->second) {
              if (deadlyTime == cur.second.second + 2 || deadlyTime == cur.second.second + 3) { // And it's deadly at this simulated moment
                isDead = true;
              }
            }
          }
          if (!isDead && !state->isOccupied(realPlace)) {
            q.push({move, {cur.second.first, cur.second.second + 2}});
          }
        }
      }
    }
  }
  return bestMove;
}

Position solvePreyHeuristic(GameState *state) {
  bool hasWallBetween = findWallBetween(state, state->prey, state->hunter) != -1;
  int hunterPreyDistance = computeDistance(state->prey, state->hunter);
  int movesToSearch = 39;
  if (!hasWallBetween && hunterPreyDistance <= 2 * movesToSearch * movesToSearch) { // The 2 is sqrt2 squared (the amount of distance coverable in 1 timestep)
    // First check if we're in danger of dying if we're in the same box as hunter and pretty close
    vector<pair<Position, vector<int>>> deadlyPoints = getDeadlyPoints(state, movesToSearch);
    return findSurvivalMove(state->prey, &deadlyPoints, movesToSearch >> 1, state).first;
  }
  // Since we're not in danger of dying we compute our move to middle heuristic
  return getPathToCenter(state, state->prey).second;
}

HunterMove solveHunterHeuristic(GameState *state) {
  if (state->cooldownTimer > 0) {
    return {0};
  }
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
      if ((state->preyMoves && abs(dy) > 2) || (!state->preyMoves && abs(dy) > 1)) {
        // We can wait until next move with building the wall
        return {0};
      }
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
      if ((state->preyMoves && abs(dx) > 2) || (!state->preyMoves && abs(dx) > 1)) {
        // We can wait until next move with building the wall
        return {0};
      }
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
