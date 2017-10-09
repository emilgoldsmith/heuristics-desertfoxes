#include "move.h"
#include "asp_game_state.h"
#include "solve.h"

#include <vector>
#include <algorithm>
#include <bitset>
#include <set>
#include <utility>
#include <sstream>
#include <string>
#include <unordered_map>

#include <iostream>

using namespace std;

string getStateString(int currentNode, bool isAdversary, customSet changedEdges) {
  stringstream ss;
  if (isAdversary) {
    ss << "x";
  } else {
    ss << "y";
  }
  ss << currentNode << ' ';
  for (piild node : changedEdges) {
    ss << node.first << ' ' << node.second << ' ';
  }
  return ss.str();
}

unordered_map<string, Move> mem;
int cnt = 0;
int cnt2 = 0;

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
  if (depth < 0 || (depth - state->intDistances[currentNode]) < 0) {
//     return {
//       -1,
//       -1,
//       state->INF
//     };
  }
//   cnt++;
//   string stateString = getStateString(currentNode, true, changedEdges);
 //  if (mem.count(stateString)) {
 //    cnt2++;
 //    return mem[stateString];
 //  }
  Move bestMove = {
    -1,
    -1,
    -1
  };
  for (; currentNode != destNode && parentNodes[currentNode] != -1; currentNode = parentNodes[currentNode]) {
    // Copy game state
    ASPGameState stateCopy(*state);
    stateCopy.adversaryMakeMove(currentNode, parentNodes[currentNode]);

    // Copy set, and add new info (as it's a set duplicates will be ignored)
   //  customSet changedEdgesCopy(changedEdges);
   //  piild node = {currentNode, parentNodes[currentNode]};
   //  if (node.first > node.second) {
   //    swap(node.first, node.second);
   //  }
   //  changedEdgesCopy.insert(node);

    // Call the Traverser recursively
    long double pathLength = miniMaxTraverser(&stateCopy, alpha, beta, depth, currentCost).costRelatedInfo;
    if (pathLength > bestMove.costRelatedInfo) {
      bestMove = {
        currentNode,
        parentNodes[currentNode],
        pathLength
      };
      if (pathLength + currentCost > alpha) {
        alpha = pathLength + currentCost;
      }
      if (beta <= alpha) {
        break;
      }
    }
  }
  // mem[stateString] = bestMove;
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
  // We know of a lowerbound on the length we can go
  if (state->distances[currentNode] + currentCost >= beta) {
    // Not worth it as it's impossible improving beta
    return {
      -1,
      -1,
      -1
    };
  }
//   string stateString = getStateString(currentNode, false, changedEdges);
  // cnt++;
 //  if (mem.count(stateString)) {
 //    cnt2++;
 //    cout << stateString << endl;
 //    return mem[stateString];
 //  }
 //  if (cnt % 1000 == 0) {
 //    cout << "Buckets: " << mem.bucket_count() << endl;
 //    cout << cnt2 << "/" << cnt << " hits: " << ((double)100 * cnt2 / cnt) << "%" << endl;
 //  }
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

  for (int neighbour : (*graph)[currentNode]) {
    ASPGameState stateCopy(*state);
    stateCopy.traverserMakeMove(neighbour);
    long double addedCost = state->costs[currentNode][neighbour];
    long double pathLength = miniMaxAdversary(&stateCopy, alpha, beta, depth - 1, currentCost + addedCost).costRelatedInfo;
    if (pathLength < bestMove.costRelatedInfo) {
      bestMove = {
        currentNode,
        neighbour,
        pathLength + addedCost
      };
      if (pathLength + currentCost + addedCost < beta) {
        beta = pathLength + currentCost + addedCost;
      }
      if (beta <= alpha) {
        break;
      }
    }
  }
  // mem[stateString] = bestMove;
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
