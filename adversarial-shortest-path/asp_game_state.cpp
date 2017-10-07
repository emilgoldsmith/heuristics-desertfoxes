#include "asp_game_state.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>
#include <limits>

using namespace std;

ASPGameState::ASPGameState(vector<vector<int>> *g) {
  graph = g;
  int size = g->size();

  costs = new double*[size];
  for (int i = 0; i < size; i++) {
    costs[i] = new double[size];
  }

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      costs[i][j] = INF;
    }
    for (int n : (*graph)[i]) {
      costs[i][n] = 1;
      costs[n][i] = 1;
    }
  }
}

ASPGameState::~ASPGameState() {
  for (int i = 0; i < graph->size(); i++) {
    delete [] costs[i];
  }
  delete [] costs;
}

pair<double, vector<int>> ASPGameState::getShortestPath(int source, int dest) {
  bool *visited = new bool[graph->size()]; // if node has been included in shortest path already
  double *distances = new double[graph->size()]; // distance of each node from source
  priority_queue<Node, vector<Node>, decltype(comp)> pq(comp); // min heap sorted by distance
  int *parentNodes = new int[graph->size()]; // store index of parent of each node in shortest path

  // initializations
  distances[source] = 0;
  pq.push({ source, 0 });
  for (int i = 0; i < graph->size(); i++) {
    visited[i] = false;
    parentNodes[i] = -1;
    if (i != source) {
      // distance initialized to INF besides source node
      pq.push({ source, INF }); 
      distances[i] = INF;
    }
  }

  // main dijstra routine
  while (!pq.empty()) {
    // pop the node currently closest to the destination
    Node currentNode = pq.top();
    int currentIndex = currentNode.index;
    pq.pop();

    // we have reached the destination
    if (currentIndex = dest) {
      break;
    }
    // revisiting the node due to insertion of duplicate
    if (visited[currentIndex]) {
      continue;
    }
    // destination not reachable
    if (distances[currentIndex] == INF) {
      cerr << "Error: destination not reachable" << endl;
      break;
    }

    // for each adjacent node
    for (int neighbor : (*graph)[currentIndex]) {
      double stepDist = costs[currentIndex][neighbor];
      // sanity check
      if (stepDist == INF) {
        cerr << "Error: edge has infinite cost" << endl;
      }
      // do something if new distance is less than recorded shortest distance
      double newDist = distances[currentIndex] + stepDist;
      if (distances[neighbor] > newDist) {
        pq.push({ neighbor, newDist });
        distances[neighbor] = newDist;
        parentNodes[neighbor] = currentIndex;
      }
    }
  }

  
  // construct shortest path, and compute path cost
  vector<int> path;
  double pathCost = 0;

  if (parentNodes[dest] == -1) {
    cerr << "Error: destination not reachable" << endl;
    return make_pair(-1, path);
  }
  // build path in reverse order
  path.push_back(dest);
  int parent = parentNodes[dest];
  while (parent != -1) {
    path.push_back(parent);
    parent = parentNodes[parent];
  }
  // reverse path to correct order
  reverse(path.begin(), path.end());
  // compute path cost
  for (int i = 0; i < path.size() - 1; i++) {
    pathCost += costs[path[i]][path[i+1]];
  }
  
  return make_pair(pathCost, path);
}

void ASPGameState::updateCost(int node1, int node2, double newCost) {
  if (node1 < 0 || node2 < 0 || node1 >= graph->size() || node2 >= graph->size()) {
    cerr << "Error: cost update failed, node does not exist" << endl;
    return;
  }
  costs[node1][node2] = newCost;
  costs[node2][node1] = newCost;
}
