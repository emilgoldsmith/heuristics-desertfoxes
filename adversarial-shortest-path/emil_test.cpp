#include "client.h"
#include "solve.h"
#include "move.h"
#include "../timer/timer.h"

#include <utility>
#include <iostream>
#include <string>
#include <bitset>
#include <limits>
#include <set>
#include <algorithm>

using namespace std;

int main(int argc, char const *argv[]) {
  cout << "Starting" << endl;
  if (argc != 4) {
    cout << "Usage: ./main.out IP_ADDRESS PORT IS_ADVERSARY(0/1)" << endl;
  }
  string ip = argv[1];
  int port = atoi(argv[2]);
  int role = atoi(argv[3]);
  int type = atoi(argv[4]);
  Client client(ip, port, role);
  cout << "Connected" << endl;

  while (true) { // This loop will be terminated by exit(0) in the client class
    Move moveToMake;
    Timer t;
    t.start();
    if (role == 0) {
      moveToMake = getTraverseMove(client.state, type);
    } else {
      moveToMake = getAdversaryMove(client.state, type);
    }
    int bfsDist = client.state->intDistances[client.state->currentNode];
    t.pause();
    cout << "Time taken with distance " << client.state->intDistances[client.state->currentNode] << " was: " << t.getTime() << endl;
    client.makeMove(moveToMake.node1, moveToMake.node2);
  }
}
