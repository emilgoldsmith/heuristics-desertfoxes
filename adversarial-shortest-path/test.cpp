#include "asp_game_state.h"
#include <vector>
#include <utility>
#include <iostream>

using namespace std;

int main() {
  vector<vector<int>> graph {
    {1, 3},
    {0, 2, 3},
    {1, 4, 5},
    {0, 1, 4},
    {2, 3, 5},
    {2, 4}
  };
  ASPGameState gs(&graph, 0, 0);
  gs.updateCost(0, 1, 3);
  gs.updateCost(1, 2, 3);
  gs.updateCost(2, 5, 2);
  gs.updateCost(3, 4, 6);

  auto pathWithCost = gs.getShortestPath(0, 5, true);
  cout << "Cost: " << pathWithCost.first << endl;
  cout << "Shortest Path: " << endl;
  for (int node : pathWithCost.second) {
    cout << node << " ";
  }
  cout << endl;

  gs.adversaryMakeMove(0, 3);
  cout << gs.costs[0][3] << endl;
  return 0;
}
