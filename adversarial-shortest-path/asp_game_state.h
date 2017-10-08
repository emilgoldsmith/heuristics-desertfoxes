#ifndef ASP_GAME_STATE
#define ASP_GAME_STATE

#include <vector>
#include <utility>
#include <limits>

struct Node {
  int index; // up to 1000 nodes
  double doubleDist; // distance (cost) of node to source
  int intDist; // number of edges from node to source
};

auto comp = [] (Node &a, Node &b) -> bool { 
  if (a.doubleDist == -1 || b.doubleDist == -1) {
    return a.intDist > b.intDist;
  }
  return a.doubleDist > b.doubleDist; 
};

class ASPGameState {
public:
  const double INF = std::numeric_limits<double>::infinity();
  const int INT_INF = std::numeric_limits<int>::max();
  std::vector<std::vector<int>> *graph;
  int currentNode; // where the traverser is now
  int destNode; // destination node

  // stores cost of each edge of the graph
  double **costs;
  // stores the factor k of each edge (min distance to destination)
  int **factors;
  // store the next node of each node's shortest path to destination
  int *parentNodes;
  // stores the shortest distance of each node from the destination
  double *distances; // double for edges with costs
  int *intDistances; // int for unit-length edges (bfs)
  
  ASPGameState(std::vector<std::vector<int>> *g, int source, int dest);
  ~ASPGameState();

  void computeDijkstra(int source, bool bfs);
  void traverserMakeMove(int nextNode);
  void adversaryMakeMove(int node1, int node2);
  std::pair<double, std::vector<int>> getShortestPath(int source, int dest, bool bfs);
  void updateCost(int node1, int node2, double newCost);
};

#endif
