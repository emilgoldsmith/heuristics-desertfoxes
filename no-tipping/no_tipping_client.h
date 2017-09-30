#ifndef NO_TIPPING_CLIENT
#define NO_TIPPING_CLIENT

#include "../socket/socket_client.h"
#include "game_state.h"
#include <string>

using string = std::string;

class NoTippingClient {
  // network stuff
  const int DATA_SIZE = 1024;
  const int LENGTH = 63; // add/remove flag, 61 board positions, gameover flag
  string name = "desertfoxes";
  SocketClient *sc;

  // game stuff
  int first;
  int *currentStateArr = new int[LENGTH];
  int *splitStringBySpace(string s);
  void checkConsistency();
  void printCSA();

public:
  NoTippingClient(bool goesFirst, string socketAddress, int socketPort);
  void receiveMove();
  void makeMove(int weight, int position);
  GameState *state;
};

#endif
