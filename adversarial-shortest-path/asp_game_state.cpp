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
  visited = new bool[size];
  // Tells destructor that it's supposed to free this
  intDistancesOwner = true;
  intDistances = new int[size];
  traversedNodes = new bool[size];
  for (int i = 0; i < size; i++) {
    traversedNodes[i] = false;
  }
  traversedNodes[currentNode] = true;

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
  traversedNodes = new bool[size];

  memcpy(costs[0], gs.costs[0], size*size*sizeof(long double));
  memcpy(parentNodes, gs.parentNodes, size*sizeof(int));
  memcpy(distances, gs.distances, size*sizeof(long double));
  memcpy(traversedNodes, gs.traversedNodes, size*sizeof(bool));

  visited = new bool[size];
}

ASPGameState::~ASPGameState() {
  delete [] costs[0];
  delete [] costs;
  delete [] parentNodes;
  delete [] distances;
  delete [] traversedNodes;
  delete [] visited;
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
    for (int& nb : (*graph)[node1]) {
      if ((costs[node1][nb] + distances[nb]) == distances[node1]) {
        // we have an alternative path, right now distances[node1] refers to the old best path
        return;
      }
    }
  }
    // Node completeNode1 = {node1, distances[node1], -1};
    // auto it1 = lower_bound(dijkstraOrder.begin(), dijkstraOrder.end(), completeNode1, compOpposite);
    // while ((*it1).index != node1) it1++;
    // Node completeNode2 = {node2, distances[node2], -1};
    // auto it2 = lower_bound(dijkstraOrder.begin(), dijkstraOrder.end(), completeNode2, compOpposite);
    // while ((*it2).index != node2) it2++;
    // // We want it1 to be furthest along in the vector
    // if (distance(it1, it2) > 0) swap(it1, it2);
    // for (; it1 != dijkstraOrder.end(); it1++) {
    //   visited[(*it1).index] = false;
    //   parentNodes[(*it1).index] = -1;
    //   distances[(*it1).index] = INF;
    // }
    // dijkstraOrder.erase(it1, dijkstraOrder.end());
    // for (Node& finishedNode : dijkstraOrder) {
    //   int finishedIndex = finishedNode.index;
    //   for (int& nb : (*graph)[finishedIndex]) {
    //     long double newDist = distances[finishedIndex] + costs[finishedIndex][nb];
    //     if (!visited[nb] && distances[nb] > newDist) {
    //       pq.push({ nb, newDist, -1});
    //       distances[nb] = newDist;
    //       parentNodes[nb] = finishedIndex;
    //     }
    //   }
    // }


  // min heap sorted by distance
  priority_queue<Node, vector<Node>, decltype(comp)> pq(comp);
  // initializations
  distances[source] = 0;
  if (bfs) {
    intDistances[source] = 0;
  }
  pq.push({ source, 0, 0});
  for (int i = 0; i < graph->size(); i++) {
    visited[i] = false;
    parentNodes[i] = -1;
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
}

void ASPGameState::traverserMakeMove(int nextNode) {
  if (costs[currentNode][nextNode] == INF) {
    cerr << "Error: trying to move to a non-adjacent node: ";
    cerr << currentNode << "->" << nextNode << endl;
  } else {
    currentNode = nextNode;
    traversedNodes[currentNode] = true;
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

pair<long double, vector<int>> ASPGameState::getShortestPath(int source, int dest, bool bfs) {
  // compute all shortest paths from destination
  computeDijkstra(dest, bfs);
  vector<int> path;
  long double pathCost = 0;

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

void ASPGameState::updateCost(int node1, int node2, long double newCost) {
  if (node1 < 0 || node2 < 0 || node1 >= graph->size() || node2 >= graph->size()) {
    cerr << "Error: cost update failed, node does not exist" << endl;
    return;
  }
  costs[node1][node2] = newCost;
  costs[node2][node1] = newCost;
}
