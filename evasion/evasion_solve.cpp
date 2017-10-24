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

vector<pair<Position, vector<int>>> getDeadlyPoints(GameState *initialState, int length) {
  // We first delete all walls between the hunter and the prey just to make sure we get deadlyPoints
  // where the hunter suddenly removes a wall and then takes us
  GameState stateCopy(*initialState);
  GameState *state = &stateCopy;
  for (int wallIndexBetween = findWallBetween(state, state->prey, state->hunter); wallIndexBetween != -1; wallIndexBetween = findWallBetween(state, state->prey, state->hunter)) {
    vector<int> indicesToDelete { wallIndexBetween };
    state->removeWalls(indicesToDelete);
  }
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

pair<Position, pair<int, int>> findSurvivalMove(Position start, vector<pair<Position, vector<int>>> *deadlyPoints, int ticksToSearch, GameState *state) {
  int arrLength = 2 * ticksToSearch + 10;
  int offset = ticksToSearch;
  bool **visited = new bool*[arrLength];
  for (int i = 0; i < arrLength; i++) {
    visited[i] = new bool[arrLength];
    for (int j = 0; j < arrLength; j++) {
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
        Position realPlace = start + step;
        auto it = deadlyPoints->begin();
        for (; it != deadlyPoints->end(); it++) {
          if (it->first == realPlace) break;
        }
        bool isDead = false;
        bool futureSafeSpot = true;
        int safeUntil = 0;
        if (it != deadlyPoints->end()) { // The current point is deadly
          for (int deadlyTime : it->second) {
            if (deadlyTime == 1 || deadlyTime == 2) { // And it's deadly at this simulated moment
              isDead = true;
              futureSafeSpot = false;
              break;
            }
            if (deadlyTime >= 1) {
              futureSafeSpot = false;
              safeUntil = deadlyTime - 1;
              // We can break because we know it's sorted so the above if statement would never occur if it hasn't already
              break;
            }
          }
        }
        if (!isDead && !state->isOccupied(realPlace)) {
          visited[step.x + offset][step.y + offset] = true;
          q.push({step, {step, 1}});
          if (futureSafeSpot) {
            // We also consider just standing still here for the rest of the time as it's safe
            q.push({step, {step, ticksToSearch}});
          }
          else {
            q.push({step, {step, safeUntil}});
          }
        }
      }
    }
  }
  pair<Position, pair<int, int>> bestMove = {{0, 0}, {100000, 0}};
  // the second pair is {distanceToCenter, tickNum (only valid when you would otherwise have been dead)

  while (!q.empty()) {
    pair<Position, pair<Position, int>> cur = q.front();
    q.pop();
    if (cur.second.second >= ticksToSearch) {
      // Time to evaluate the best move
      int distToCenter = getPathToCenter(state, start + cur.first).first;
      if (distToCenter < bestMove.second.first) {
        bestMove = {cur.second.first, {distToCenter, ticksToSearch}};
      }
      // We don't wanna search any further than movesToSearch
      continue;
    }

    bool hasValidMoves = false;

    for (int parity = -1; parity <= 1; parity += 2) {
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          Position move = {cur.first.x + parity * dx[i], cur.first.y + parity * dy[j]};
          if (visited[move.x + offset][move.y + offset]) continue;
          Position realPlace = start + move;
          auto it = deadlyPoints->begin();
          for (; it != deadlyPoints->end(); it++) {
            if (it->first == realPlace) break;
          }
          bool isDead = false;
          bool futureSafeSpot = true;
          int safeUntil = 0;
          if (it != deadlyPoints->end()) { // The current point is deadly
            for (int deadlyTime : it->second) {
              if (deadlyTime == cur.second.second + 2 || deadlyTime == cur.second.second + 3) { // And it's deadly at this simulated moment
                isDead = true;
                futureSafeSpot = false;
                break;
              }
              if (deadlyTime >= cur.second.second + 2) {
                futureSafeSpot = false;
                safeUntil = deadlyTime - 1;
                // We can break because we know it's sorted so the above if statement would never occur if it hasn't already
                break;
              }
            }
          }
          if (!isDead && !state->isOccupied(realPlace)) {
            hasValidMoves = true;
            visited[move.x + offset][move.y + offset] = true;
            q.push({move, {cur.second.first, cur.second.second + 2}});
            if (futureSafeSpot) {
              // We also consider just standing still here for the rest of the time as it's safe
              q.push({move, {cur.second.first, ticksToSearch}});
            }
            else {
              q.push({move, {cur.second.first, safeUntil}});
            }
          }
        }
      }
    }
    if (!hasValidMoves && cur.second.second > bestMove.second.second) {
      // This move can't go any further but it is also the move that stayed along the longest
      // up until now
      int distToCenter = getPathToCenter(state, start + cur.first).first;
      bestMove = {cur.second.first, {distToCenter, cur.second.second}};
    }
  }
  for (int i = 0; i < arrLength; i++) {
    delete[] visited[i];
  }
  delete[] visited;
  return bestMove;
}

Position solvePreyHeuristic(GameState *state) {
  bool hasWallBetween = findWallBetween(state, state->prey, state->hunter) != -1;
  int hunterPreyDistance = computeDistance(state->prey, state->hunter);
  int ticksToSearch = 45;
  if (hunterPreyDistance <= 2 * ticksToSearch * ticksToSearch) { // The 2 is sqrt2 squared (the amount of distance coverable in 1 timestep)
    // First check if we're in danger of dying if we're in the same box as hunter and pretty close
    vector<pair<Position, vector<int>>> deadlyPoints = getDeadlyPoints(state, ticksToSearch);
    return findSurvivalMove(state->prey, &deadlyPoints, ticksToSearch, state).first;
  }
  // Since we're not in danger of dying we just try to find the fastest way to the center
  vector<pair<Position, vector<int>>> deadlyPointsDummy;
  return findSurvivalMove(state->prey, &deadlyPointsDummy, 25, state).first;
}

vector<int> getPreyBoundingWalls(GameState *state) {
  // diagonal not considered since hunter only builds horizontal and vertical walls for now
  vector<int> boundingWalls;
  for (int i = 0; i < state->walls.size(); i++) {
    Wall wall = state->walls[i];
    Position pos = { state->prey.x, state->prey.y };
    int wallIndex = -1;
    // vertical wall
    if (wall.start.x == wall.end.x) {
      int dx = wall.start.x > state->prey.x ? 1 : -1;
      for (int x = state->prey.x; x >= 0 && x < 300; x += dx) {
        if (state->isOccupied({x, state->prey.y}), wallIndex) {
          break;
        }
      }
    // horizontal wall
    } else if (wall.start.y == wall.end.y) {
      int dy = wall.start.y > state->prey.y ? 1 : -1;
      for (int y = state->prey.y; y >= 0 && y < 300; y += dy) {
        if (state->isOccupied({state->prey.x, y}, wallIndex)) {
          break;
        }
      }
    }
    if (wallIndex == i) {
      boundingWalls.push_back(wallIndex);
    }
  }
  
  return boundingWalls;
}

Dimension getPreyBoxDimension(GameState *state, vector<int> boundingWalls) {
  int minX = 0, minY = 0, maxX = 299, maxY = 299;
  for (int i : boundingWalls) {
    Wall wall = state->walls[i];
    // vertical wall, update x
    if (wall.start.x == wall.end.x) {
      if (state->prey.x < wall.start.x) {
        maxX = wall.start.x;
      } else {
        minX = wall.start.x;
      }
    // horizontal wall, update y
    } else {
      if (state->prey.y < wall.start.y) {
        maxY = wall.start.y;
      } else {
        minY = wall.start.y;
      }
    }
  }
  return { minX, minY, maxX, maxY };
}

HunterMove solveHunterGuyu(GameState *state) {
  int wallType = 0;
  vector<int> wallCandidates;
  vector<int> indicesToDelete;

  Position hunterPreyDist = state->hunter - state->prey;
  bool hunterTowardsPreyX = hunterPreyDist.x * state->hunterDirection.x < 0;
  bool hunterTowardsPreyY = hunterPreyDist.y * state->hunterDirection.y < 0;

  // should consider building vertical
  if (abs(hunterPreyDist.x) <= 2) {
    if (hunterTowardsPreyX) {
      wallCandidates.push_back(1);
    // hunter is moving away
    } else {
      // calculate the number of ticks before hunter gets back to the same x
      int numTicks;
      GameState stateCopy(*state);
      do {
        HunterMove hm = { 0, {} };
        Position pm = { 0, 0 };
        if (!stateCopy.preyMoves) {
          pm = { 2, 2 };
        }
        stateCopy.makeMove(hm, pm);
        numTicks++;
        if (numTicks > state->cooldown) {
          break;
        }
      } while (stateCopy.hunter.x != state->hunter.x);
      // hunter is able to come back, destroy the old wall, and build a new wall immediately
      if (numTicks > state->cooldown) {
        wallCandidates.push_back(1);
      }
    }
  }
  // should consider building horizontal
  if (abs(hunterPreyDist.y) <= 2) {
    if (hunterTowardsPreyY) {
      wallCandidates.push_back(0);
    // hunter is moving away
    } else {
      // calculate the number of ticks before hunter gets back to the same y
      int numTicks = 0;
      GameState stateCopy(*state);
      do {
        HunterMove hm = { 0, {} };
        Position pm = { 0, 0 };
        if (!stateCopy.preyMoves) {
          pm = { 2, 2 };
        }
        stateCopy.makeMove(hm, pm);
        numTicks++;
        if (numTicks > state->cooldown) {
          break;
        }
      } while (stateCopy.hunter.y != state->hunter.y);
      // hunter is able to come back, destroy the old wall, and build a new wall immediately
      if (numTicks > state->cooldown) {
        wallCandidates.push_back(0);
      }
    }
  }

  // get best wall candidate (out of 2 at most)
  int minDim = 300;
  for (int wc : wallCandidates) {
    GameState stateCopy(*state);
    stateCopy.maxWalls += 1;
    Position pm = { 0, 0 };
    if (!stateCopy.preyMoves) {
      pm = { 2, 2 };
    }
    stateCopy.makeMove({ wc, {} }, pm);
    vector<int> boundingWalls = getPreyBoundingWalls(&stateCopy);
    Dimension preyBox = getPreyBoxDimension(&stateCopy, boundingWalls);
    int dim = min(preyBox.maxX - preyBox.minX, preyBox.maxY - preyBox.minY);
    if (dim < minDim) {
      minDim = dim;
      wallType = wc;
    }
  }

  vector<int> boundingWalls = getPreyBoundingWalls(state);
  // delete trap walls
  for (int i = 0; i < state->walls.size(); i++) {
    Wall wall = state->walls[i];
    Position hunterWallDist = state->hunter - wall.start;
    // if vertical wall
    if (wall.start.x == wall.end.x) {
      // if hunter is moving towards the wall
      if (hunterWallDist.x * state->hunterDirection.x < 0) {
        // if hunter is only 1 unit away from the wall
        // and moving towards the prey (trap wall)
        if (hunterWallDist.x <= 1 && hunterTowardsPreyX) {
          indicesToDelete.push_back(i);
          continue;
        }
      }
    // horizontal wall
    } else {
      // if hunter is moving towards the wall
      if (hunterWallDist.y * state->hunterDirection.y < 0) {
        // if hunter is only 1 unit away from the wall
        // and moving towards the prey (trap wall)
        if (hunterWallDist.y <= 1 && hunterTowardsPreyY) {
          indicesToDelete.push_back(i);
          continue;
        }
      }
    }
  }

  // delete non-bounding walls (after building the new wall)
  GameState stateCopy(*state);
  stateCopy.maxWalls += 1;
  Position pm = { 0, 0 };
  if (!stateCopy.preyMoves) {
    pm = { 2, 2 };
  }
  stateCopy.makeMove({ wallType, {} }, pm);
  vector<int> boundingWallsCopy = getPreyBoundingWalls(&stateCopy);
  for (int i = 0; i < state->walls.size(); i++) {
    // not already included in indiceToDelete
    bool alreadyDeleted = false;
    for (int deleted : indicesToDelete) {
      if (deleted == i) {
        alreadyDeleted = true;
        break;
      }
    }
    if (alreadyDeleted) {
      continue;
    }
    // not a bounding wall
    bool isBounding = false;
    for (int bound : boundingWallsCopy) {
      if (bound == i) {
        isBounding = true;
        break;
      }
    }
    if (!isBounding) {
      indicesToDelete.push_back(i);
    }
  }

  return { wallType, indicesToDelete };
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
