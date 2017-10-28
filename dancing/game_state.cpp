#include "game_state.h"

using namespace std;

GameState::GameState(int boardSize, int numColors, vector<Dancer> dancers, vector<Point> stars)
:boardSize(boardSize), numColors(numColors), dancers(dancers), stars(stars) {
  // init board
  board = new int*[boardSize];
  for (int i = 0; i < boardSize; i++) {
    board[i] = new int[boardSize];
    for (int j = 0; j < boardSize; j++) {
      // 0 is empty
      board[i][j] = 0;
    }
  }
  // populate dancers
  for (auto &dancer : dancers) {
    // color becomes 1-indexed in board
    board[dancer.position.x][dancer.position.y] = dancer.color + 1;
  }
  // populate stars
  for (auto &star : stars) {
    board[star.x][star.y] = -1;
  }

  numSimulations = 0;
}

GameState::~GameState() {
  for (int i = 0; i < boardSize; i++) {
    delete [] board[i];
  }
  delete [] board;
}

bool GameState::isConsistent() {
  bool isConsistent = true;
  // check dancers
  for (auto &dancer : dancers) {
    if (board[dancer.position.x][dancer.position.y] != dancer.color + 1) {
      isConsistent = false;
      cerr << "Dancer list inconsistent with board" << endl;
    }
  }
  // check stars
  for (auto &star : stars) {
    if (board[star.x][star.y] != -1) {
      isConsistent = false;
      cerr << "Star list inconsistent with board" << endl;
    }
  }
  // check empty location
  int correctNumEmpty = boardSize * boardSize - stars.size() - dancers.size();
  int numEmpty = 0;
  for (int i = 0; i < boardSize; i++) {
    for (int j = 0; j < boardSize; j++) {
      if (board[i][j] == 0) {
        numEmpty++;
      }
    }
  }
  if (numEmpty != correctNumEmpty) {
    isConsistent = false;
    cerr << "Too many/too few empty spots on board" << endl;
  }

  return isConsistent;
}

void GameState::display() {
  for (int i = 0; i < boardSize; i++) {
    for (int j = 0; j < boardSize; j++) {
      cout << board[i][j] << "\t";
    }
    cout << endl;
  }
}
