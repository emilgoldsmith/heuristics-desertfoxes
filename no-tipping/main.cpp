#include "game_state.h"

#include <iostream>
#include <vector>

using namespace std;

int main() {
  GameState game(25);
  while (game.getWinner() == -1) {
    vector<Move> validMoves = game.getValidMoves();
    game.makeMove(validMoves[0]);
    // Just testing that it copies
    game.copy();
  }
  cout << "Winner is: " << game.getWinner() << endl;
}
