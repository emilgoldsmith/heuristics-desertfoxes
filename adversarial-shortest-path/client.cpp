#include "client.h"
#include "move.h"
#include "../socket/socket_client.h"
#include "../json/json.hpp"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <utility>

using namespace std;
using json = nlohmann::json;

Client::Client(string ip, int port, int playerRole) {
  role = playerRole;
  socket = new SocketClient(ip, port);
  json playerInfo = {
    {"name", "DesertFoxes"},
    {"type", playerRole}
  };
  socket->sendJSON(playerInfo);
  receiveGraph();
}

void Client::receiveGraph() {
  json graphInfo = socket->receiveJSON(2000*1000);
  targetNodes = make_pair(graphInfo["start_node"], graphInfo["end_node"]);
  json graph = graphInfo["graph"];
  string graph_string = graph.dump();
  int numNodes = 0;
  for (char c : graph_string) {
    if (c == '[') numNodes++;
  }
  adjacencyList.resize(numNodes);
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
          adjacencyList[currentEncodedNode].push_back(encoder[nodeName]);
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
}

void Client::sendTraversal(int start, int end) {
  json msg = {
    {"start_node", decoder[start]},
    {"end_node", decoder[end]}
  };
  socket->sendJSON(msg);
}

void Client::sendUpdate(int node1, int node2, double factor) {
  json msg = {
    {"node_1", decoder[node1]},
    {"node_2", decoder[node2]},
    {"cost_factor", factor}
  };
  socket->sendJSON(msg);
}

void Client::makeMove(int node1_or_start, int node2_or_start, double factor) {
  if (role == 0) {
    sendTraversal(node1_or_start, node2_or_start);
  } else if (factor == -1) {
    cerr << "DIDN'T SUPPLY FACTOR WITH ADVERSARY MOVE" << endl;
  } else {
    sendUpdate(node1_or_start, node2_or_start, factor);
  }
}

Move Client::receiveUpdate(bool ourUpdate) {
  json info = socket->receiveJSON(500);
  if (info["done"]) {
    cout << "Game is over" << endl;
    exit(0);
  }
  if (info["error"]) {
    string player = ourUpdate ? "we" : "they";
    cout << player << " ran out of time" << endl;
    exit(0);
  }
  string node1 = info["edge"][0];
  string node2 = info["edge"][1];
  Move move = {
    encoder[node1.substr(1, node1.size() - 1)], // Remove quotes around number and encode
    encoder[node2.substr(1, node2.size() - 1)], // Remove quotes around number
    ((role == 0 && ourUpdate) || (role == 1 && !ourUpdate)) ? info["add_cost"] : info["new_cost"]
  };
  return move;
}

pair<int, int> Client::getTargetNodes() {
  return make_pair(encoder[targetNodes.first], encoder[targetNodes.second]);
}
