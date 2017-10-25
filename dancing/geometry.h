#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cstdlib>
#include <string>

static int unitizeSingle(int x) {
  if (x > 0) {
    return 1;
  } else if (x == 0) {
    return 0;
  } else {
    return -1;
  }
}

class Point {
public:
	int x, y;
	Point(){
		x = y = 0;
	}
	Point(int a, int b){
		x = a;
		y = b;
	}
	Point operator +(const Point &b) const {
		return Point(x+b.x, y+b.y);
	}
	Point operator -(const Point &b) const {
		return Point(x-b.x, y-b.y);
	}
	Point operator -() const {
		return Point(-x, -y);
	}
	int operator *(const Point &b) const{
		return x*b.x+y*b.y;
	}
	int operator ^(const Point &b) const {
		return x*b.y-y*b.x;
	}
	Point invert() const {
		return Point(-y, x);
	}
        bool operator ==(const Point &b) const {
          return x == b.x && y == b.y;
        }

        bool operator !=(const Point &b) const {
          return !(*this == b);
        }

        // Not a real unit vector
        Point unitize() const {
          return {unitizeSingle(x), unitizeSingle(y)};
        }

        std::string toString() const {
          return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
        }
};

inline Point operator *(const int lambda, const Point &a){
	return Point(lambda*a.x, lambda*a.y);
}

inline Point operator *(const Point &a, const int lambda){
	return Point(lambda*a.x, lambda*a.y);
}

inline Point operator /(const Point &a, const int lambda){
	return Point(lambda/a.x, lambda/a.y);
}

// Manhattan distance
inline int manDist(Point a, Point b) {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

#endif
