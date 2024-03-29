#include "client.h"
#include "solve.h"
#include "solve-guyu.h"
#include "move.h"

#include <utility>
#include <iostream>
#include <string>
#include <bitset>
#include <limits>

using namespace std;

int main(int argc, char const *argv[]) {
  cout << "Starting" << endl;
  if (argc != 4) {
    cout << "Usage: ./main.out IP_ADDRESS PORT IS_ADVERSARY(0/1)" << endl;
  }
  string ip = argv[1];
  int port = atoi(argv[2]);
  int role = atoi(argv[3]);
  Client client(ip, port, role);
  cout << "Connected" << endl;

  bitset<1010> emptyVisited;

  while (true) { // This loop will be terminated by exit(0) in the client class
    Move moveToMake;
    if (role == 0) {
      // moveToMake = getTraverseMove(client.state);
      moveToMake = guyuTraverser(client.state);
      cout << "Traverser: " << moveToMake.node1 << " " << moveToMake.node2 << endl;
    } else {
      bool guyu = true; // change this to toggle between our strats
     
      if (!guyu) { // emil
        // moveToMake = getAdversaryMove(client.state);
        moveToMake = miniMaxAdversary(client.state, emptyVisited, numeric_limits<long double>::min(), numeric_limits<long double>::max());
        cout << moveToMake.node1 << ' ' << moveToMake.node2 << ' ' << moveToMake.costRelatedInfo << endl;
      } else { // guyu
        // moveToMake = guyuAdversary(client.state);
        moveToMake = smartGuyuAdversary(client.state);
        cout << "Adversary: " << moveToMake.node1 << " " << moveToMake.node2 << endl;
      }
    }
    client.makeMove(moveToMake.node1, moveToMake.node2);
  }
}
