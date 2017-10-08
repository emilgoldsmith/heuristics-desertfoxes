#include "move.h"

#include <vector>
#include <algorithm>

using namespace std;

Move miniMaxAdversary(ASPGameState *state) {
  int *parentNodes = state->parentNodes;
  double *distances = state->distances;
  int curNode = state->curNode;
  int destNode = state->destNode;
  Move bestMove = {
    -1,
    -1,
    -1
  };
  for (; curNode != destNode && parentNodes[curNode] != -1; curNode = parentNodes[curNode]) {
    ASPGameState stateCopy(*state);
    stateCopy->adversaryMakeMove(curNode, parentNodes[curNode]);
    double pathLength = miniMaxTraverser(&stateCopy).costRelatedInfo;
    if (pathLength > bestMove.costRelatedInfo) {
      bestMove = {
        curNode,
        parentNodes[curNode],
        pathLength
      };
    }
  }
  return bestMove;
}

Move miniMaxTraverser(ASPGameState *state) {
  vector<vector<int> > *graph = state->graph;
  int curNode = state->curNode;
  Move bestMove = {
    -1,
    -1,
    state->INF
  };
  for (int neighbour : (*graph)[curNode]) {
    ASPGameState stateCopy(*state);
    stateCopy->traverserMakeMove(neighbour);
    double pathLength = miniMaxAdversary(&stateCopy).costRelatedInfo;
    if (pathLength < bestMove.costRelatedInfo) {
      bestMove = {
        curNode,
        neighbour,
        pathLength
      };
    }
  }
  return bestMove;
}

Move getTraverseMove(vector<vector<int> > *adjacencyList) {
  return {
    0,
    (*adjacencyList)[0][0],
    0
  };
}

Move getAdversaryMove(vector<vector<int> > *adjacencyList) {
  return {
    0,
    (*adjacencyList)[0][0],
    1.5
  };
}
