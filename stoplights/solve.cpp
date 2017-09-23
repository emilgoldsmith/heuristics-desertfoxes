#include "parser.h"

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <limits>

using namespace std;

struct Node {
  long long index; // index of node
  long long dist; // distance of node from source
  long long arrivedAt; // clock time of arrival (-1 for nodes not yet reached)
  Node(long long i, long long d, long long t) {
    index = i;
    dist = d;
    arrivedAt = t;
  }
};

// compare function used by priority queue
auto comp = [] (Node &a, Node &b) -> bool { return a.dist > b.dist; };

// get the stopLight distance (accounting for waiting time)
long long getStoplightDist(long long clockTime, graph_edge edge) {
  if (edge.redtime == 0) return edge.traversetime;
  if (edge.greentime < edge.traversetime) return -1;
  long long cycle = ((long long) edge.greentime) + edge.redtime;
  long long stoplightStatus = edge.greentime - (clockTime % cycle);
  // edge is currently red or not green enough to be traversed
  if (stoplightStatus < edge.traversetime) {
    return edge.traversetime + (cycle - (clockTime % cycle));
  }
  // edge has enough green time left
  return edge.traversetime;
}

string solve(vector<vector<graph_edge> > graph, long long startIndex, long long endIndex) {
  const long long INF = numeric_limits<long long>::max(); // set INF to largest long long
  bool *visited = new bool[graph.size()]; // if node has been included in shortest path already
  long long *distances = new long long[graph.size()]; // distance of each node to source
  priority_queue<Node, vector<Node>, decltype(comp)> pq(comp); // min heap sorted by distance
  long long *parentNodes = new long long[graph.size()]; // store index of parent of each node in the shortest path

  // initializations
  distances[startIndex] = 0;
  pq.push(Node(startIndex, 0, 0));
  for (long long i = 0; i < graph.size(); i++) {
    visited[i] = false;
    parentNodes[i] = -1;
    if (i != startIndex) {
      pq.push(Node(i, INF, -1));
      distances[i] = INF;
    }
  }

  vector<graph_edge> edges;
  long long clockTime; // the time when the current node is reached
  long long stoplightDist; // effective distance after accounting for waiting time
  long long newDistance; // the possibly shorter distance for adjacenet nodes

  while (!pq.empty()) {
    Node currentNode = pq.top(); // node currently closest to sources
    pq.pop();
    if (currentNode.index == endIndex) { // we have reached the destination
      break;
    }
    if (visited[currentNode.index]) { // revisiting due to insertion of duplicate
      continue;
    }
    if (distances[currentNode.index] == INF) { // destination not reachable
      cout << "Error: destination not reachable" << endl;
      break;
    }

    clockTime = currentNode.arrivedAt;
    visited[currentNode.index] = true;
    edges = graph[currentNode.index];

    // for each adjacent node
    for (auto &neighbor : edges) {
      // get the stopLight distance
      stoplightDist = getStoplightDist(clockTime, neighbor);
      // do something only when edge is crossable
      if (stoplightDist != -1) {
        // we do not update the existing node in the priority queue
        // instead, we insert a new copy of that node with a shorter distance
        newDistance = distances[currentNode.index] + stoplightDist;
        if (distances[neighbor.target] > newDistance) {
          pq.push(Node(neighbor.target, newDistance, clockTime + stoplightDist));
          distances[neighbor.target] = newDistance; // update distance
          parentNodes[neighbor.target] = currentNode.index; // udpate parent node
        }
      }
    }
  }

  // construct path string from parentNodes
  string pathString = "";
  if (parentNodes[endIndex] == -1) {
    pathString = "Error: destination not reachable";
  } else {
    vector<long long> pathIndices; // path in reverse order
    pathIndices.push_back(endIndex);
    long long parent = parentNodes[endIndex];
    while (parent != -1) {
      pathIndices.push_back(parent);
      parent = parentNodes[parent];
    }

    long long t = 0;
    long long startNode, endNode, startTime, endTime;
    graph_edge edge;
    for (long long i = pathIndices.size() - 1; i > 0; i--) {
      startNode = pathIndices[i];
      endNode = pathIndices[i - 1];
      // get the edge between startNode and endNode
      for (auto &e : graph[startNode]) {
        if (e.target == endNode) {
          edge = e;
          break;
        }
      }
      // compute endTime
      endTime = t + getStoplightDist(t, edge);
      // compute startTime
      startTime = endTime - edge.traversetime;
      // update clock
      t = endTime;
      // append to string
      pathString += "n" + to_string(startNode) + " ";
      pathString += "n" + to_string(endNode) + " ";
      pathString += to_string(startTime) + " ";
      pathString += to_string(endTime) + "\n";
    }
  }

  // clean up
  delete [] visited;
  delete [] distances;
  delete [] parentNodes;

  return pathString;
}
