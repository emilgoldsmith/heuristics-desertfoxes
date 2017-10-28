/*
    x 0   1   2   3   4
  y +---+---+---+---+---+
  0 |   |   | * |   |   |
    +---+---+---+---+---+
  1 |[1]|   |   |   |   |
    +---+---+---+---+---+
  2 |[2]| 1 |   |   |   |
    +---+---+---+---+---+
  3 |[3]|   |   |   | * |
    +---+---+---+---+---+
  4 | * |   |   | 2 | 3 |
    +---+---+---+---+---+
*/

#include "game_state.h"

using namespace std;

bool testSimulateOneMove(GameState *state) {
  bool success = true;
  vector<Dancer> dancersBackup = state->cloneDancers();

  // success scenario: 1, 2, 3 all move to the left
  vector<Point> nextPositions = {
    Point(0, 2), Point(2, 4), Point(3, 4)
  };
  if (!state->simulateOneMove(nextPositions)) {
    cerr << "Legal dancer moves are rejected" << endl;
    success = false;
  } else {
    // restore game state for the next test
    state->dancers = dancersBackup;
    state->resetBoard();
    state->fillBoard(state->dancers, state->stars);
  }

  // failure scenario #1 : manhattan distance > 1
  nextPositions = {
    Point(0, 1), Point(2, 4), Point(3, 4)
  };
  if (state->simulateOneMove(nextPositions)) {
    cerr << "Dancer moves more than 1 manhattan distance away, but considered legal" << endl;
    success = false;
    // restore game state for the next test
    state->dancers = dancersBackup;
    state->resetBoard();
    state->fillBoard(state->dancers, state->stars);
  }

  // failure scenario #2 : move out of bounds > 1
  nextPositions = {
    Point(0, 2), Point(2, 4), Point(4, 5)
  };
  if (state->simulateOneMove(nextPositions)) {
    cerr << "Dancer moves outside of the board, but considered legal" << endl;
    success = false;
    // restore game state for the next test
    state->dancers = dancersBackup;
    state->resetBoard();
    state->fillBoard(state->dancers, state->stars);
  }

  // failure scenario #3 : dancers moves onto star
  nextPositions = {
    Point(0, 2), Point(2, 4), Point(4, 3)
  };
  if (state->simulateOneMove(nextPositions)) {
    cerr << "Dancer moves onto star, but considered legal" << endl;
    success = false;
    // restore game state for the next test
    state->dancers = dancersBackup;
    state->resetBoard();
    state->fillBoard(state->dancers, state->stars);
  }

  // failure scenario #4: more than one dancer move to the same position
  nextPositions = {
    Point(0, 2), Point(3, 4), Point(3, 4)
  };
  if (state->simulateOneMove(nextPositions)) {
    cerr << "Dancers clash, but considered legal" << endl;
    success = false;
  }

  return success;
}

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
    Point(2, 0), Point(4, 3), Point(0, 4)
  };

  GameState state(boardSize, numColors, dancers, stars);
  if (state.isConsistent()) {
    cout << "TEST PASSED: INITIALIZATION" << endl;
  } else {
    cerr << "TEST FAILED: INITIALIZATION" << endl;
  }

  // visual check
  state.display();

  if (testSimulateOneMove(&state)) {
    cout << "TEST PASSED: SIMULATE ONE MOVE" << endl;
  } else {
    cerr << "TEST FAILED: SIMULATE ONE MOVE" << endl;
  }
}
