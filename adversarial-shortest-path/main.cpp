#include "client.h"
#include "solve.h"
#include "solve-guyu.h"
#include "move.h"

#include <utility>
#include <iostream>
#include <string>
#include <bitset>

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
    } else {
      // moveToMake = getAdversaryMove(client.state);
      moveToMake = miniMaxAdversary(client.state, emptyVisited);
      cout << moveToMake.node1 << ' ' << moveToMake.node2 << ' ' << moveToMake.costRelatedInfo << endl;
    }
    client.makeMove(moveToMake.node1, moveToMake.node2);
  }
}
