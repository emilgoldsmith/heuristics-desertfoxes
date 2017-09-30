#include "no_tipping_client.h"
#include "solve.h"

using namespace std;

int main(int argc, char const *argv[])
{
  string address;
  int port;
  bool goesFirst = false;

  if (argc < 3) {
    cout << "Usage: " << argv[1] << " <server_address> <port> (-f)" << endl;
  } else {
    address = string(argv[1]);
    port = stoi(argv[2]);
    if (argc == 4) {
      goesFirst = true;
    }
  }

  NoTippingClient ntp(goesFirst, address, port);

  while (true) {
    ntp.receiveMove();
    Move m = solve(ntp.state);
    ntp.makeMove(m.weight, m.position);
  }

  return 0;
}
