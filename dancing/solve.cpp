#include "geometry.hpp"
#include "solve.h"
#include "client.h"
#include "structs.h"
#include "spiral_iterator.h"

#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

Point getFurthestPoint(const Point &center, const vector<Point> &points, bool getSecondFurthestPoint) {
  int maxDist = -1;
  Point furthestPoint = points[0];
  Point secondFurthestPoint = points[1];
  for (Point singlePoint : points) {
    int dist = manDist(center, singlePoint);
    if (dist >= maxDist) {
      maxDist = dist;
      secondFurthestPoint = furthestPoint;
      furthestPoint = singlePoint;
    }
  }
  if (getSecondFurthestPoint) {
    return secondFurthestPoint;
  }
  return furthestPoint;
}

struct MoveCloserUpdate {
  Point center;
  Point furthestPoint;

  int dist() {
    return manDist(center, furthestPoint);
  }
};

MoveCloserUpdate moveCloser(const Point &center, const Point &furthestPoint, const vector<Point> &points, bool onlyDoCheck = false) {
  Point directionToMove = (furthestPoint - center).unitize();
  int dx[] = {directionToMove.x, directionToMove.x, 0};
  int dy[] = {directionToMove.y, 0, directionToMove.y};
  Point curCenter = center;
  Point curFurthestPoint = furthestPoint;
  for (int i = 0; i < 3; i++) {
    while(1) {
      Point newCenter = curCenter + Point(dx[i], dy[i]);
      Point newFurthestPoint = getFurthestPoint(curCenter, points);
      bool shouldStop = newFurthestPoint != furthestPoint;
      if (shouldStop) {
        // We can't go any further in this direction
        break;
      } else {
        // Else we update our new values
        curCenter = newCenter;
        curFurthestPoint = newFurthestPoint;
      }
    }
  }
  return {curCenter, curFurthestPoint};
}

Point computeCenter(vector<Point> points) {
  // We start with the mean
  Point sum = {0, 0};
  for (Point singlePoint : points) {
    sum = sum + singlePoint;
  }
  // We initialize the center as the mean of the points (rounded of course)
  Point center = sum / points.size();
  Point furthestPoint = getFurthestPoint(center, points);
  bool movedCloser;
  MoveCloserUpdate latestCenterInformation = {center, furthestPoint};
  int bestCandidateDist = latestCenterInformation.dist();
  Point bestCandidateCenter = center;
  while (1) {
    latestCenterInformation = moveCloser(latestCenterInformation.center, latestCenterInformation.furthestPoint, points);
    if (latestCenterInformation.dist() < bestCandidateDist) {
      bestCandidateDist = latestCenterInformation.dist();
      bestCandidateCenter = latestCenterInformation.center;
      // Make sure we search on the second furthest point next
      latestCenterInformation.furthestPoint = getFurthestPoint(latestCenterInformation.center, points, true);
    } else {
      break;
    }
  }
  return bestCandidateCenter;
}

Point computeCenterBruteforce(const vector<Point> &points) {
  // Get the rectangle that encloses our points
  int minX = numeric_limits<int>::max();
  int minY = numeric_limits<int>::max();
  int maxX = numeric_limits<int>::min();
  int maxY = numeric_limits<int>::min();
  for (Point curPoint : points) {
    minX = min(minX, curPoint.x);
    minY = min(minY, curPoint.y);
    maxX = max(maxX, curPoint.x);
    maxY = max(maxY, curPoint.y);
  }
  // Now bruteforce all positions in this square
  int minFurthestDistance = numeric_limits<int>::max();
  Point bestCenter;
  for (int x = minX; x <= maxX; x++) {
    for (int y = minY; y <= maxY; y++) {
      Point curFurthestPoint = getFurthestPoint({x, y}, points);
      int minMaxDist = manDist({x, y}, curFurthestPoint);
      if (minMaxDist < minFurthestDistance) {
        bestCenter = {x, y};
        minFurthestDistance = minMaxDist;
      }
    }
  }
  return bestCenter;
}

vector<Pairing> getPairing(Client *client) {
  vector<Pairing> pairings;
  for (int i = 0; i < client->serverNumDancers; i++) {
    vector<Point> curPairing;
    for (int j = 0; j < client->serverNumColors; j++) {
      curPairing.push_back(client->dancers[i + j * client->serverNumDancers].position);
    }
    pairings.push_back({curPairing});
  }
  return pairings;
}

SolutionSpec pairingsToPositions(Client *client, vector<Pairing> pairings) {
  int n = client->serverBoardSize;
  vector<vector<bool>> board(n, vector<bool>(n, false));
  for (Point star : client->stars) {
    board[star.x][star.y] = true;
  }
  vector<DancerMove> dancerMapping;
  vector<EndLine> finalConfiguration;
  for (Pairing curPairing : pairings) {
    Point center = computeCenterBruteforce(curPairing.dancers);
    SpiralIterator it(center);
    int pointsOnNegative = curPairing.dancers.size() / 2;
    int pointsOnPositive = pointsOnNegative + curPairing.dancers.size() % 2;
    while (true) {
      Point cur = it.getNext();
      if (cur.x < 0 || cur.x >= n || cur.y < 0 || cur.y >= n) continue;
      // First try horizontal
      Position candidatePosition;
      for (int i = cur.x - pointsOnNegative; i < cur.x + pointsOnPositive && i >= 0 && i < n; i++) {
        if (board[i][cur.y]) break;
        candidatePosition.placements.push_back({i, cur.y});
      }
      if (candidatePosition.placements.size() == curPairing.dancers.size()) {
        // We found a proper placement!
        for (Point dancerFinal : candidatePosition.placements) {
          board[dancerFinal.x][dancerFinal.y] = true;
        }
        finalConfiguration.push_back({cur - Point(pointsOnNegative, 0), cur + Point(pointsOnPositive - 1, 0)});
#ifdef DEBUG
        if (curPairing.dancers.size() != candidatePosition.placements.size()) {
          cerr << "ERROR: curPairing and CandidatePosition are not the same size in pairingsToPositions" << endl;
        }
#endif
        for (int i = 0; i < curPairing.dancers.size(); i++) {
          dancerMapping.push_back({curPairing.dancers[i], candidatePosition.placements[i]});
        }
        break;
      }
      candidatePosition.placements.clear();
      for (int j = cur.y - pointsOnNegative; j < cur.y + pointsOnPositive && j >= 0 && j < n; j++) {
        if (board[cur.x][j]) break;
        candidatePosition.placements.push_back({cur.x, j});
      }
      if (candidatePosition.placements.size() == curPairing.dancers.size()) {
        // We found a proper placement!
        for (Point dancerFinal : candidatePosition.placements) {
          board[dancerFinal.x][dancerFinal.y] = true;
        }
        finalConfiguration.push_back({cur - Point(0, pointsOnNegative), cur + Point(0, pointsOnPositive - 1)});
#ifdef DEBUG
        if (curPairing.dancers.size() != candidatePosition.placements.size()) {
          cerr << "ERROR: curPairing and CandidatePosition are not the same size in pairingsToPositions" << endl;
        }
#endif
        for (int i = 0; i < curPairing.dancers.size(); i++) {
          dancerMapping.push_back({curPairing.dancers[i], candidatePosition.placements[i]});
        }
        break;
      }
    }
  }
  return {dancerMapping, finalConfiguration};
}

static Point extractClosestDancer(const Point &source, vector<Point> &dancers) {
  int indexOfClosest = 0;
  int closestDistance = manDist(source, dancers[0]);
  for (int i = 1; i < dancers.size(); i++) {
    int dist = manDist(source, dancers[i]);
    if (dist < closestDistance) {
      indexOfClosest = i;
      closestDistance = dist;
    }
  }
  Point ret = dancers[indexOfClosest];
  dancers.erase(dancers.begin() + indexOfClosest);
  return ret;
}

SolutionSpec solveManyPoints(Client *client, const vector<Dancer> &dancers, const vector<Point> &takenPositions) {
  // We first figure out which squares we can actually use
  int n = client->serverBoardSize;
  vector<vector<bool>> board(n, vector<bool>(n, false));
  for (Point star : client->stars) {
    board[star.x][star.y] = true;
  }
  for (Point takenPoint : takenPositions) {
    board[takenPoint.x][takenPoint.y] = true;
  }
  // Then we prep a few data structures
  vector<vector<Point>> dancersByColor(client->numColors);
  vector<Point> dancerPoints;
  dancerPoints.reserve(dancers.size());
  for (Dancer curDancer : dancers) {
    dancerPoints.push_back(curDancer.position);
    dancersByColor[curDancer.color].push_back(curDancer.position);
  }
  // Then we compute the center of all the points and let's go!
  Point center = computeCenterBruteforce(dancerPoints);
  vector<EndLine> finalConfiguration;
  vector<DancerMove> dancerMapping;
  int m = dancers.size();
  SpiralIterator it(center);
  int pointsOnNegative = client->numColors / 2;
  int pointsOnPositive = pointsOnNegative + client->numColors % 2;
  while (dancerMapping.size() < m) {
    Point cur = it.getNext();
    if (cur.x < 0 || cur.x >= n || cur.y < 0 || cur.y >= n) continue;
    // First try horizontal
    Position candidatePosition;
    for (int i = cur.x - pointsOnNegative; i < cur.x + pointsOnPositive && i >= 0 && i < n; i++) {
      if (board[i][cur.y]) break;
      candidatePosition.placements.push_back({i, cur.y});
    }
    if (candidatePosition.placements.size() == client->numColors) {
      // We found a proper placement!
      for (Point dancerFinal : candidatePosition.placements) {
        board[dancerFinal.x][dancerFinal.y] = true;
      }
      finalConfiguration.push_back({cur - Point(pointsOnNegative, 0), cur + Point(pointsOnPositive - 1, 0)});
#ifdef DEBUG
      if (client->numColors != candidatePosition.placements.size()) {
        cerr << "ERROR: numColors and CandidatePosition are not the same size in solveManyPoints" << endl;
      }
#endif
      for (int i = 0; i < client->numColors; i++) {
        // Here we could probably do a small improvement so we don't statically consider colors but actually
        // check if another color placement on the strip would be better
        Point closestDancer = extractClosestDancer(candidatePosition.placements[i], dancersByColor[i]);
        dancerMapping.push_back({closestDancer, candidatePosition.placements[i]});
      }
      continue;
    }
    candidatePosition.placements.clear();
    for (int j = cur.y - pointsOnNegative; j < cur.y + pointsOnPositive && j >= 0 && j < n; j++) {
      if (board[cur.x][j]) break;
      candidatePosition.placements.push_back({cur.x, j});
    }
    if (candidatePosition.placements.size() == client->numColors) {
      // We found a proper placement!
      for (Point dancerFinal : candidatePosition.placements) {
        board[dancerFinal.x][dancerFinal.y] = true;
      }
      finalConfiguration.push_back({cur - Point(0, pointsOnNegative), cur + Point(0, pointsOnPositive - 1)});
#ifdef DEBUG
      if (client->numColors != candidatePosition.placements.size()) {
        cerr << "ERROR: numColors and CandidatePosition are not the same size in solveManyPoints" << endl;
      }
#endif
      for (int i = 0; i < client->numColors; i++) {
        // Here we could probably do a small improvement so we don't statically consider colors but actually
        // check if another color placement on the strip would be better
        Point closestDancer = extractClosestDancer(candidatePosition.placements[i], dancersByColor[i]);
        dancerMapping.push_back({closestDancer, candidatePosition.placements[i]});
      }
      continue;
    }
  }
  return {dancerMapping, finalConfiguration};
}

vector<Point> dummyPlaceStars(Client *client) {
  int n = client->serverBoardSize;
  int k = client->serverNumDancers;
  int c = client->serverNumColors;
  // Get dancers as a vector of points
  vector<Point> dancers;
  for (Dancer curDancer : client->dancers) {
    dancers.push_back(curDancer.position);
  }
  int x = 0, y = 0;
  vector<Point> starsToPlace;

  while (k--) {
    while (Point(x, y).in(dancers)) {
      x++;
      if (x >= n) {
        x = 0;
        y += c + 1;
      }
    }

    starsToPlace.push_back({x, y});
    x += c + 1;
    if (x >= n) {
      x = 0;
      y += c + 1;
    }
  }
  return starsToPlace;
}


ChoreographerMove dummyGetChoreographerMove(Client *client) {
  // We are just going to make some garbage as it's too comprehensive actually finding a solution
  ChoreographerMove move;
  for (int i = 0; i < 3; i++) {
    vector<DancerMove> singleStep;
    for (Dancer singleDancer : client->dancers) {
      singleStep.push_back({singleDancer.position, singleDancer.position + Point(1, 0)});
    }
    move.dancerMoves.push_back(singleStep);
  }
  for (int i = 0; i < client->serverNumDancers; i++) {
    move.finalPosition.push_back({{i, i}, {i + client->serverNumColors, i}});
  }
  return move;
}

