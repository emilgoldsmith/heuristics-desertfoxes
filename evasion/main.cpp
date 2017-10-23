#include "evasion_client.h"
#include "evasion_solve.h"

#include <string>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int main(int argc, char** argv) {
  if (argc != 3) {
    cerr << "usage: ./main.out IP PORT" << endl;
  }
  string ip = argv[1];
  int port = atoi(argv[2]);
  EvasionClient client(ip, port);

  HunterMove hm;
  Position pm;
  while (true) {
#ifdef DEBUG
    this_thread::sleep_for(chrono::milliseconds(4));
#endif
    if (client.isHunter) {
      hm = client.hunterMakeMove(solveHunterHeuristic);
      client.receiveUpdate();
      if (client.latestUpdate.gameNum == 1 && client.latestUpdate.tickNum == 0) {
        continue;
      }
      pm = client.parsePreyMove();
      cout << "Parsed prey move: " << pm.x << ", " << pm.y << endl;
      client.state->makeMove(hm, pm);
    } else {
      pm = client.preyMakeMove(solvePreyHeuristic);
      client.receiveUpdate();
      if (client.latestUpdate.gameNum == 1 && client.latestUpdate.tickNum == 0) {
        continue;
      }
      hm = client.parseHunterMove();
      cout << "Parsed hunter move: " << hm.wallType << " ";
      for (int i : hm.indicesToDelete) {
        cout << i << " ";
      }
      cout << endl;
      client.state->makeMove(hm, pm);
    }
#ifdef DEBUG
    if (!client.isConsistent()) {
      cerr << "NOPE" << endl;
    }
#endif
  }
}
