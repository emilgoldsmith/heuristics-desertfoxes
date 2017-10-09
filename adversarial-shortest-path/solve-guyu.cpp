#include "solve-guyu.h"
#include "asp_game_state.h"
#include "move.h"

#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <map>

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

void mehDFSHelper(ASPGameState *gs, int depth, map<pair<int, int>, int> *edgeMap) {
  if (depth < 0) {
    return;
  }
  // insert the current shortest path
  int currentNode = gs->currentNode;
  int *parentNodes = gs->parentNodes;
  for (; currentNode != gs->destNode; currentNode = parentNodes[currentNode]) {
    // edge should be the same regardless of order of nodes
    pair<int, int> leftFirst = make_pair(currentNode, parentNodes[currentNode]);
    pair<int, int> rightFirst = make_pair(parentNodes[currentNode], currentNode);
    if (edgeMap->count(leftFirst) == 0 && edgeMap->count(rightFirst) == 0) {
      // default to left first
      (*edgeMap)[leftFirst] = 1;
    } else if (edgeMap->count(leftFirst) > 0) {
      (*edgeMap)[leftFirst] = (*edgeMap)[leftFirst] + 1;
    } else if (edgeMap->count(rightFirst) > 0) {
      (*edgeMap)[rightFirst] = (*edgeMap)[rightFirst] + 1;
    } else {
      cerr << "Error: same edge stored in two different ways" << endl;
    }
  }

  // recursively insert shortest paths at the next level
  for (int neighbor: (*gs->graph)[gs->currentNode]) {
    if (gs->traversedNodes[neighbor]) {
      continue;
    }
    ASPGameState gsCopy(*gs);
    gsCopy.traverserMakeMove(neighbor);
    mehDFSHelper(&gsCopy, depth - 1, edgeMap);
  }
}

Move mehGuyuAdversary(ASPGameState *gs, int depth) {
  map<pair<int, int>, int> edgeMap;
  mehDFSHelper(gs, depth, &edgeMap);

  int currentNode = gs->currentNode;
  int *parentNodes = gs->parentNodes;
  pair<int, int> chosenEdge = make_pair(gs->currentNode, gs->parentNodes[gs->currentNode]);
  long double maxScore = -1;
  
  // for each edge on the current shortest path
  for (; currentNode != gs->destNode; currentNode = parentNodes[currentNode]) {
    // edge should be the same regardless of order of nodes
    pair<int, int> leftFirst = make_pair(currentNode, parentNodes[currentNode]);
    pair<int, int> rightFirst = make_pair(parentNodes[currentNode], currentNode);
    long double edgeScore = -1;
    if (edgeMap.count(leftFirst) > 0) {
      int frequency = edgeMap[leftFirst];
      int increaseFactor = min(gs->intDistances[leftFirst.first], gs->intDistances[leftFirst.second]);
      long double delta = gs->costs[leftFirst.first][leftFirst.second] * increaseFactor;
      edgeScore = frequency * delta;
    } else if (edgeMap.count(rightFirst) > 0) {
      int frequency = edgeMap[rightFirst];
      int increaseFactor = min(gs->intDistances[rightFirst.first], gs->intDistances[rightFirst.second]);
      long double delta = gs->costs[rightFirst.first][rightFirst.second] * increaseFactor;
      edgeScore = frequency * delta;
    }
    if (edgeScore > maxScore) {
      chosenEdge = leftFirst;
      maxScore = edgeScore;
    }
  }
  
  return { chosenEdge.first, chosenEdge.second };
}

Move smartGuyuAdversary(ASPGameState *gs) {
  // this adversary try increasing the cost of each edge repeatedly
  // until the edge is no longer in the shortest path
  // or the number of times the edge cost is increased exceeds the number of
  // edges in between this edge and the source

  pair<int, int> bestEdge = make_pair(gs->currentNode, gs->parentNodes[gs->currentNode]);
  long double maxIncrease = -1;
  int edgeDistance = 1;

  // for each edge
  for (int currentNode = gs->currentNode; currentNode != gs->destNode; currentNode = gs->parentNodes[currentNode]) {
    pair<int, int> currentEdge = make_pair(currentNode, gs->parentNodes[currentNode]);
    // break if adjacent to destination node (can't increase the cost at all)
    if (currentEdge.second == gs->destNode) {
      break;
    }

    long double currentCost = gs->costs[currentEdge.first][currentEdge.second];
    long double newCost = currentCost;
    ASPGameState gsCopy(*gs);

    // iteratively increase edge cost
    for (int i = 0; i < edgeDistance; i++) {
      gsCopy.adversaryMakeMove(currentEdge.first, currentEdge.second);
      bool inShortestPath = false;

      // check if the edge is still in the shortest path
      for (int currentNodeCopy = gsCopy.currentNode; currentNodeCopy != gsCopy.destNode; currentNodeCopy = gsCopy.parentNodes[currentNodeCopy]) {
        pair<int, int> currentEdgeCopy = make_pair(currentNodeCopy, gsCopy.parentNodes[currentNodeCopy]);
        if (currentEdge.first == currentEdgeCopy.first && currentEdge.second == currentEdgeCopy.second) {
          inShortestPath = true;
          break;
        } else if (currentEdge.first == currentEdgeCopy.second && currentEdge.second == currentEdgeCopy.first) {
          inShortestPath = true;
          break;
        }
      }

      // not in shortest path anymore, just break
      if (!inShortestPath) {
        break;
      // still in shortest path, update new cost
      } else {
        newCost = gsCopy.costs[currentEdge.first][currentEdge.second];
      }
    }

    long double currentIncrease = newCost - currentCost;
    if (currentIncrease > maxIncrease) {
      maxIncrease = currentIncrease;
      bestEdge = currentEdge;
    }
    edgeDistance++;
  }

  return { bestEdge.first, bestEdge.second, maxIncrease };
}
