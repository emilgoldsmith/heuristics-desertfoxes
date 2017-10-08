#include "client.h"
#include "move.h"
#include "../socket/socket_client.h"
#include "../json/json.hpp"
#include "asp_game_state.h"

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
  if (role == 1) {
    // Receive the traversers first move
    receiveUpdate(false);
  }
}

void Client::receiveGraph() {
  json graphInfo = socket->receiveJSON(2000*1000);

  string startNodeString = graphInfo["start_node"];
  string endNodeString = graphInfo["end_node"];
  if (startNodeString[0] == '"' || endNodeString[0] == '"') {
    cout << "start and end node were correctly strings as expected" << endl;
  }

  json graph = graphInfo["graph"];
  string graph_string = graph.dump();
  int numNodes = 0;
  for (char c : graph_string) {
    if (c == '[') numNodes++;
  }
  adjacencyList = new vector<vector<int> >(numNodes);
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
          (*adjacencyList)[currentEncodedNode].push_back(encoder[nodeName]);
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

  int startNode = encoder[startNodeString];
  int endNode = encoder[endNodeString];

  state = new ASPGameState(adjacencyList, startNode, endNode);
}

Client::~Client() {
  delete state;
  delete adjacencyList;
}

void Client::sendTraversal(int start, int end) {
  json msg = {
    {"start_node", decoder[start]},
    {"end_node", decoder[end]}
  };
  socket->sendJSON(msg);
}

void Client::sendUpdate(int node1, int node2) {
  json msg = {
    {"node_1", decoder[node1]},
    {"node_2", decoder[node2]},
  };
  socket->sendJSON(msg);
}

void Client::makeMove(int node1_or_start, int node2_or_end) {
  if (role == 0) {
    sendTraversal(node1_or_start, node2_or_end);
    state->traverserMakeMove(node2_or_end);
  } else {
    sendUpdate(node1_or_start, node2_or_end);
    state->adversaryMakeMove(node1_or_start, node2_or_end);
  }
  receiveUpdate(true);
  receiveUpdate(false);
}

void Client::receiveUpdate(bool ourUpdate) {
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
  string newPosition = info["position"];
  if (node1[0] == '"' || node2[0] == '"' || newPosition[0] == '"') {
    cout << "receiveUpdate had strings unexpectedly" << endl;
  }
  long double returnedCost =
    ((role == 0 && ourUpdate) || (role == 1 && !ourUpdate))
      ? info["add_cost"]
      : info["new_cost"];
  cout << node1 << ' ' << node2 << ' ' << newPosition << ' ' << returnedCost << endl;

  Move move = {
    encoder[node1], // Remove quotes around number and encode
    encoder[node2], // Remove quotes around number
  };

  if (returnedCost == 0) {
    if (ourUpdate) {
      cout << "Server deemed our move invalid" << endl;
    } else {
      cout << "Server deemed their move invalid" << endl;
    }
  }
  if (!ourUpdate && returnedCost > 0) {
    if (role == 0) {
      state->adversaryMakeMove(move.node1, move.node2);
    } else {
      state->traverserMakeMove(move.node2);
    }
  }
  if (state->currentNode != encoder[newPosition]) {
    cerr << "currentNode incoherent" << endl;
  }
  if ((role == 1 && ourUpdate) || (role == 0 && !ourUpdate)) {
    long double ourCost = (state->costs)[move.node1][move.node2];
    long double epsilon = 1.0e-6;
    long double difference = ourCost - returnedCost;
    if (difference < 0) {
      difference *= -1;
    }
    if (difference > epsilon) {
      cout << "ERROR: Incoherent cost edge" << endl;
      cout << ourCost << ' ' << returnedCost << endl;
    }
  }
}
