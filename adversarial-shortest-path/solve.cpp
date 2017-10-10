#include "move.h"
#include "asp_game_state.h"
#include "solve.h"
#include "../timer/timer.h"
#include "solve-guyu.h"

#include <vector>
#include <algorithm>
#include <utility>
#include <sstream>
#include <string>
#include <unordered_map>
#include <stack>
#include <cmath>

#include <iostream>

using namespace std;

struct MemoEntry {
  Move move;
  bool wasCut;
};

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

unordered_map<string, MemoEntry> mem(1000 * 1000);
// int cnt = 0;
// int cnt2 = 0;
bool depthUsed = false;

const long double ALPHA_FACTOR = 1.5;

Move miniMaxTraverser(ASPGameState *state, long double alpha, long double beta, int depth, long double currentCost, Timer *t, double deadline, bool guyuHeuristic = false, bool emilHeuristic = false);

Move miniMaxAdversary(ASPGameState *state, long double alpha, long double beta, int depth, long double currentCost, Timer *t, double deadline, bool guyuHeuristic = false, bool emilHeuristic = false) {
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
  if (depth < 0 && emilHeuristic) {
    depthUsed = true;
    return {
      -1,
      -1,
      distances[currentNode] * ALPHA_FACTOR
    };
  }
  if (!emilHeuristic && depth > 0 && (depth - state->intDistances[currentNode]) < 0) {
    depthUsed = true;
    return {
      -1,
      -1,
      state->INF
    };
  }
  if (t->getTime() > deadline) {
    return {
      -1,
      -1,
      state->INF
    };
  }
  Move bestMove = {
    -1,
    -1,
    -1
  };
  string stateString;
  if (depth < 0 && !guyuHeuristic) {
    // We only use this for perfect search
    string stateString = getStateString(currentNode, true, state);
    if (mem.count(stateString)) {
      MemoEntry cached_entry = mem[stateString];
      if (!cached_entry.wasCut) {
        return cached_entry.move;
      } else {
        alpha = max(alpha, cached_entry.move.costRelatedInfo);
        bestMove = cached_entry.move;
      }
    }
  }

  bool pruned = false;
  for (; currentNode != destNode && parentNodes[currentNode] != -1; currentNode = parentNodes[currentNode]) {
    Move guyuMove;
    if (guyuHeuristic) {
      guyuMove = smartGuyuAdversary(state);
    }
    // Copy game state
    ASPGameState stateCopy(*state);
    if (guyuHeuristic) {
      stateCopy.adversaryMakeMove(guyuMove.node1, guyuMove.node2);
    } else {
      stateCopy.adversaryMakeMove(currentNode, parentNodes[currentNode]);
    }
    // We can already now compute a lowerbound on the score we will get using Dijkstra
    alpha = max(alpha, currentCost + stateCopy.distances[stateCopy.currentNode]);
    if (guyuHeuristic) {
      // We make a more "hardcore" estimate
      alpha = max(alpha, currentCost + stateCopy.distances[stateCopy.currentNode] * 2);
    }
    if (alpha >= beta) {
      bestMove = {
        currentNode,
        parentNodes[currentNode],
        alpha
      };
      pruned = true;
      break;
    }


    // Call the Traverser recursively
    long double pathLength = miniMaxTraverser(&stateCopy, alpha, beta, depth, currentCost, t, deadline, guyuHeuristic, emilHeuristic).costRelatedInfo;
    if (pathLength > bestMove.costRelatedInfo) {
      bestMove = {
        currentNode,
        parentNodes[currentNode],
        pathLength
      };
      alpha = max(alpha, currentCost + pathLength);
      if (beta <= alpha) {
        pruned = true;
        break;
      }
      if (guyuHeuristic) {
        // with guyuHeuristic we just do one iteration not the whole loop
        break;
      }
    }
  }
  if (depth < 0 && !guyuHeuristic) {
    if (t->getTime() > deadline) {
      // This means we didn't do a full search and this is not perfect
      pruned = true;
    }
    // Only use this when we're not doing heuristics
    MemoEntry entry = {bestMove, pruned};
//     if (!pruned) {
//       if (distances[currentNode] > 0) {
//         cout << "WOHOO" << endl;
//       }
//       cout << "Shortest path: " << distances[currentNode] << ' ' << state->intDistances[currentNode] << endl;
//       cout << "Actual cost: " << bestMove.costRelatedInfo << endl;
//       cout << "Ratio: " << bestMove.costRelatedInfo / distances[currentNode] << endl;
//     }
    mem[stateString] = entry;
  }
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

void insertionSort(ASPGameState *state, int currentNode, bool useBinarySearch) {
  vector<int>& v = (*(state->graph))[currentNode];
  long double *distances = state->distances;
  for (int i = v.size() - 2; i >= 0; i--) {
    if (distances[v[i]] > distances[v[i + 1]]) {
      if (useBinarySearch) {
        int high = v.size() - 1;
        int low = i + 1;
        for (int j = 0; j < 3; j++) {
          if (distances[v[i]] >= distances[v[high - j]]) {
            // We're done, we do this check as it seems likely
            low = high = high - j;
            break;
          }
        }
        while (low < high) {
          int mid = (low + high) >> 1;
          if (distances[v[i]] >= distances[v[mid]]) {
            high = mid;
          } else {
            low = mid + 1;
          }
        }
        v.insert(v.begin() + low + 1, v[i]);
        v.erase(v.begin() + i);
      } else {
        for (int j = v.size() - 1; j >= 0; j--) {
          if (distances[v[i]] >= distances[v[j]]) {
            v.insert(v.begin() + j + 1, v[i]);
            v.erase(v.begin() + i);
            break;
          }
        }
      }
    }
  }
}

struct M {
  int increments;
  int node;
};

long double getBetaEstimate(ASPGameState *state) {
  stack<M> s;
  long double currentCost = 0;
  for (int currentNode = state->currentNode; state->parentNodes[currentNode] != -1; currentNode = state->parentNodes[currentNode]) {
    int parent = state->parentNodes[currentNode];
    long double factor = sqrt(state->intDistances[parent]) + 1;
    long double curCost = state->costs[currentNode][parent] * factor;
    long double dif = factor * curCost - curCost;
    int increments = 1;
    while (s.size()) {
      M p = s.top();
      int candidateParent = state->parentNodes[p.node];
      long double candidateFactor = sqrt(state->intDistances[candidateParent]);
      long double candidateDif = state->costs[p.node][candidateParent] * (candidateFactor - 1) / candidateFactor;
      if (candidateDif > dif) break;
      s.pop();
      long double computedFactor = pow(candidateFactor, p.increments);
      currentCost -= state->costs[p.node][candidateParent] * (computedFactor - 1) / computedFactor;
      curCost *= pow(factor, p.increments);
      increments += p.increments;
    }
    currentCost += curCost;
    s.push({ increments, currentNode });
  }
  return currentCost;
}

Move miniMaxTraverser(ASPGameState *state, long double alpha, long double beta, int depth, long double currentCost, Timer *t, double deadline, bool guyuHeuristic, bool emilHeuristic) {
  vector<vector<int> > *graph = state->graph;
  int currentNode = state->currentNode;
  if (currentNode == state->destNode) {
    return {
      -1,
      -1,
      0
    };
  }
  Move bestMove = {
    -1,
    -1,
    state->INF
  };
  if (t->getTime() > deadline) {
    return {
      -1,
      -1,
      -10
    };
  }
  string stateString;
  if (depth < 0 && !guyuHeuristic) {
    // Only do this if we're not doing heuristics
    string stateString = getStateString(currentNode, false, state);
    if (mem.count(stateString)) {
      MemoEntry cached_entry = mem[stateString];
      if (!cached_entry.wasCut) {
        return cached_entry.move;
      } else {
        beta = min(beta, cached_entry.move.costRelatedInfo);
        bestMove = cached_entry.move;
      }
    }
  }

  // bubbleSort(state, currentNode); // This outperforms std::sort for our usecase
  insertionSort(state, currentNode, true);

  bool pruned = false;
  for (int i = 0; i < (*graph)[currentNode].size(); i++) {
    if (guyuHeuristic && i >= 2) {
      // Only check first 3 when doing heuristic
      break;
    }
    int neighbour = (*graph)[currentNode][i];
    ASPGameState stateCopy(*state);
    stateCopy.traverserMakeMove(neighbour);

    long double addedCost = state->costs[currentNode][neighbour];
    long double pathLength = miniMaxAdversary(&stateCopy, alpha, beta, depth - 1, currentCost + addedCost, t, deadline, guyuHeuristic, emilHeuristic).costRelatedInfo + addedCost;
    if (pathLength < bestMove.costRelatedInfo) {
      bestMove = {
        currentNode,
        neighbour,
        pathLength
      };
      beta = min(beta, pathLength + currentCost);
      if (beta <= alpha) {
        pruned = true;
        break;
      }
    }
  }
  if (depth < 0 && !guyuHeuristic) {
    if (t->getTime() > deadline) {
      pruned = true;
    }
    MemoEntry entry = {bestMove, pruned};
    mem[stateString] = entry;
  }
  return bestMove;
}

Move getMove(ASPGameState *state, int role, int type, Timer *t, double deadline) {
  cout << "Using type " << type << " with deadline " << (deadline - t->getTime()) << " seconds" << endl;
  Move bestMove;
  int i;

  switch (type) {
    case 0:
      return {
        state->currentNode,
        state->parentNodes[state->currentNode]
      };

    case 1:
      if (role == 0)
        return miniMaxTraverser(state, state->distances[state->currentNode], getBetaEstimate(state), -1, 0, t, deadline);
      else
        return miniMaxAdversary(state, state->distances[state->currentNode], getBetaEstimate(state), -1, 0, t, deadline);

    case 2:
      depthUsed = true;
      for (i = 1; t->getTime() < deadline && depthUsed; i++) {
        Move candidateMove;
        depthUsed = false;
        if (role == 0)
          candidateMove = miniMaxTraverser(state, state->distances[state->currentNode], getBetaEstimate(state), i, 0, t, deadline);
        else
          candidateMove = miniMaxAdversary(state, state->distances[state->currentNode], getBetaEstimate(state), i, 0, t, deadline);
        if (t->getTime() < deadline) {
          bestMove = candidateMove;
        }
      }
      cout << "Ended with i = " << i << endl;
      return bestMove;

    case 3:
      if (role == 0) {
        cerr << "Can't use type 3 with traverser atm" << endl;
        exit(1);
      } else {
        return smartGuyuAdversary(state);
      }

    case 4:
      if (role == 0) {
        return miniMaxTraverser(state, state->distances[state->currentNode], getBetaEstimate(state), -1, 0, t, deadline, true);
      } else {
        cerr << "Type 4 is not currently supported for adversaries" << endl;
        exit(1);
      }

    case 5:
      depthUsed = true;
      for (i = 1; t->getTime() < deadline && depthUsed; i++) {
        depthUsed = false;
        Move candidateMove;
        if (role == 0)
          candidateMove = miniMaxTraverser(state, state->distances[state->currentNode], getBetaEstimate(state), i, 0, t, deadline, false, true);
        else
          candidateMove = miniMaxAdversary(state, state->distances[state->currentNode], getBetaEstimate(state), i, 0, t, deadline, false, true);
        if (t->getTime() < deadline) {
          bestMove = candidateMove;
        }
      }
      cout << "Ended with i = " << i << endl;
      return bestMove;
  }
}
