#include "game_state.h"
#include "solve.h"

#include <string>
#include <iostream>

using namespace std;

int runCompetition(int numWeights, NoTippingSolve player1, NoTippingSolve player2) {
  GameState state(numWeights);
  int currentPlayer = 0;
  while (state.getWinner() == -1) {
    Move move;
    if (currentPlayer == 0) {
      move = player1.getMove(state);
    } else {
      move = player2.getMove(state);
    }

    state.makeMove(move);
    currentPlayer = (currentPlayer + 1) % 2;
  }

  return state.getWinner();
}

int main() {

  int actualWins = 0;
  int secondWins = 0;
  for (int i = 0; i < 10; i++) {
    NoTippingSolve Guyu1(3, 2, true, 25, 35);
    NoTippingSolve Emil1(3, 2, true, 28, 20);
    int winner = runCompetition(25, Emil1, Guyu1);
    if (winner == 0) {
      actualWins++;
      cout << "Emil wins" << endl;
    } else {
      secondWins++;
      cout << "Guyu wins" << endl;
    }
    NoTippingSolve Guyu2(3, 2, true, 25, 35);
    NoTippingSolve Emil2(3, 2, true, 28, 20);
    // switch players
    winner = runCompetition(25, Emil2, Guyu2);
    if (winner == 0) {
      secondWins++;
      cout << "Guyu wins" << endl;
    } else {
      actualWins++;
      cout << "Emil wins" << endl;
    }
  }
  cout << "Emil " << " won " << actualWins << "/20 times" << endl;
}
