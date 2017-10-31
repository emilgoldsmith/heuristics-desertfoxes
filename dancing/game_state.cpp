#include "game_state.h"
#include "../random/random.h"

#include <algorithm>
#include <queue>

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

  sortedDancerIndices.reserve(dancers.size());
  for (int i = 0; i < dancers.size(); i++) sortedDancerIndices.push_back(i);
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

bool GameState::isConsistent() {
  bool isConsistent = true;
  // check dancers
  for (auto &dancer : dancers) {
    if (board[dancer.position.y][dancer.position.x] != dancer.color + 1) {
      isConsistent = false;
      #ifdef DEBUG
        cerr << "Dancer list inconsistent with board" << endl;
        cerr << board[dancer.position.y][dancer.position.x] << ' ' << dancer.color + 1 << endl;
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
      if (board[i][j] == 0) {
        cout << "." << " ";
      } else if (board[i][j] == -1) {
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
#ifdef DEBUG
  vector<Dancer> dancersBackup(dancers);
#endif

  for (int i = 0; i < dancers.size(); i++) {
    Dancer dancer = dancers[i];
    Point nextPosition = nextPositions[i];
    // check for errors
#ifdef DEBUG
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
#endif
    // clear dancer's original position and set dancer new position
    board[dancer.position.y][dancer.position.x] = 0;
    dancers[i].position = nextPosition;
  }

  // update board with new dancer positions
  for (int i = 0; i < dancers.size(); i++) {
    board[dancers[i].position.y][dancers[i].position.x] = dancers[i].color + 1;
  }

#ifdef DEBUG
  // final consistency check (e.g. if multiple dancers moved to same position)
  if (!isConsistent()) {
    success = false;
  }
#endif

  // restore game state if simulation fails
#ifdef DEBUG
  if (!success) {
    dancers = dancersBackup;
    resetBoard();
    fillBoard(dancers, stars);
  }
#endif

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

vector<Point> GameState::getViableNextPositions(Point dancerPosition) {
  vector<Point> viableNextPositions;
  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      if (dx != 0 && dy != 0) {
        continue;
      }
      Point nextPosition = dancerPosition + Point(dx, dy);
      // next position CAN be occupied by another dancer
      if (withinBounds(nextPosition) && board[nextPosition.y][nextPosition.x] != -1) {
        viableNextPositions.push_back(nextPosition);
      }
    }
  }

  return viableNextPositions;
}

vector<Point> GameState::getViableNextPositions(Dancer &dancer) {
  return getViableNextPositions(dancer.position);
}

Point GameState::searchBestNext(Dancer &dancer, Point &finalPosition, vector<Point> &initViableNexts) {
  const int bfsLimit = 5;
  queue<PointParent> q;

  // initialize visited flags
  vector<vector<bool>> visited(bfsLimit * 2 + 1, vector<bool>(bfsLimit * 2 + 1, false));

  for (Point &next : initViableNexts) {
    Point dir = next - dancer.position;
    q.push({ dir, dir, 1 });
    visited[dir.y + bfsLimit][dir.x + bfsLimit] = true;
  }

  Point bestMove = initViableNexts[0] - dancer.position;
  int bestMoveDistance = manDist(bestMove + dancer.position, finalPosition);
  while (!q.empty()) {
    PointParent pp = q.front();
    q.pop();
    // found a viable play, but don't stop
    if (pp.depth == bfsLimit) {
      int dist = manDist(pp.point + dancer.position, finalPosition);
      if (dist < bestMoveDistance) {
        bestMove = pp.source;
        bestMoveDistance = dist;
      }
      // We continue as we don't want to add any new items to the queue with depth higher than this
      continue;
    }
    // push unvisited neighbors into the queue
    vector<Point> ppNext = getViableNextPositions(pp.point + dancer.position);
    for (Point &next : ppNext) {
      Point dir = next - dancer.position;
      if (!visited[dir.y + bfsLimit][dir.x + bfsLimit]) {
        q.push({ dir, pp.source, pp.depth + 1 });
        visited[dir.y + bfsLimit][dir.x + bfsLimit] = true;
      }
    }
  }

  return bestMove + dancer.position;
}

void GameState::simulate(SolutionSpec &input, string strategy) {
  // back up dancers
  vector<Dancer> dancersBackup(dancers);
  ChoreographerMove move = {{}, input.finalConfiguration};

  // map destinations to final positions
  vector<Point> finalPositions(dancers.size());
  for (auto &fromTo : input.dancerMapping) {
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
    vector<Point> nextPositions(dancers.size(), {-1, -1});
    int numOutOfPlaceDancers = 0;
    bool stuck = false;

    for (int i = 0; i < dancers.size(); i++) {
      if (dancers[i].position != finalPositions[i]) {
        numOutOfPlaceDancers++;
      }
    }

    // sorted dancer index with priority given to dancer with the highest manhattan distance to final position
    if (!randomize) {
      sort(sortedDancerIndices.begin(), sortedDancerIndices.end(),
          [&](const int &a, const int &b) -> bool {
            return manDist(dancers[a].position, finalPositions[a]) > manDist(dancers[b].position, finalPositions[b]);
          }
      );
    } else {
      Random r;
      shuffle(sortedDancerIndices.begin(), sortedDancerIndices.end(), r.generator);
    }

    // get next position for every dancer
    for (int i : sortedDancerIndices) {
      Dancer dancer = dancers[i];
      Point finalPos = finalPositions[i];
      vector<Point> viableNextPositions = getViableNextPositions(dancer);
      vector<Point> filteredViableNexts;
      // sort viable positions based on manhattan distance (shortest first)
      for (int i = 1; i < viableNextPositions.size(); i++) {
        for (int j = i; j - 1 >= 0 && manDist(viableNextPositions[j], finalPos) < manDist(viableNextPositions[j - 1], finalPos); j--) {
          swap(viableNextPositions[j - 1], viableNextPositions[j]);
        }
      }
      // filter viableNextPositions by checking if they are already occupied
      for (auto &candidate : viableNextPositions) {
        bool alreadyOccupied = false;
        for (int j : sortedDancerIndices) {
          if (j == i) break; // All the rest of the dancers haven't been moved yet
          if (nextPositions[j] == candidate) {
            alreadyOccupied = true;
            break;
          }
        }
        if (!alreadyOccupied) {
          filteredViableNexts.push_back(candidate);
        }
      }

      if (filteredViableNexts.empty()) {
        stuck = true;
        break;
      }

      // get and set best next position
      nextPositions[i] = searchBestNext(dancer, finalPos, filteredViableNexts);
    }

    if (stuck) {
      randomize = true;
      move.dancerMoves.pop_back();
      continue;
    }
    randomize = false;

    for (int i = 0; i < dancers.size(); i++) {
      move.dancerMoves[move.dancerMoves.size() - 1].push_back({
        dancers[i].position, nextPositions[i]
      });
    }
    #ifdef DEBUG // useful for checking when simulation gets stuck
      vector<int> movedDancers;
      for (int i = 0; i < dancers.size(); i++) {
        auto dm = move.dancerMoves[move.dancerMoves.size() - 1][i];
        if (dm.from != dm.to) {
          movedDancers.push_back(i);
        }
      }
      vector<int> dancersShouldMove;
      for (int i = 0; i < dancers.size(); i++) {
        if (dancers[i].position != finalPositions[i]) {
          dancersShouldMove.push_back(i);
        }
      }
    #endif
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
#ifdef DEBUG
  if (bestMovePerStrategy.count(strategy) == 0) {
    bestMovePerStrategy[strategy] = move.dancerMoves.size();
  } else if (bestMovePerStrategy[strategy] > move.dancerMoves.size()) {
    bestMovePerStrategy[strategy] = move.dancerMoves.size();
  }
#endif
}

#ifdef DEBUG
void GameState::printStrategyStats() {
  cout << "Best scores achieved from each strategy" << endl;
  for (auto mapPair : bestMovePerStrategy) {
    cout << "Strategy " << mapPair.first << " achieved best score of: " << mapPair.second << endl;
  }
}
#endif
