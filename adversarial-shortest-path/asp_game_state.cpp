#include "asp_game_state.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>
#include <limits>
#include <cmath>

using namespace std;

ASPGameState::ASPGameState(vector<vector<int>> *g, int source, int dest) {
  graph = g;
  currentNode = source;
  destNode = dest;

  int size = g->size();
  costs = new double*[size];
  parentNodes = new int[size];
  distances = new double[size];
  intDistances = new int[size];
  for (int i = 0; i < size; i++) {
    costs[i] = new double[size];
  }
  
  // initialize costs
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      costs[i][j] = INF;
    }
    for (int n : (*graph)[i]) {
      costs[i][n] = 1;
      costs[n][i] = 1;
    }
  }
  
  // populate intDistances first
  computeDijkstra(destNode, true);
  // then populate distances and parentNodes using edges with costs
  computeDijkstra(destNode, false);
}

ASPGameState::ASPGameState(ASPGameState &gs) {
  graph = gs.graph;
  currentNode = gs.currentNode;
  destNode = gs.destNode;

  // deep copy of everything
  int size = graph->size();
  costs = new double*[size];
  parentNodes = new int[size];
  distances = new double[size];
  intDistances = new int[size];

  for (int i = 0; i < size; i++) {
    costs[i] = new double[size];
    parentNodes[i] = gs.parentNodes[i];
    distances[i] = gs.distances[i];
    intDistances[i] = gs.intDistances[i];
    for (int j = 0; j < size; j++) {
      costs[i][j] = gs.costs[i][j];
    }
  }
}

ASPGameState::~ASPGameState() {
  for (int i = 0; i < graph->size(); i++) {
    delete [] costs[i];
  }
  delete [] costs;
  delete [] parentNodes;
  delete [] distances;
}

void ASPGameState::computeDijkstra(int source, bool bfs) {
  // if node has been included in shortest path already
  bool *visited = new bool[graph->size()];
  // min heap sorted by distance
  priority_queue<Node, vector<Node>, decltype(comp)> pq(comp);
  
  // initializations
  distances[source] = 0;
  intDistances[source] = 0;
  pq.push({ source, 0, 0});
  for (int i = 0; i < graph->size(); i++) {
    visited[i] = false;
    parentNodes[i] = -1;
    if (i != source) {
      // distance initialized to INF besides source node
      pq.push({ source, INF, INT_INF });
      distances[i] = INF;
      intDistances[i] = INT_INF;
    }
  }

  // main dijkstra routine
  while (!pq.empty()) {
    // pop the node currently closest to the destination
    Node currentNode = pq.top();
    int currentIndex = currentNode.index;
    pq.pop();

    // revisiting the node due to insertion of duplicate
    if (visited[currentIndex]) {
      continue;
    }
    // destination not reachable
    if ((!bfs && distances[currentIndex] == INF) || 
        (bfs && intDistances[currentIndex] == INT_INF)) {
      cerr << "Error: destination not reachable" << endl;
      continue;
    }
    
    // for each adjacent node
    for (int neighbor : (*graph)[currentIndex]) {
      if (!bfs) { // normal dijkstra
        double stepDist = costs[currentIndex][neighbor];
        // sanity check
        if (stepDist == INF) {
          cerr << "Error: edge has infinite cost" << endl;
        }
        // do something if new distance is less than recorded shortest distance
        double newDist = distances[currentIndex] + stepDist;
        if (distances[neighbor] > newDist) {
          pq.push({ neighbor, newDist, -1 });
          distances[neighbor] = newDist;
          parentNodes[neighbor] = currentIndex;
        }
      } else { // do bfs
        int newDist = intDistances[currentIndex] + 1;
        if (intDistances[neighbor] > newDist) {
          pq.push({ neighbor, -1, newDist});
          intDistances[neighbor] = newDist;
          parentNodes[neighbor] = currentIndex;
        }
      }
    }
  }
}

void ASPGameState::traverserMakeMove(int nextNode) {
  if (costs[currentNode][nextNode] == INF) {
    cerr << "Error: trying to move to a non-adjacent node";
  } else {
    currentNode = nextNode;
  }
}

void ASPGameState::adversaryMakeMove(int node1, int node2) {
  if (costs[node1][node2] == INF) {
    cerr << "Error: trying to increase cost of a non-existent edge";
  } else {
    int k = min(intDistances[node1], intDistances[node2]);
    double newCost = costs[node1][node2] * (1 + sqrt(k));
    costs[node1][node2] = newCost;
    costs[node2][node1] = newCost;
    computeDijkstra(destNode, false);
  }
}

pair<double, vector<int>> ASPGameState::getShortestPath(int source, int dest, bool bfs) {
  // compute all shortest paths from destination
  computeDijkstra(dest, bfs);
  vector<int> path;
  double pathCost = 0;
  
  // construct shortest path
  if (parentNodes[source] == -1) {
    cerr << "Error: destination not reachable" << endl;
    return make_pair(-1, path);
  }
  path.push_back(source);
  int next = parentNodes[source];
  while (next != dest) {
    path.push_back(next);
    next = parentNodes[next];
  }
  path.push_back(dest);

  if (bfs) {
    return make_pair(path.size() - 1, path);
  }

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
