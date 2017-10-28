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
  // fill board
  fillBoard(dancers, stars);

  numSimulations = 0;
}

GameState::~GameState() {
  for (int i = 0; i < boardSize; i++) {
    delete [] board[i];
  }
  delete [] board;
}

void GameState::resetBoard() {
  for (int i = 0; i < boardSize; i++) {
    for (int j = 0; j < boardSize; j++) {
      board[i][j] = 0;
    }
  }
}

void GameState::fillBoard(vector<Dancer> &dancerList, vector<Point> &starList) {
  // populate dancers
  for (auto &dancer : dancerList) {
    // color becomes 1-indexed in board
    board[dancer.position.y][dancer.position.x] = dancer.color + 1;
  }
  // populate stars
  for (auto &star : starList) {
    board[star.y][star.x] = -1;
  }
}

vector<Dancer> GameState::cloneDancers() {
  vector<Dancer> dancersClone;
  for (auto &dancer : dancers) {
    dancersClone.push_back({
      Point(dancer.position.x, dancer.position.y),
      dancer.color
    });
  }
  return dancersClone;
}

bool GameState::isConsistent() {
  bool isConsistent = true;
  // check dancers
  for (auto &dancer : dancers) {
    if (board[dancer.position.y][dancer.position.x] != dancer.color + 1) {
      isConsistent = false;
      #ifdef DEBUG
        cerr << "Dancer list inconsistent with board" << endl;
      #endif
    }
  }
  // check stars
  for (auto &star : stars) {
    if (board[star.y][star.x] != -1) {
      isConsistent = false;
      #ifdef DEBUG
        cerr << "Star list inconsistent with board" << endl;
      #endif
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
    #ifdef DEBUG
      cerr << "Too many/too few empty spots on board" << endl;
    #endif
  }

  return isConsistent;
}

void GameState::display() {
  for (int i = 0; i < boardSize; i++) {
    for (int j = 0; j < boardSize; j++) {
      if (board[i][j] == -1) {
        cout << "*" << " ";
      } else {
        cout << board[i][j] << " ";
      }
    }
    cout << endl;
  }
  cout << endl;
}

bool GameState::withinBounds(Point position) {
  if (position.x < 0 || position.x >= boardSize) {
    return false;
  }
  if (position.y < 0 || position.y >= boardSize) {
    return false;
  }
  return true;
}

bool GameState::simulateOneMove(vector<Point> &nextPositions) {
  bool success = true;
  vector<Dancer> dancersBackup = cloneDancers();

  for (int i = 0; i < dancers.size(); i++) {
    Dancer dancer = dancers[i];
    Point nextPosition = nextPositions[i];
    // check for errors
    if (manDist(dancer.position, nextPosition) > 1) {
      #ifdef DEBUG
      cerr << "Dancer attempted to move more than 1 manhattan distance away" << endl;
      #endif // DEBUG
      success = false;
    }
    if (!withinBounds(nextPosition)) {
      #ifdef DEBUG
      cerr << "Dancer attempted to move outside the board" << endl;
      #endif // DEBUG
      success = false;
      continue; // otherwise segfault
    }
    if (board[nextPosition.y][nextPosition.x] == -1) {
      #ifdef DEBUG
      cerr << "Dancer attempted to move to a star" << endl;
      #endif // DEBUG
      success = false;
    }
    // clear dancer's original position and set dancer new position
    board[dancer.position.y][dancer.position.x] = 0;
    dancers[i].position = nextPosition;
  }

  // update board with new dancer positions
  for (int i = 0; i < dancers.size(); i++) {
    board[dancers[i].position.y][dancers[i].position.x] = dancers[i].color + 1;
  }

  // final consistency check (e.g. if multiple dancers moved to same position)
  if (!isConsistent()) {
    success = false;
  }

  // restore game state if simulation fails
  if (!success) {
    dancers = dancersBackup;
    resetBoard();
    fillBoard(dancers, stars);
  }

  return success;
}

bool GameState::atFinalPositions(std::vector<Point> &finalPositions) {
  for (int i = 0; i < dancers.size(); i++) {
    if (dancers[i].position != finalPositions[i]) {
      return false;
    }
  }

  return true;
}

vector<Point> GameState::getViableNextPositions(Dancer &dancer) {
  vector<Point> viableNextPositions;
  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      if (dx != 0 && dy != 0) {
        continue;
      }
      Point nextPosition = dancer.position + Point(dx, dy);
      // next position CAN be occupied by another dancer
      if (withinBounds(nextPosition) && board[nextPosition.y][nextPosition.x] != -1) {
        viableNextPositions.push_back(nextPosition);
      }
    }
  }

  return viableNextPositions;
}

ChoreographerMove GameState::simulate(vector<DancerMove> &dancerSrcDest) {
  // back up dancers
  vector<Dancer> dancersBackup = cloneDancers();
  ChoreographerMove move;

  // map destinations to final positions
  vector<Point> finalPositions(dancers.size());
  for (auto &fromTo : dancerSrcDest) {
    for (int i = 0; i < dancers.size(); i++) {
      if (dancers[i].position == fromTo.from) {
        finalPositions[i] = fromTo.to;
        break;
      }
    }
  }
  #ifdef LOGGING
    for (auto &dancer : dancers) {
      cout << dancer.position.toString() << " ";
    }
    cout << endl;
    for (auto &fp : finalPositions) {
      cout << fp.toString() << " ";
    }
    cout << endl;
  #endif
  // simulate stuff
  while (!atFinalPositions(finalPositions)) {
    move.dancerMoves.push_back({});
    vector<Point> nextPositions;
    // get next position for every dancer
    for (int i = 0; i < dancers.size(); i++) {
      Dancer dancer = dancers[i];
      Point finalPos = finalPositions[i];
      vector<Point> viableNextPositions = getViableNextPositions(dancer);
      // sort viable positions based on manhattan distance (shortest first)
      for (int i = 1; i < viableNextPositions.size(); i++) {
        for (int j = i; j > 0; j--) {
          if (manDist(viableNextPositions[j], finalPos) < manDist(viableNextPositions[j - 1], finalPos)) {
            Point swap = viableNextPositions[j];
            viableNextPositions[j] = viableNextPositions[j - 1];
            viableNextPositions[j - 1] = swap;
          }
        }
      }
      // for each next position
      for (auto &candidate : viableNextPositions) {
        bool alreadyOccupied = false;
        for (auto &nextPos : nextPositions) {
          if (nextPos == candidate) {
            alreadyOccupied = true;
            break;
          }
        }
        if (!alreadyOccupied) {
          nextPositions.push_back(candidate);
          break;
        }
      }
    }
    for (int i = 0; i < dancers.size(); i++) {
      move.dancerMoves[move.dancerMoves.size() - 1].push_back({
        dancers[i].position, nextPositions[i]
      });
    }
    if (!simulateOneMove(nextPositions)) {
      cerr << "Simulation failed" << endl;
    }
    #ifdef VERBOSE
      display();
    #endif
  }

  // restore dancers and board
  dancers = dancersBackup;
  resetBoard();
  fillBoard(dancers, stars);

  // book keeping
  if (numSimulations == 0) {
    currentBestSequence = move;
  } else if (currentBestSequence.dancerMoves.size() > move.dancerMoves.size()) {
    currentBestSequence = move;
  }
  numSimulations++;

  return move;
}
