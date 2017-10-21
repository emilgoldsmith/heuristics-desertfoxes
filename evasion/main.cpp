#include "evasion_client.h"
#include "evasion_solve.h"

#include <string>

using namespace std;

int main() {
  string ip = "127.0.0.1";
  int port = 9001;
  EvasionClient client(ip, port);

  HunterMove hm;
  Position pm;
  while (true) {
    if (client.isHunter) {
      hm = client.hunterMakeMove();
      client.receiveUpdate();
      pm = client.parsePreyMove();
      cout << "Parsed prey move: " << pm.x << ", " << pm.y << endl;
      client.state->makeMove(hm, pm);
    } else {
      pm = client.preyMakeMove();
      client.receiveUpdate();
      hm = client.parseHunterMove();
      cout << "Parsed hunter move: " << hm.wallType << " ";
      for (int i : hm.indicesToDelete) {
        cout << i << " ";
      }
      cout << endl;
      client.state->makeMove(hm, pm);
    }
    if (!client.isConsistent()) {
      cerr << "NOPE" << endl;
    }
  }
}
