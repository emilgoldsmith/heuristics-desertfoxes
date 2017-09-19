#include <algorithm>
#include "nim_client.h"
#include "algorithm.h"

using namespace std;

void checkGameOver(json gameState) {
  cout << gameState << endl;
  if (gameState["finished"]) {
    cout << gameState["reason"] << endl;
    exit(0);
  }
}

int main(int argc, char const *argv[])
{
  // initialize dp array
  initialize();

  // client setup
  bool goesFirst = false;
  NimClient client(goesFirst, "127.0.0.1", 9000);

  // initialize game variable
  int numStonesLeft = client.initNumStones;
  int numMyResets = client.initNumResets;
  int numAdvResets = client.initNumResets;
  int currentMax = 0;
  int numStonesToTake;
  bool shouldIReset;

  if (goesFirst) {
    numStonesToTake = getMove(numStonesLeft, currentMax, numMyResets, numAdvResets, 0, shouldIReset);
    checkGameOver(client.makeMove(numStonesToTake, shouldIReset));

    // update game state after my move
    numStonesLeft -= numStonesToTake;
    currentMax = max(currentMax, numStonesToTake);
    if (shouldIReset) {
      numMyResets--;
    }
  }

  while (true) {
    // receive adversary's move
    json gameState = client.receiveMove();
    checkGameOver(gameState);

    // update game state after adversary moves
    numStonesLeft -= int(gameState["stones_removed"]);
    if (numStonesLeft != int(gameState["stones_left"])) {
      cout << "Client data / server data mismatch" << endl;
      cout << numStonesLeft << " " << int(gameState["stones_left"]) << endl;
      exit(0);
    }
    currentMax = max(currentMax, int(gameState["stones_removed"]));
    if (int(gameState["reset_used"])) {
      numAdvResets--;
    }
    
    // make my move
    numStonesToTake = getMove(numStonesLeft, currentMax, numMyResets, numAdvResets, int(gameState["reset_used"]), shouldIReset);
    checkGameOver(client.makeMove(numStonesToTake, shouldIReset));

    // update game state after my move
    numStonesLeft -= numStonesToTake;
    currentMax = max(currentMax, numStonesToTake);
    if (shouldIReset) {
      numMyResets--;
    }
  }
}
