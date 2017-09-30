#include "game_state.h"
#include "solve.h"

#include <string>
#include <iostream>
#include <vector>

using namespace std;

int runCompetition(int maxWeights, int someVariableP1, int otherVariableP1, int someVariableP2, int otherVariableP2) {
  GameState state(maxWeights);

  int currentPlayer = 0;
  while (state.getWinner() == -1) {
    Move move;
    if (currentPlayer == 0) {
      move = solveMinimax(&state, someVariableP1);
    } else {
      move = solveMinimax(&state, otherVariableP2);
    }
    state.makeMove(move);
    currentPlayer = (currentPlayer + 1) % 2;
  }

  return state.getWinner();
}

int main() {
  vector<int> someParam = {1, 2};
  vector<int> otherParam = {1, 2};
  vector<int> scores (someParam.size() * otherParam.size(), 0);
  for (int i = 0; i < someParam.size(); i++) {
    for (int j = 0; j < otherParam.size(); j++) {
      for (int k = 0; k < someParam.size(); k++) {
        for (int l = 0; l < otherParam.size(); l++) {
          if (i != k || j != l) {
            for (int maxWeight = 10; maxWeight <= 25; maxWeight++) {
              for (int episode = 0; episode < 25; episode++) {
                int winner = runCompetition(maxWeight, someParam[i], otherParam[j], someParam[k], otherParam[j]);
                if (winner == 0) {
                  scores[someParam.size()*i + j]++;
                } else {
                  scores[someParam.size()*k + l]++;
                }
              }
            }
          }
        }
      }
    }
  }
  int maxValue = -1;
  int bestSomeParam = -1;
  int bestOtherParam = -1;
  bool tie = false;
  for (int i = 0; i < someParam.size(); i++) {
    for (int j = 0; j < otherParam.size(); j++) {
      if (scores[i*someParam.size() + j] ==  maxValue) {
        tie = true;
      }

      if (scores[i*someParam.size() + j] > maxValue) {
        maxValue = scores[i*someParam.size() + j];
        bestSomeParam = someParam[i];
        bestOtherParam = otherParam[j];
        tie = false;
      }
    }
  }
  if (tie) {
    cout << "There was a tie but..." << endl;
  }
  cout << "The winner is: First param " << bestSomeParam << " and Second param " << bestOtherParam << endl;
}
