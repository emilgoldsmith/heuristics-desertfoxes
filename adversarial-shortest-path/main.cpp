#include "client.h"
#include "solve.h"
#include "move.h"

#include <utility>
#include <iostream>
#include <string>

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

  while (true) { // This loop will be terminated by exit(0) in the client class
    Move moveToMake;
    if (role == 0) {
      moveToMake = getTraverseMove(client.state);
    } else {
      moveToMake = getAdversaryMove(client.state);
    }
    client.makeMove(moveToMake.node1, moveToMake.node2);
  }
}
