#ifndef ASP_GAME_STATE
#define ASP_GAME_STATE

#include <vector>
#include <utility>
#include <limits>

struct Node {
  int index; // up to 1000 nodes
  double dist; // distance (cost) of node to source
};

auto comp = [] (Node &a, Node &b) -> bool { return a.dist > b.dist; };

class ASPGameState {
  const double INF = std::numeric_limits<double>::infinity();

public:
  std::vector<std::vector<int>> *graph;
  double **costs;
  
  ASPGameState(std::vector<std::vector<int>> *g);
  ~ASPGameState();
  // return the shortest path cost, along with the shortest path (a list of nodes)
  std::pair<double, std::vector<int>> getShortestPath(int source, int dest);
  // update the cost of traversing from node1 to node2
  void updateCost(int node1, int node2, double newCost);
};

#endif
