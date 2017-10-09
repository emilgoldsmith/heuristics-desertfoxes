#include "move.h"
#include "asp_game_state.h"
#include "solve.h"

#include <vector>
#include <algorithm>
#include <bitset>

using namespace std;

Move miniMaxAdversary(ASPGameState *state, bitset<1010> visited, long double alpha, long double beta) {
  int *parentNodes = state->parentNodes;
  long double *distances = state->distances;
  int currentNode = state->currentNode;
  int destNode = state->destNode;
  if (currentNode == destNode) {
    return {
      -1,
      -1,
      0
    };
  }
  Move bestMove = {
    -1,
    -1,
    -1
  };
  for (; currentNode != destNode && parentNodes[currentNode] != -1; currentNode = parentNodes[currentNode]) {
    ASPGameState stateCopy(*state);
    stateCopy.adversaryMakeMove(currentNode, parentNodes[currentNode]);
    long double pathLength = miniMaxTraverser(&stateCopy, visited, alpha, beta).costRelatedInfo;
    if (pathLength > bestMove.costRelatedInfo) {
      bestMove = {
        currentNode,
        parentNodes[currentNode],
        pathLength
      };
      if (pathLength > alpha) {
        alpha = pathLength;
      }
      if (beta <= alpha) {
        break;
      }
    }
  }
  return bestMove;
}

Move miniMaxTraverser(ASPGameState *state, bitset<1010> visited, long double alpha, long double beta) {
  vector<vector<int> > *graph = state->graph;
  int currentNode = state->currentNode;
  if (currentNode == state->destNode) {
    return {
      -1,
      -1,
      0
    };
  }
  visited.set(currentNode);
  Move bestMove = {
    -1,
    -1,
    state->INF
  };
  for (int neighbour : (*graph)[currentNode]) {
    if (visited.test(neighbour)) {
      continue;
    }
    ASPGameState stateCopy(*state);
    stateCopy.traverserMakeMove(neighbour);
    long double pathLength = miniMaxAdversary(&stateCopy, visited, alpha, beta).costRelatedInfo;
    if (pathLength < bestMove.costRelatedInfo) {
      bestMove = {
        currentNode,
        neighbour,
        pathLength + (state->costs)[currentNode][neighbour]
      };
      if (pathLength < beta) {
        beta = pathLength;
      }
      if (beta <= alpha) {
        break;
      }
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
