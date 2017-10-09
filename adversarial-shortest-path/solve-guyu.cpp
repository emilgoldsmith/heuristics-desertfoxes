#include "solve-guyu.h"
#include "asp_game_state.h"
#include "move.h"

#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

Move guyuTraverser(ASPGameState *gs) {
  return { gs->currentNode, gs->parentNodes[gs->currentNode] };
}

Move guyuAdversary(ASPGameState *gs) {
  return guyuTraverser(gs);
}

Move smartGuyuTraverser(ASPGameState *gs) {
  return guyuTraverser(gs);
}

void dfsHelper(ASPGameState *gs, int depth, multiset<pair<int, int>> *edgeSet) {
  if (depth < 0) {
    return;
  }
  // insert the current shortest path
  int currentNode = gs->currentNode;
  int *parentNodes = gs->parentNodes;
  for (; currentNode != gs->destNode; currentNode = parentNodes[currentNode]) {
    // edge should be the same regardless of order of nodes
    // always default to "right first"
    pair<int, int> leftFirst = make_pair(currentNode, parentNodes[currentNode]);
    if (edgeSet->count(leftFirst) != 0) {
      edgeSet->insert(leftFirst);
    } else {
      edgeSet->insert(make_pair(parentNodes[currentNode], currentNode));
    }
  }

  // recursively insert shortest paths at the next level
  for (int neighbor: (*gs->graph)[currentNode]) {
    ASPGameState gsCopy(*gs);
    gsCopy.traverserMakeMove(neighbor);
    dfsHelper(&gsCopy, depth - 1, edgeSet);
  }
}

Move smartGuyuAdversary(ASPGameState *gs, int depth) {
  multiset<pair<int, int>> edgeSet;
  dfsHelper(gs, depth, &edgeSet);
  pair<int, int> chosenEdge = make_pair(-1, -1);
  int maxFrequency = -1;
  long double maxCost = -1;

  for (auto &edge : edgeSet) {
    int frequency = edgeSet.count(edge);
    if (frequency > maxFrequency) {
      chosenEdge = edge;
      maxFrequency = frequency;
    }
  }
  
  return { chosenEdge.first, chosenEdge.second };
}
