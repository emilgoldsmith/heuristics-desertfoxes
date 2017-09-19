#ifndef NIM_CLIENT
#define NIM_CLIENT

#include "socket_client.h"

class NimClient {
  // network stuff
  const int DATA_SIZE = 1024;
  string name = "desertfoxes";
  SocketClient *sc;
  // game stuff
  int order; // 0 goes first, 1 goes second

public:
  int initNumStones;
  int initNumResets;
  
  NimClient(bool goesFirst, string socketAddress, int socketPort);
  json makeMove(int takeHowMany, bool useReset);
  json receiveMove();
};

#endif
