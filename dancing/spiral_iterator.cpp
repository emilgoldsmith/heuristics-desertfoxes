#include "geometry.hpp"
#include "spiral_iterator.h"

#ifdef DEBUG
#include <iostream>
#endif

SpiralIterator::SpiralIterator(int x, int y): curPoint({x, y}) {}
SpiralIterator::SpiralIterator(Point start): curPoint(start) {}

Point SpiralIterator::getNext() {
  Point toReturn = curPoint;
  if (curSquareLength == 1) {
    nextSquare();
  } else { // It's not the first point so now we're actually in a proper square
    curPoint = curPoint + direction;
    curSideSquaresTraversed++;
    if (curSideSquaresTraversed == curSquareLength - 1) {
      nextDirection();
    }
  }
  return toReturn;
}

void SpiralIterator::nextSquare() {
  curPoint = curPoint + Point(-1, 1);
  curSquareLength += 2;
  direction = {1, 0};
  curSideSquaresTraversed = 0;
}

void SpiralIterator::nextDirection() {
  curSideSquaresTraversed = 0;
  if (direction.x == 1) {
    direction = {0, -1};
  } else if (direction.y == -1) {
    direction = {-1, 0};
  } else if (direction.x == -1) {
    direction = {0, 1};
  } else if (direction.y == 1) {
    nextSquare();
  } else {
#ifdef DEBUG
    std::cerr << "SpiralIterator ERROR: nextDirection reached else statement" << std::endl;
#endif
  }
}
