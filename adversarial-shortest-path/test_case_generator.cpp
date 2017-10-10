#include <iostream>
#include <random>
#include <set>
#include <utility>
#include <algorithm>
#include <chrono>

using namespace std;

bool adj[1010][1010];
int costs[1010][1010];

int main() {
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  default_random_engine generator(seed);
  uniform_int_distribution<int> numNodesDist(300, 1000);
  int numNodes = numNodesDist(generator);
  int maxNumEdges = (numNodes * (numNodes - 1)) / 2;
  uniform_int_distribution<int> numEdgesDist(numNodes - 1, numNodes * 3);
  int numEdges = numEdgesDist(generator);

  uniform_int_distribution<int> nodeDist(0, numNodes - 1);
  int startNode = nodeDist(generator);

  for (int i = 1; i < numNodes; i++) {
    uniform_int_distribution<int> targetNodeDist(0, i-1);
    int targetNode = targetNodeDist(generator);

    adj[i][targetNode] = adj[targetNode][i] = true;
  }

  numEdges -= numNodes - 1;
  while (numEdges--) {
    int a, b;
    a = nodeDist(generator);
    b = nodeDist(generator);
    while (adj[a][b] || a == b) {
      a = nodeDist(generator);
      b = nodeDist(generator);
    }
    adj[a][b] = adj[b][a] = true;
  }

  for (int i = 0; i < numNodes; i++) {
    for (int j = 0; j < numNodes; j++) {
      if (i == j) {
        costs[i][j] = 0;
      } else {
        if (adj[i][j]) {
          costs[i][j] = 1;
        } else {
          costs[i][j] = 101000;
        }
      }
    }
  }
  // Floyd-Warshall
  for (int k = 0; k < numNodes; k++) {
    for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numNodes; j++) {
        costs[i][j] = min(costs[i][j], costs[i][k] + costs[k][j]);
      }
    }
  }
  int endNode = 0;
  for (int i = 0; i < numNodes; i++) {
    if (costs[startNode][endNode] < costs[startNode][i]) {
      endNode = i;
    }
  }
  cerr << costs[startNode][endNode] << endl;
  // Find diameter
  startNode = endNode;
  for (int i = 0; i < numNodes; i++) {
    for (int j = 0; j < numNodes; j++) {
      if (i == j) {
        costs[i][j] = 0;
      } else {
        if (adj[i][j]) {
          costs[i][j] = 1;
        } else {
          costs[i][j] = 101000;
        }
      }
    }
  }
  for (int k = 0; k < numNodes; k++) {
    for (int i = 0; i < numNodes; i++) {
      for (int j = 0; j < numNodes; j++) {
        costs[i][j] = min(costs[i][j], costs[i][k] + costs[k][j]);
      }
    }
  }
  endNode = 0;
  for (int i = 0; i < numNodes; i++) {
    if (costs[startNode][endNode] < costs[startNode][i]) {
      endNode = i;
    }
  }
  cerr << costs[startNode][endNode] << endl;
  cout << "Starting node: " << startNode << endl;
  cout << "Ending node: " << endNode << endl;
  cout << "Edges:" << endl;
  for (int i = 0; i < numNodes; i++) {
    for (int j = i + 1; j < numNodes; j++) {
      if (adj[i][j]) {
        cout << i << ' ' << j << endl;
      }
    }
  }
}

