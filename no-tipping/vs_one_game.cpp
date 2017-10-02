#include "game_state.h"
#include "solve.h"

#include <string>
#include <iostream>

using namespace std;

int runCompetition(int maxWeights, int someVariableP1, int otherVariableP1, int someVariableP2, int otherVariableP2) {
  GameState state(maxWeights);

  int currentPlayer = 0;
  while (state.getWinner() == -1) {
    Move move;
    if (currentPlayer == 0) {
      cout << "first player ";
      move = solveMinimaxRemoveFocus(&state, someVariableP1);
    } else {
      cout << "second player ";
      move = solveMinimax(&state, someVariableP2);
    }
    cout << "Adding phase: " << state.isAddingPhase() << endl;
    cout << "Player: " << currentPlayer << "; Move: " << "(" << move.weight << ", " << move.position << ")" << endl;
    state.makeMove(move);
    currentPlayer = (currentPlayer + 1) % 2;
  }

  return state.getWinner();
}

int main() {
  int p1MaxDepth = 5;
  int p2MaxDepth = 1;
  int winner = runCompetition(25, p1MaxDepth, -1, p2MaxDepth, -1);
  cout << winner << endl;
}
