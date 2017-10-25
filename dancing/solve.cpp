#include "geometry.h"

#include <vector>

using namespace std;

Point getFurthestPoint(Point center, vector<Point> points) {
  int maxDist = -1;
  Point furthestPoint;
  for (Point singlePoint : points) {
    int dist = manDist(center, singlePoint);
    if (dist > maxDist) {
      maxDist = dist;
      furthestPoint = singlePoint;
    }
  }
  return furthestPoint;
}

struct MoveCloserUpdate {
  Point center;
  Point furthestPoint;
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
      Point newFurthestPoint = getFurthestPoint(newCenter, points);
      if (newFurthestPoint == curFurthestPoint) {
        // We moved closer and it's still the closest point so we update and keep going
        curCenter = newCenter;
        curFurthestPoint = furthestPoint;
      } else {
        // We can't go any further in this direction
        break;
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
  while (1) {
    MoveCloserUpdate newUpdate = moveCloser(latestCenterInformation.center, latestCenterInformation.furthestPoint, points);
    movedCloser = manDist(newUpdate.center, newUpdate.furthestPoint) < manDist(latestCenterInformation.center, latestCenterInformation.furthestPoint);
    if (movedCloser) {
      latestCenterInformation = newUpdate;
    } else {
      break;
    }
  }
  return latestCenterInformation.center;
}


/**
void pairingsToPositions() {
  board;
  for (pair : pairings) {

  }
}
*/
