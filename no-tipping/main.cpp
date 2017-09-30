#include "game_state.h"
#include "python_communicator.h"

#include <iostream>
#include <vector>

using namespace std;

int main() {
  GameState game(25);
  init("python python_communicator.py");
  while (game.getWinner() == -1) {
    vector<Move> validMoves = game.getValidMoves();
    game.makeMove(validMoves[0]);
    // Just testing that it copies
    game.copy();
    string features = game.extractFeatures();
    writeMsg(features);
  }
  cout << "Winner is: " << game.getWinner() << endl;
  teardown();
}
