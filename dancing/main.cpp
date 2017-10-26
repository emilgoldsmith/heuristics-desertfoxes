#include "client.h"
#include "geometry.hpp"
#include "solve.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv) {
  if (argc != 4) {
    cout << "Usage: ./main.out IP HOST (0 == choreographer, 1 == spoiler)" << endl;
    exit(1);
  }
  string ip = argv[1];
  int port = atoi(argv[2]);
  int role = atoi(argv[3]);
  Client client(ip, port, role);
  if (role == 1) {
    client.makeSpoilerMove(dummyPlaceStars(&client));
  }
}
