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
  for (int i = 0; i < 3; i++) {
    Point newCenter = center + Point(dx[i], dy[i]);
    Point newFurthestPoint = getFurthestPoint(newCenter, points);
    if (newFurthestPoint == furthestPoint) {
      // We moved closer and it's still the closest point so we update and keep going
      return {newCenter, newFurthestPoint};
    } else { // We check whether this enters a loop where they just alternate between being the furthestPoint
      if (!onlyDoCheck) {
        MoveCloserUpdate newCenterTest = moveCloser(newCenter, newFurthestPoint, points, true);
        if (newCenterTest.furthestPoint == furthestPoint) {
          // It is a loop so this is not worth anything
          continue;
        } else {
          // It is a valid new furthest point to check
          return {newCenter, newFurthestPoint};
        }
      } else {
        // This is a depth 2 call to the function that just wants to know whether there is a loop
        // And here we want to find a direction where we can actually go closer while it still being the furthest point
        continue;
      }
    }
  }
  return {center, furthestPoint};
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
  do {
    MoveCloserUpdate newUpdate = moveCloser(latestCenterInformation.center, latestCenterInformation.furthestPoint, points);
    movedCloser = newUpdate.center != latestCenterInformation.center;
    latestCenterInformation = newUpdate;
  } while(movedCloser);
  return latestCenterInformation.center;
}


/**
void pairingsToPositions() {
  board;
  for (pair : pairings) {

  }
}
*/
