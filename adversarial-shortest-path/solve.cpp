#include "move.h"
#include "asp_game_state.h"
#include "solve.h"

#include <vector>
#include <algorithm>
#include <bitset>
#include <utility>
#include <sstream>
#include <string>
#include <unordered_map>

#include <iostream>

using namespace std;

string getStateString(int currentNode, bool isAdversary, ASPGameState *state) {
  stringstream ss;
  if (isAdversary) {
    ss << "x";
  } else {
    ss << "y";
  }
  ss << currentNode << ' ';
  for (pii node : state->changedEdges) {
    ss << node.first << ' ' << node.second << ' ' << state->costs[node.first][node.second];
  }
  return ss.str();
}

unordered_map<string, Move> mem(1000 * 1000);
// int cnt = 0;
// int cnt2 = 0;

Move miniMaxTraverser(ASPGameState *state, long double alpha, long double beta, int depth, long double currentCost);

Move miniMaxAdversary(ASPGameState *state, long double alpha, long double beta, int depth, long double currentCost) {
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
//  if (depth < 0 || (depth - state->intDistances[currentNode]) < 0) {
//     return {
//       -1,
//       -1,
//       state->INF
//     };
//  }
//   cnt++;
  string stateString = getStateString(currentNode, true, state);
  if (mem.count(stateString)) {
    return mem[stateString];
  }
  Move bestMove = {
    -1,
    -1,
    -1
  };
  for (; currentNode != destNode && parentNodes[currentNode] != -1; currentNode = parentNodes[currentNode]) {
    // Copy game state
    ASPGameState stateCopy(*state);
    stateCopy.adversaryMakeMove(currentNode, parentNodes[currentNode]);
    // We can already now compute a lowerbound on the score we will get using Dijkstra
    alpha = max(alpha, currentCost + stateCopy.distances[stateCopy.currentNode]);
    if (alpha >= beta) {
      bestMove = {
        -1,
        -1,
        alpha
      };
      break;
    }


    // Call the Traverser recursively
    long double pathLength = miniMaxTraverser(&stateCopy, alpha, beta, depth, currentCost).costRelatedInfo;
    if (pathLength > bestMove.costRelatedInfo) {
      bestMove = {
        currentNode,
        parentNodes[currentNode],
        pathLength
      };
      alpha = max(alpha, currentCost + pathLength);
      if (beta <= alpha) {
        break;
      }
    }
  }
  mem[stateString] = bestMove;
  return bestMove;
}

void bubbleSort(ASPGameState *state, int currentNode) {
  bool isSorted = false;
  vector<int>& v = (*(state->graph))[currentNode];
  int cnt = 0;
  while (!isSorted) {
    isSorted = true;
    for (int i = 0; i < v.size() - 1; i++) {
      if (state->distances[v[i]] > state->distances[v[i+1]]) {
        swap(v[i], v[i+1]);
        isSorted = false;
      }
    }
  }
}

Move miniMaxTraverser(ASPGameState *state, long double alpha, long double beta, int depth, long double currentCost) {
  vector<vector<int> > *graph = state->graph;
  int currentNode = state->currentNode;
  if (currentNode == state->destNode) {
    return {
      -1,
      -1,
      0
    };
  }
  string stateString = getStateString(currentNode, false, state);
  if (mem.count(stateString)) {
    return mem[stateString];
  }
  Move bestMove = {
    -1,
    -1,
    state->INF
  };

  bubbleSort(state, currentNode); // This outperforms std::sort for our usecase

  for (int neighbour : (*graph)[currentNode]) {
    ASPGameState stateCopy(*state);
    stateCopy.traverserMakeMove(neighbour);
    long double addedCost = state->costs[currentNode][neighbour];
    long double pathLength = miniMaxAdversary(&stateCopy, alpha, beta, depth - 1, currentCost + addedCost).costRelatedInfo + addedCost;
    if (pathLength < bestMove.costRelatedInfo) {
      bestMove = {
        currentNode,
        neighbour,
        pathLength
      };
      beta = min(beta, pathLength + currentCost);
      if (beta <= alpha) {
        break;
      }
    }
  }
  mem[stateString] = bestMove;
  return bestMove;
}

Move getTraverseMove(ASPGameState *state, int minimax) {
  if (minimax) {
    return miniMaxTraverser(state, state->distances[state->currentNode], state->INF, -1, 0);
  } else {
    return {
      state->currentNode,
      state->parentNodes[state->currentNode]
    };
  }
}

Move getAdversaryMove(ASPGameState *state, int minimax) {
  if (minimax) {
    return miniMaxAdversary(state, state->distances[state->currentNode], state->INF, -1, 0);
  } else {
    return {
      state->currentNode,
      state->parentNodes[state->currentNode]
    };
  }
}
