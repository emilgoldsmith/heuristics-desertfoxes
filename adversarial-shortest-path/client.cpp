#include "../socket/socket_client.h"
#include "../json/json.hpp"

#include <iostream>
#include <string>

using namespace std;
using json = nlohmann::json;

class Client {
  SocketClient *socket;

  public:
    Client(string ip, int port, bool isTraverser);
    json getGraph();
};

Client::Client(string ip, int port, bool isTraverser) {
  socket = new SocketClient(ip, port);
  int playerType = isTraverser ? 0 : 1;
  json playerInfo = {
    {"name", "DesertFoxes"},
    {"type", playerType}
  };
  socket->sendJSON(playerInfo);
}

json Client::getGraph() {
  return socket->receiveJSON(2000*1000);
}

int main() {
  Client traverser("127.0.0.1", 8080, true);
  Client adversary("127.0.0.1", 8080, false);
  json graph = traverser.getGraph();
  cout << graph.dump(4) << endl;
}
