#ifndef SPIRAL_ITERATOR_H
#define SPIRAL_ITERATOR_H

#include "geometry.hpp"

class SpiralIterator {
  void nextSquare();
  void nextDirection();
  public:
    Point curPoint;
    Point direction;
    int curSquareLength = 1;
    int curSideSquaresTraversed;
    SpiralIterator(int x, int y);
    Point getNext();
};

#endif
