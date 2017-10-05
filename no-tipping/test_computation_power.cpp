#include "game_state.h"
#include "solve.h"
#include "../timer/timer.h"

#include <string>
#include <iostream>

using namespace std;

double testSpeed(int numWeights, NoTippingSolve player1, NoTippingSolve player2, int n) {
  GameState state(numWeights);
  int currentPlayer = 0;
  while (!(!state.isAddingPhase() && state.numWeightsLeft() <= n)) {
    Move move;
    if (currentPlayer == 0) {
      move = player1.getMove(state);
    } else {
      move = player2.getMove(state);
    }
    state.makeMove(move);
    currentPlayer = (currentPlayer + 1) % 2;
  }
  Timer t;
  t.start();
  player1.getMove(state);
  t.pause();
  return t.getTime();
}

int main() {
  double avg = 0;
  double low = 10000000;
  double high = 0;
  int timesEfficient = 0;
  for (int j = 0; j < 20; j++) {
    int i = 19;
    NoTippingSolve exhaustivePlayer(3, 2, true, i);
    NoTippingSolve maxMovesPlayer(3, 2, true, i);
    double timeTaken = testSpeed(25, exhaustivePlayer, maxMovesPlayer, i);
    avg += timeTaken;
    if (timeTaken > high) high = timeTaken;
    if (timeTaken < low) low = timeTaken;
    if (timeTaken < 5) timesEfficient++;
  }
  cout << "Efficient runs: " << timesEfficient << "/1000" << endl;
  avg /= 100;
  cout << "Average time: " << avg << " seconds" << endl;
  cout << "Slowest time was: " << high << " seconds" << endl;
  cout << "Fastest time was: " << low << " seconds" << endl;
}
