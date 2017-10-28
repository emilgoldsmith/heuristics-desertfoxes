/*
    x 0   1   2   3   4
  y +---+---+---+---+---+
  0 |   |   | * |   |   |
    +---+---+---+---+---+
  1 |[1]|   |   |   |   |
    +---+---+---+---+---+
  2 |[2]| 1 |   |   |   |
    +---+---+---+---+---+
  3 |[3]|   |   |   |   |
    +---+---+---+---+---+
  4 | * |   |   | 2 | 3 |
    +---+---+---+---+---+
*/

#include "game_state.h"

using namespace std;

int main() {
  int boardSize = 5;
  int numColors = 3;
  int numStars = 2;
  vector<Dancer> dancers = {
    { Point(1, 2), 0 },
    { Point(3, 4), 1 },
    { Point(4, 4), 2 }
  };
  vector<Point> stars = {
    Point(2, 0), Point(0, 4)
  };

  GameState state(boardSize, numColors, dancers, stars);
  if (!state.isConsistent()) {
    cerr << "Inconsistency at initialization" << endl;
  } else {
    cout << "Initialization success" << endl;
  }
  state.display();
}
