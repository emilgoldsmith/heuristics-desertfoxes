#include <iostream>
#include <cstdio>
#include <algorithm>
#include "algorithm.h"

using namespace std;

int main() {
  initialize();
  cout << "Ready, please input initial number of stones" << endl;
  int stonesLeft, curMax, nextResets, prevResets, currentlyReset;
  curMax = 0; nextResets = 4; prevResets = 4; currentlyReset = 0;
  cin >> stonesLeft;
  cout << "Game is about to start with " << stonesLeft << " initial stones\n" << endl;
  bool playerId = false;
  while (stonesLeft > 0) {

    bool shouldIReset;
    int stonesToTake = getMove(stonesLeft, curMax, nextResets, prevResets, currentlyReset, shouldIReset);
    if (!playerId) {
      if (stonesToTake) {
        cout << "Player " << playerId << " Used " << stonesToTake << " stones and reset flag for next turn is: " << shouldIReset << endl;
      } else {
        stonesToTake = 1;
        cout << "Player " << playerId << " believes he is lost and is therefore only using " << stonesToTake << " stone and reset flag for next turn is: " << shouldIReset << endl;
      }
    } else {
      cin >> stonesToTake >> shouldIReset;
    }
    stonesLeft -= stonesToTake;
    curMax = max(curMax, stonesToTake);
    nextResets -= shouldIReset;
    swap(nextResets, prevResets);
    currentlyReset = shouldIReset;
    printf("\nCurrent status is:\nStones left: %d\nCurrent max: %d\nResets for player %d: %d\nResets for player %d: %d\nIs reset activated for next turn: %d\n\n\n", stonesLeft, curMax, playerId, prevResets, !playerId, nextResets, currentlyReset);
    playerId = !playerId;
  }
  cout << "Game is over" << endl;
}
