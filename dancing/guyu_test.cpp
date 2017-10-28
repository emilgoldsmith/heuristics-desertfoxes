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

bool testAtFinalPosition(GameState *state) {
  vector<Point> finalPositive = {
    Point(1, 2), Point(3, 4), Point(4, 4)
  };
  vector<Point> finalNegative = {
    Point(2, 2), Point(3, 4), Point(4, 4)
  };

  return state->atFinalPositions(finalPositive) && !state->atFinalPositions(finalNegative);
}

bool testGetViableNextPositions(GameState *state) {
  Dancer dancers[2] = { state->dancers[0], state->dancers[2] };
  vector<Point> referenceViables[2] = {
    { Point(0, 2), Point(1, 1), Point(1, 2), Point(1, 3), Point(2, 2) },
    { Point(3, 4), Point(4, 4) }
  };

  for (int i = 0; i < 2; i++) {
    vector<Point> viable = state->getViableNextPositions(dancers[i]);
    if (viable.size() != referenceViables[i].size()) {
      return false;
    }
    for (int j = 0; j < viable.size(); j++) {
      if (viable[j] != referenceViables[i][j]) {
        return false;
      }
    }
  }

  return true;
}

int main() {
  int boardSize = 40;
  int numColors = 4;
  int numStars = 2;
  vector<Dancer> dancers = {
    {{7, 14}, 0}, {{11, 18}, 0}, {{13, 12}, 1}, {{12, 10}, 1}, {{8, 4}, 2}, {{21, 15}, 2}, {{1, 20}, 3}, {{3, 21}, 3}
  };
  vector<Point> stars = {
    {0, 0}, {5, 0}
  };
  vector<Point> finalPositions = {
    {1, 10}, {7, 15}, {2, 10}, {8, 15}, {3, 10}, {9, 15}, {4, 10}, {10, 15}
  };
  vector<DancerMove> dancerSrcDest;
  for (int i = 0; i < dancers.size(); i++) {
    dancerSrcDest.push_back({dancers[i].position, finalPositions[i]});
  }
  // int boardSize = 5;
  // int numColors = 3;
  // int numStars = 2;
  // vector<Dancer> dancers = {
  //   { Point(1, 2), 0 },
  //   { Point(3, 4), 1 },
  //   { Point(4, 4), 2 }
  // };
  // vector<Point> stars = {
  //   Point(2, 0), Point(4, 3), Point(0, 4)
  // };
  // vector<Point> finalPositions = {
  //   Point(0, 1), Point(0, 2), Point(0, 3)
  // };
  // vector<DancerMove> dancerSrcDest = {
  //   { dancers[1].position, finalPositions[1] },
  //   { dancers[2].position, finalPositions[2] },
  //   { dancers[0].position, finalPositions[0] }
  // };

  GameState state(boardSize, numColors, dancers, stars);
  // if (state.isConsistent()) {
  //   cout << "TEST PASSED: INITIALIZATION" << endl;
  // } else {
  //   cerr << "TEST FAILED: INITIALIZATION" << endl;
  // }

  // // visual check
  // state.display();

  // if (testSimulateOneMove(&state)) {
  //   cout << "TEST PASSED: SIMULATE ONE MOVE" << endl;
  // } else {
  //   cerr << "TEST FAILED: SIMULATE ONE MOVE" << endl;
  // }

  // if (testAtFinalPosition(&state)) {
  //   cout << "TEST PASSED: AT FINAL POSITION" << endl;
  // } else {
  //   cerr << "TEST FAILED: AT FINAL POSITION" << endl;
  // }

  // if (testGetViableNextPositions(&state)) {
  //   cout << "TEST PASSED: GET VIABLE NEXT POSITIONS" << endl;
  // } else {
  //   cerr << "TEST FAILED: GET VIABLE NEXT POSITIONS" << endl;
  // }

  auto move = state.simulate(dancerSrcDest);
}
