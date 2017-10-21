#include "evasion_client.h"
#include "evasion_solve.h"

#include <string>

using namespace std;

int main() {
  string ip = "127.0.0.1";
  int port = 9001;
  EvasionClient client(ip, port);

  while (true) {
    if (client.isHunter) {
      client.hunterMakeMove();
    } else {
      client.preyMakeMove();
    }
    client.receiveUpdate();
  }
}
