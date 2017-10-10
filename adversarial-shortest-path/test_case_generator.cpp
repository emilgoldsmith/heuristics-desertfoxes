#include <iostream>
#include <random>
#include <set>
#include <utility>
#include <algorithm>
#include <chrono>

using namespace std;

bool adj[1010][1010];

bool visited[1010];

pair<int, int> dfs(int n, int depth, int numNodes) {
  visited[n] = true;
  pair<int, int> best = {depth, n};
  for (int i = 0; i < numNodes; i++) {
    if (adj[n][i] && !visited[i]) {
      pair<int, int> candidate = dfs(i, depth + 1, numNodes);
      if (candidate > best) {
        best = candidate;
      }
    }
  }
  return best;
}

int main() {
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  default_random_engine generator(seed);
  uniform_int_distribution<int> numNodesDist(300, 1000);
  int numNodes = numNodesDist(generator);
  int maxNumEdges = (numNodes * (numNodes - 1)) / 2;
  uniform_int_distribution<int> numEdgesDist(numNodes - 1, maxNumEdges);
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
    visited[i] = false;
  }
  pair<int, int>  endPair = dfs(startNode, 0, numNodes);
  cout << "Starting node: " << startNode << endl;
  cout << "Ending node: " << endPair.second << endl;
  cout << "Edges:" << endl;
  for (int i = 0; i < numNodes; i++) {
    for (int j = i + 1; j < numNodes; j++) {
      if (adj[i][j]) {
        cout << i << ' ' << j << endl;
      }
    }
  }
}

