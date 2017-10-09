#include "asp_game_state.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>
#include <limits>
#include <cmath>
#include <cstring>

using namespace std;

ASPGameState::ASPGameState(vector<vector<int>> *g, int source, int dest) {
  graph = g;
  currentNode = source;
  destNode = dest;

  int size = g->size();
  costs = new long double*[size];
  // contiguous memory allocation hack
  costs[0] = new long double[size * size];
  for (int i = 1; i < size; i++) {
    costs[i] = costs[i-1] + size;
  }
  parentNodes = new int[size];
  distances = new long double[size];
  // Tells destructor that it's supposed to free this
  intDistancesOwner = true;
  intDistances = new int[size];

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

  // Sort all the adjacent edges initially
  for (vector<int>& v : *graph) {
    sort(v.begin(), v.end(),
        [this] (const int& a, const int& b) {
      return this->distances[a] < this->distances[b];
    });
  }
}

ASPGameState::ASPGameState(ASPGameState &gs) {
  graph = gs.graph;
  intDistances = gs.intDistances;
  currentNode = gs.currentNode;
  destNode = gs.destNode;

  // deep copy of everything
  int size = graph->size();
  costs = new long double*[size];
  // contiguous memory allocation hack
  costs[0] = new long double[size * size];
  for (int i = 1; i < size; i++) {
    costs[i] = costs[i-1] + size;
  }
  parentNodes = new int[size];
  distances = new long double[size];

  memcpy(costs[0], gs.costs[0], size*size*sizeof(long double));
  memcpy(parentNodes, gs.parentNodes, size*sizeof(int));
  memcpy(distances, gs.distances, size*sizeof(long double));
}

ASPGameState::~ASPGameState() {
  delete [] costs[0];
  delete [] costs;
  delete [] parentNodes;
  delete [] distances;
  if (intDistancesOwner) {
    delete [] intDistances;
  }
}

void ASPGameState::computeDijkstra(int source, bool bfs, bool invalidator, int node1, int node2) {
  if (invalidator) {
    if (parentNodes[node1] != node2 && parentNodes[node2] != node1) {
      // Refer to Emil for proof
      return;
    }
    if (distances[node2] > distances[node1]) swap(node1, node2);
    // now node1 is furthest away, as one must be further away since the edge lies on a shortest path
    int nb = (*graph)[node1][0]; // There is a chance the minimax sorted it and it's kind of a free guess
    if ((costs[node1][nb] + distances[nb]) == distances[node1]) {
      // we have an alternative path, right now distances[node1] refers to the old best path
      return;
    }
  }

  char *visited = new char[(graph->size() + 7) >> 3];
  // min heap sorted by distance
  priority_queue<Node, vector<Node>, decltype(comp)> pq(comp);
  // initializations
  distances[source] = 0;
  if (bfs) {
    intDistances[source] = 0;
  }
  pq.push({ source, 0, 0});
  // The two below only work with 0 and -1 as those are 00000000 and ffffffff in hex
  memset(visited, 0, (graph->size() + 7) >> 3);
  memset(parentNodes, -1, sizeof(int) * graph->size());
  for (int i = 0; i < graph->size(); i++) {
    if (i != source) {
      // distance initialized to INF besides source node
      distances[i] = INF;
      if (bfs) {
        intDistances[i] = INT_INF;
      }
    }
  }

  // main dijkstra routine
  while (!pq.empty()) {
    // pop the node currently closest to the destination
    Node currentNode = pq.top();
    int currentIndex = currentNode.index;
    pq.pop();

    visited[currentIndex >> 3] |= (1 << (currentIndex % 8));
    // destination not reachable
    if ((!bfs && distances[currentIndex] == INF) ||
        (bfs && intDistances[currentIndex] == INT_INF)) {
      cerr << "Error: destination not reachable" << endl;
      continue;
    }

    // for each adjacent node
    for (int neighbor : (*graph)[currentIndex]) {
      if (visited[neighbor >> 3] & (1 << (neighbor % 8))) {
        continue;
      }
      if (!bfs) { // normal dijkstra
        long double stepDist = costs[currentIndex][neighbor];
        // sanity check
        if (stepDist == INF) {
          cerr << "Error: edge has infinite cost" << endl;
        }
        // do something if new distance is less than recorded shortest distance
        long double newDist = distances[currentIndex] + stepDist;
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
  delete [] visited;
}

void ASPGameState::traverserMakeMove(int nextNode) {
  if (costs[currentNode][nextNode] == INF) {
    cerr << "Error: trying to move to a non-adjacent node: ";
    cerr << currentNode << "->" << nextNode << endl;
  } else {
    currentNode = nextNode;
  }
}

void ASPGameState::adversaryMakeMove(int node1, int node2) {
  if (costs[node1][node2] == INF) {
    cerr << "Error: trying to increase cost of a non-existent edge: ";
    cerr << node1 << " " << node2 << endl;
  } else {
    int k = min(intDistances[node1], intDistances[node2]);
    long double newCost = costs[node1][node2] * (1 + sqrt(k));
    costs[node1][node2] = newCost;
    costs[node2][node1] = newCost;
    computeDijkstra(destNode, false, true, node1, node2);
  }
}
