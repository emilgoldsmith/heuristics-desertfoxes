#include "structs.h"

#include <vector>
#include <cstdlib>
#include <algorithm>

using namespace std;

/**
 * This is only a slightly modified copy/paste from architects algorithm
 */
vector<Position> bresenham(Position p1, Position p2) {
  short int x0 = p1.x;
  short int x1 = p2.x;
  short int y0 = p1.y;
  short int y1 = p2.y;

  vector<Position> points;
  bool steep = abs(y1-y0) > abs(x1-x0);

  if(steep){
    swap(x0, y0);
    swap(x1, y1);
  }
  if(x0 > x1){
    swap(x0, x1);
    swap(y0, y1);
  }

  short int deltax = x1-x0;
  short int deltay = abs(y1-y0);
  short int error = deltax / 2;
  short int y = y0;
  short int ystep;
  if(y0 < y1){
    ystep = 1;
  } else {
    ystep = -1;
  }

  for(short int x = x0; x <= x1; x++){
    if(steep){
      points.push_back({y,x});
    } else {
      points.push_back({x, y});
    }
    error -= deltay;
    if(error < 0){
      y += ystep;
      error += deltax;
    }
  }

  return points;
}
