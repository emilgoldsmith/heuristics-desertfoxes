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
  int completeSearchLimit = 7;
  while (true) { // This loop will be terminated by exit(0) in the client class
    Move moveToMake;
    t.start();
    double timeMoveStarted = t.getTime();
    int stepsAway = client.state->intDistances[client.state->currentNode];

    if (type == 0 || type == 1) {
      moveToMake = getMove(client.state, role, type, &t, 60 + 57);
    } else {
      if (t.timeLeft() < 0) {
        moveToMake = getMove(client.state, role, 0, &t, -1);
      } else if (stepsAway <= completeSearchLimit) {
        double deadline = t.getTime() + min(10.0, t.timeLeft() * 0.8);
        moveToMake = getMove(client.state, role, 1, &t, deadline);
        if (t.getTime() > deadline) {
          completeSearchLimit--;
          int instantMoveType = role == 0 ? 0 : 3;
          moveToMake = getMove(client.state, role, instantMoveType, &t, -1);
        }
      } else {
        double luckyAttemptIncrement;
        double heuristicIncrement;
        if (t.timeLeft() > 60) {
          luckyAttemptIncrement = 0.25;
          heuristicIncrement = 5;
        } else if (t.timeLeft() > 30) {
          luckyAttemptIncrement = 0.25;
          heuristicIncrement = 3;
        } else if (t.timeLeft() > 10) {
          luckyAttemptIncrement = 0;
          heuristicIncrement = 1.5;
        } else {
          // In this case we might want to switch strategy completely and not follow this structure
          // Hopefully we never get here though
          // Though if it's a 1000 move game or something crazy like that it could easily happen
          luckyAttemptIncrement = 0;
          heuristicIncrement = 0.5;
        }

        double deadline = t.getTime() + luckyAttemptIncrement;
        moveToMake = getMove(client.state, role, 1, &t, deadline);
        if (t.getTime() > deadline) {
          // Our hopeful attempt at perfect searching didn't work
          deadline = t.getTime() + min(heuristicIncrement, t.timeLeft() / stepsAway);
          // Here it actually matters what type you chose, you choose the heuristic here
          moveToMake = getMove(client.state, role, type, &t, deadline);
        }
      }
    }

    t.pause();
    double timeTakenForMove = t.getTime() - timeMoveStarted;
    if (timeTakenForMove > 0.5) {
      cout << "Time taken with distance " << stepsAway << " was: " << timeTakenForMove << endl;
      cout << "Time Left: " << (t.timeLeft() + 3) << endl;
    }
    if (moveToMake.node1 == -1 || moveToMake.node2 == -1) {
      client.makeMove(0, 1);
    } else {
      client.makeMove(moveToMake.node1, moveToMake.node2);
    }
  }
}
