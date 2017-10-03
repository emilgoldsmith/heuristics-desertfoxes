#include "solve.h"
#include <iostream>
#include <random>

using namespace std;

NoTippingSolve::NoTippingSolve(int addStrat, int removeStrat, bool exhaustive, int exhaustiveN, double maxDeadline) {
  addStrategy = addStrat;
  removeStrategy = removeStrat;
  exhaustiveSearch = exhaustive;
  n = exhaustiveN;
  dead = maxDeadline;
}

int NoTippingSolve::naiveEval(GameState &gs) {
  if (gs.getWinner() == 0) {
    return POSITIVE_INF;
  }
  if (gs.getWinner() == 1) {
    return NEGATIVE_INF;
  }
  return 0;
}

int NoTippingSolve::optionEval(GameState &gs) {
  if (gs.getWinner() == 0) {
    return POSITIVE_INF;
  }
  if (gs.getWinner() == 1) {
    return NEGATIVE_INF;
  }
  // not terminal node
  // since first player is the maximizing player, if it's the first player's turn
  // higher values are favored, so the positive size is returned
  if (gs.isPlayer1sTurn()) {
    return gs.getValidMoves().size();
  }
  // similarly, second player is minimizing player and favors lower values, so negative size is returned
  return -1 * gs.getValidMoves().size();
}

int NoTippingSolve::getUniform(int from, int to) {
  random_device randDev;
  mt19937 generator(randDev());
  uniform_int_distribution<int> distribution(from, to);
  return distribution(generator);
}

Move NoTippingSolve::getOptionLimitingMove(GameState &gs) {
  vector<Move> nextMoves = gs.getValidMoves();
  int numOptions;
  int minNumOptions = POSITIVE_INF;
  Move desiredMove;
  for (auto &move : nextMoves) {
    GameState newState = gs.copy();
    newState.makeMove(move);
    numOptions = newState.getValidMoves().size();
    if (numOptions < minNumOptions) {
      minNumOptions = numOptions;
      desiredMove = move;
    }
  }
  return desiredMove;
}

Move NoTippingSolve::getAddMove(GameState &gs) {
  if (addStrategy == 1) {
    return gs.getValidMoves()[0];
  }
  if (addStrategy == 2) {
    return getOptionLimitingMove(gs);
  }
  if (addStrategy == 3) {
    vector<Move> nextMoves = gs.getValidMoves();
    return nextMoves[getUniform(0, nextMoves.size() - 1)];
  }
  cout << "unknown adding strategy #: " << addStrategy << endl;
  exit(1);
}

Move NoTippingSolve::getRemoveMove(GameState &gs) {
  if (removeStrategy == 1) {
    return getOptionLimitingMove(gs);
  }
  if (removeStrategy == 2) {
    vector<Move> nextMoves = gs.getValidMoves();
    return nextMoves[getUniform(0, nextMoves.size() - 1)];
  }
  cout << "unknown removal strategy #: " << removeStrategy << endl;
  exit(1);
}

ScoredMove NoTippingSolve::minimax(GameState &gs, int depth, bool isMax, int alpha, int beta, double deadline) {
  // if time ran out
  if (deadline > 0 && t.getTime() >= deadline) {
    return {{-100, -100}, -999};
  }

  // condition for normal termination
  if (depth == 0 || gs.getWinner() != -1) {
    // use naive evaluation
    if (exhaustiveSearch) {
      return {{-100, -100}, naiveEval(gs)};
    }
    // use option-limiting heuristics
    return {{-100, -100 }, optionEval(gs)};
  }

  // search one level deeper
  int bestScore = -999;
  Move bestMove = getRemoveMove(gs);
  vector<Move> nextMoves = gs.getValidMoves();
  // cout << "expansion factor: " << nextMoves.size() << endl;

  if (isMax) {
    bestScore = NEGATIVE_INF;
    for (auto &move : nextMoves) {
      // if time ran out
      if (deadline > 0 && t.getTime() >= deadline) {
        break;
      }
      GameState newState = gs.copy();
      newState.makeMove(move);
      ScoredMove sm = minimax(newState, depth-1, !isMax, alpha, beta, deadline);
      if (sm.score > bestScore) {
        bestScore = sm.score;
        bestMove = {move.weight, move.position};
      }
      if (alpha < bestScore) {
        alpha = bestScore;
      }
      if (beta <= alpha) {
        break;
      }
    }
    return {bestMove, bestScore};
  }

  // min player
  bestScore = POSITIVE_INF;
  for (auto &move : nextMoves) {
    // if time ran out
    if (deadline > 0 && t.getTime() >= deadline) {
      break;
    }
    GameState newState = gs.copy();
    newState.makeMove(move);
    ScoredMove sm = minimax(newState, depth-1, !isMax, alpha, beta, deadline);
    if (sm.score < bestScore) {
      bestScore = sm.score;
      bestMove = {move.weight, move.position};
    }
    if (beta > bestScore) {
      beta = bestScore;
    }
    if (beta <= alpha) {
      break;
    }
  }
  return {bestMove, bestScore};
}

Move NoTippingSolve::getMove(GameState &gs) {
  t.start();
  Move desiredMove;

  // add phase
  if (gs.isAddingPhase()) {
    desiredMove = getAddMove(gs);
    t.pause();
    return desiredMove;
  }

  // removal phase - exhaustive search
  if (exhaustiveSearch) {
    if (gs.numWeightsLeft() > n) {
      desiredMove = getRemoveMove(gs);
      t.pause();
      return desiredMove;
    }
    // exhaustive search possible, <= n weights left
    cout << "(Less than " << n << " weights left)" << endl;
    double timeLimit = min(dead, t.timeLeft() / 2);
    double deadline = t.getTime() + timeLimit;
    ScoredMove sm = minimax(gs, n + 1, gs.isPlayer1sTurn(), NEGATIVE_INF, POSITIVE_INF, deadline);
    cout << "Score: " << sm.score << endl;
    desiredMove = sm.move;
    t.pause();
    return desiredMove;
  }

  // removal phase - regular minimax, start at search depth 2
  // allot more time for early searches, 8 is "arbitary"
  double timeLimit = t.timeLeft() / 8;
  double deadline = t.getTime() + timeLimit;
  cout << "Alloted time for this level: " << timeLimit << endl;
  desiredMove = getRemoveMove(gs);
  Move desiredMoveBackup;
  for (int depth = 2; depth <= gs.numWeightsLeft() + 1; depth++) {
    cout << "iteratively exploring depth " << depth << endl;
    ScoredMove sm = minimax(gs, depth, gs.isPlayer1sTurn(), NEGATIVE_INF, POSITIVE_INF, deadline);
    cout << "Score: " << sm.score << endl;
    desiredMoveBackup = desiredMove;
    desiredMove = sm.move;
    if (t.getTime() >= deadline) {
      cout << "Time ran out " << endl;
      break;
    }
  }
  // fix premature minimax termination
  desiredMove = desiredMoveBackup;

  t.pause();
  return desiredMove;
}
