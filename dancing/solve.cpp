#include "geometry.h"
#include "solve.h"

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

/**
void pairingsToPositions() {
  board;
  for (pair : pairings) {

  }
}
*/
