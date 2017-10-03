#include "game_state.h"
#include "solve.h"

#include <string>
#include <iostream>

using namespace std;

int runCompetition(int numWeights, NoTippingSolve player1, NoTippingSolve player2) {

  GameState state(numWeights);
  int currentPlayer = 0;
  while (state.getWinner() == -1) {
    if (state.isAddingPhase()) {
      cout << "[Add] ";
    } else {
      cout << "[Remove] ";
    }

    Move move;
    if (currentPlayer == 0) {
      move = player1.getMove(state);
      cout << "First player plays: ";
    } else {
      move = player2.getMove(state);
      cout << "Second player plays: ";
    }
    cout << "{weight: " << move.weight << ", position: " << move.position << "}" << endl;

    state.makeMove(move);
    currentPlayer = (currentPlayer + 1) % 2;
  }

  return state.getWinner();
}

int main() {
  NoTippingSolve naivePlayer(1, 2, true, 0);
  NoTippingSolve exhaustivePlayer(2, 1, true, 22);
  NoTippingSolve greedyPlayer(2, 1, false, -1);

  int winner;
  int firstWins = 0;
  int secondWins = 0;
  for (int i = 0; i < 1; i++) {
    winner = runCompetition(15, greedyPlayer, exhaustivePlayer);
    if (winner == 0) {
      firstWins++;
    } else {
      secondWins++;
    }
    // switch players
    winner = runCompetition(15, exhaustivePlayer, greedyPlayer);
    if (winner == 0) {
      secondWins++;
    } else {
      firstWins++;
    }
  }
  cout << "First player score: " << firstWins << ", second player score: " << secondWins << endl;
}
