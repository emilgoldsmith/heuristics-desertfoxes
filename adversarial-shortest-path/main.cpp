#include "client.h"
#include "solve.h"
#include "move.h"

#include <utility>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char const *argv[]) {
  cout << "Starting" << endl;
  int role = atoi(argv[1]);
  Client client("127.0.0.1", 8080, role);
  cout << "Connected" << endl;

  pair<int, int> targetNodes = client.getTargetNodes();

  if (role == 1) { // We are the adversary so first get to know the new position
    Move receivedMove = client.receiveUpdate(false);
    // We should probably do some game state logic here to let it know stuff
  }
  while (true) { // This loop will be terminated by exit(0) in the client class
    Move moveToMake;
    if (role == 0) {
      moveToMake = getTraverseMove(&client.adjacencyList);
    } else {
      moveToMake = getAdversaryMove(&client.adjacencyList);
    }
    client.makeMove(moveToMake.node1, moveToMake.node2, moveToMake.costRelatedInfo);
    client.receiveUpdate(true);
    client.receiveUpdate(false);
  }
}
