#ifndef ASP_GAME_STATE
#define ASP_GAME_STATE

#include <vector>
#include <utility>
#include <limits>
#include <set>

typedef std::pair<int, int> pii;
typedef std::set<pii> customSet;

struct Node {
  int index; // up to 1000 nodes
  long double doubleDist; // distance (cost) of node to source
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
  constexpr static const long double INF = std::numeric_limits<long double>::infinity();
  constexpr static const int INT_INF = std::numeric_limits<int>::max();
  std::vector<std::vector<int>> *graph;
  int currentNode; // where the traverser is now
  int destNode; // destination node
  bool intDistancesOwner = false;

  // stores cost of each edge of the graph
  long double **costs;
  // store the next node of each node's shortest path to destination
  int *parentNodes;
  // stores the shortest distance of each node from the destination
  long double *distances; // double for edges with costs
  int *intDistances; // int for unit-length edges (bfs)
  customSet changedEdges;

  ASPGameState(std::vector<std::vector<int>> *g, int source, int dest);
  ASPGameState(ASPGameState &gs);
  ~ASPGameState();

  void computeDijkstra(int source, bool bfs, bool invalidator = false, int node1 = -1, int node2 = -1);
  void traverserMakeMove(int nextNode);
  void adversaryMakeMove(int node1, int node2);
};

#endif
