#ifndef CLIENT_H
#define CLIENT_H

#include "../socket/socket_client.h"
#include "move.h"
#include "asp_game_state.h"

#include <string>
#include <map>
#include <vector>
#include <utility>

class Client {
  SocketClient *socket;
  std::map<std::string, int> encoder;
  std::map<int, std::string> decoder;
  int role; // 0 for traverser, 1 for adversary

  void receiveGraph();
  void sendTraversal(int start, int end);
  void sendUpdate(int node1, int node2);
  void receiveUpdate(bool ourUpdate);

  public:
    ASPGameState *state;

    Client(
      std::string ip,
      int port,
      int playerRole // 0 for traverser, 1 for adversary
    );

    ~Client();
    void makeMove(int node1_or_start, int node2_or_start);
};

#endif
