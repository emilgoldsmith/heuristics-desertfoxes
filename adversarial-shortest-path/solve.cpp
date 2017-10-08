#include "move.h"
#include "asp_game_state.h"
#include "solve.h"

#include <vector>
#include <algorithm>

using namespace std;

Move miniMaxAdversary(ASPGameState *state) {
  int *parentNodes = state->parentNodes;
  long double *distances = state->distances;
  int currentNode = state->currentNode;
  int destNode = state->destNode;
  Move bestMove = {
    -1,
    -1,
    -1
  };
  for (; currentNode != destNode && parentNodes[currentNode] != -1; currentNode = parentNodes[currentNode]) {
    ASPGameState stateCopy(*state);
    stateCopy.adversaryMakeMove(currentNode, parentNodes[currentNode]);
    long double pathLength = miniMaxTraverser(&stateCopy).costRelatedInfo;
    if (pathLength > bestMove.costRelatedInfo) {
      bestMove = {
        currentNode,
        parentNodes[currentNode],
        pathLength
      };
    }
  }
  return bestMove;
}

Move miniMaxTraverser(ASPGameState *state) {
  vector<vector<int> > *graph = state->graph;
  int currentNode = state->currentNode;
  Move bestMove = {
    -1,
    -1,
    state->INF
  };
  for (int neighbour : (*graph)[currentNode]) {
    ASPGameState stateCopy(*state);
    stateCopy.traverserMakeMove(neighbour);
    long double pathLength = miniMaxAdversary(&stateCopy).costRelatedInfo;
    if (pathLength < bestMove.costRelatedInfo) {
      bestMove = {
        currentNode,
        neighbour,
        pathLength
      };
    }
  }
  return bestMove;
}

Move getTraverseMove(ASPGameState *state) {
  return {
    state->currentNode,
    (*(state->graph))[state->currentNode][0]
  };
}

Move getAdversaryMove(ASPGameState *state) {
  return {
    state->currentNode,
    (*(state->graph))[state->currentNode][0]
  };
}
