#include "parser.h"
#include "solve.h"
#include "../socket/socket_client.h"
#include "../timer/timer.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 4) {
    cout << "usage: ./main.out IP_ADDRESS PORT MAX_INPUT_SIZE" << endl;
  }
  string ipAddress = argv[1];
  int port = atoi(argv[2]);
  SocketClient socket(ipAddress, port);

  Timer timer;

  int maxInputSize = atoi(argv[3]);
  string input = socket.receive(maxInputSize);
  timer.start();
  int startIndex, endIndex;
  vector<vector<graph_edge> > graph = parse(input, startIndex, endIndex);
  string output = solve(graph, startIndex, endIndex);
  socket.sendString(output);
  timer.pause();
  cout << "Finished\nTime Elapsed: " << timer.getTime() << endl;
}
