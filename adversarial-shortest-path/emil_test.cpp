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

  Timer t(60 + 57); // give us a 3 second buffer
  while (true) { // This loop will be terminated by exit(0) in the client class
    Move moveToMake;
    t.start();
    double timeMoveStarted = t.getTime();
    double deadline = t.getTime() + 3;
    if (role == 0) {
      moveToMake = getMove(client.state, role, type, &t, deadline);
    } else {
      moveToMake = getMove(client.state, role, type, &t, deadline);
    }
    int bfsDist = client.state->intDistances[client.state->currentNode];
    t.pause();
    double timeTakenForMove = t.getTime() - timeMoveStarted;
    if (timeTakenForMove > 0.5) {
      cout << "Time taken with distance " << client.state->intDistances[client.state->currentNode] << " was: " << timeTakenForMove << endl;
      cout << "Time Left: " << (t.timeLeft() + 3) << endl;
    }
    if (moveToMake.node1 == -1 || moveToMake.node2 == -1) {
      client.makeMove(0, 1);
    } else {
      client.makeMove(moveToMake.node1, moveToMake.node2);
    }
  }
}
