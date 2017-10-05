#include "../socket/socket_client.h"
#include "../json/json.hpp"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <utility>

using namespace std;
using json = nlohmann::json;

class Client {
  SocketClient *socket;
  map<string, int> encoder;
  map<int, string> decoder;
  pair<string, string> targetNodes;

  public:
    Client(string ip, int port, bool isTraverser);
    vector<vector<int> > getGraph();
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

vector<vector<int> > Client::getGraph() {
  json graphInfo = socket->receiveJSON(2000*1000);
  targetNodes = make_pair(graphInfo["start_node"], graphInfo["end_node"]);
  json graph = graphInfo["graph"];
  string graph_string = graph.dump();
  int numNodes = 0;
  for (char c : graph_string) {
    if (c == '[') numNodes++;
  }
  vector<vector<int> > adjacencyLists(numNodes);
  bool inList = false;
  int currentEncodedNode = -1;
  bool inString = false;
  string nodeName = "";
  int curMaxIndex = 0;
  for (char c : graph_string) {
    if (c == '"') {
      if (inString) {
        // We are at the end of a nodeName
        if (!encoder.count(nodeName)) {
          // We haven't encoded this name yet
          encoder[nodeName] = curMaxIndex;
          decoder[curMaxIndex] = nodeName;
          curMaxIndex++;
        }
        if (!inList) {
          // This is a key
          currentEncodedNode = encoder[nodeName];
        } else {
          // This is an adjacent node
          adjacencyLists[currentEncodedNode].push_back(encoder[nodeName]);
        }
      } else {
        // We are at the start of a nodeName
        nodeName = "";
      }
      inString = !inString;
    } else if (c == '[' || c == ']') {
      inList = !inList;
    } else if (inString) {
      nodeName += c;
    }
  }
  return adjacencyLists;
}

int main() {
  Client traverser("127.0.0.1", 8080, true);
  Client adversary("127.0.0.1", 8080, false);
  vector<vector<int> > graph = traverser.getGraph();
}
