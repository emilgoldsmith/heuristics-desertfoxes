#ifndef CLIENT_H
#define CLIENT_H

#include "../socket/socket_client.h"
#include "move.h"

#include <string>
#include <map>
#include <vector>
#include <utility>

class Client {
  SocketClient *socket;
  std::map<std::string, int> encoder;
  std::map<int, std::string> decoder;
  std::pair<std::string, std::string> targetNodes;
  int role; // 0 for traverser, 1 for adversary

  void receiveGraph();
  void sendTraversal(int start, int end);
  void sendUpdate(int node1, int node2, double factor);

  public:
    std::vector<std::vector<int> > adjacencyList;

    Client(
      std::string ip,
      int port,
      int playerRole // 0 for traverser, 1 for adversary
    );
    std::pair<int, int> getTargetNodes();
    void makeMove(int node1_or_start, int node2_or_start, double factor = -1);
    Move receiveUpdate(bool ourUpdate);
};

#endif
