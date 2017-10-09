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

void bubbleSort(ASPGameState *state, int currentNode) {
  bool isSorted = false;
  vector<int>& v = (*(state->graph))[currentNode];
  int cnt = 0;
  while (!isSorted) {
    cnt++;
    isSorted = true;
    for (int i = 0; i < v.size() - 1; i++) {
      if (state->distances[v[i]] > state->distances[v[i+1]]) {
        swap(v[i], v[i+1]);
        isSorted = false;
      }
    }
  }
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



  // sort((*graph)[currentNode].begin(), (*graph)[currentNode].end(),
  //     [state] (const int& a, const int& b) {
  //   return state->distances[a] < state->distances[b];
  // });

  bubbleSort(state, currentNode); // For 1000 sorts this outperformed the above by a factor of 2 on the 300 node graph, so we keep it for now

  int last = (*graph)[currentNode][0];
  for (int neighbour: (*graph)[currentNode]) {
    if (state->distances[last] > state->distances[neighbour]) {
      cerr << "FOUL" << endl;
    }
  }
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
