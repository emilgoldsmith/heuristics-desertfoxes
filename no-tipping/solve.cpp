#include "game_state.h"
#include "../timer/timer.h"
#include <iostream>

using namespace std;

const int POS_INF = 999999;
const int NEG_INF = -999999;

struct scoredMove {
  Move move;
  int score;
};

Move solveNaive(GameState *state) {
  return state->getValidMoves()[0];
}

// first player is maximizing player
int evaluateNaive(GameState *state) {
  if (state->getWinner() == -1) {
    return 0;
  // first player wins
  } else if (state->getWinner() == 0) {
    return POS_INF;
  // secomd player wins
  } else {
    return NEG_INF;
  }
}

int evaluateOptions(GameState *state) {
  if (state->getWinner() == -1) {
    return state->getValidMoves().size();
  // first player wins
  } else if (state->getWinner() == 0) {
    return POS_INF;
  // secomd player wins
  } else {
    return NEG_INF;
  }
}

// here, the max player is just the first player
scoredMove minimaxHelper(GameState *state, int depth, int maxDepth, bool isMaxPlayer, int alpha, int beta) {
  vector<Move> nextMoves = state->getValidMoves();
  // cout << "Depth: " << depth << " Branching factor: " << nextMoves.size() << endl;
  if (depth >= maxDepth || state->getWinner() != -1) {
    return { {-100, -100}, evaluateNaive(state) };
  }

  int bestScore;
  if (isMaxPlayer) {
    Move bestMove = {-100, -100};
    bestScore = NEG_INF;
    for (auto &move : nextMoves) {
      GameState stateCopy = state->copy();
      stateCopy.makeMove(move);
      scoredMove sm = minimaxHelper(&stateCopy, depth + 1, maxDepth, !isMaxPlayer, alpha, beta);
      if (sm.score > bestScore) {
        bestScore = sm.score;
        bestMove = {move.weight, move.position};
      }
      if (alpha < bestScore) { // alpha is minimum score guaranteed on a max node
        alpha = bestScore;
      }
      if (beta <= alpha) {
        break;
      }
    }
    return {bestMove, bestScore};
  } else {
    Move bestMove = {-100, -100};
    bestScore = POS_INF;
    for (auto &move : nextMoves) {
      GameState stateCopy = state->copy();
      stateCopy.makeMove(move);
      scoredMove sm = minimaxHelper(&stateCopy, depth + 1, maxDepth, !isMaxPlayer, alpha, beta);
      if (sm.score < bestScore) {
        bestScore = sm.score;
        bestMove = {move.weight, move.position};
      }
      if (beta > bestScore) { // beta is maximum score guaranteed on a min node
        beta = bestScore;
      }
      if (beta <= alpha) {
        break;
      }
    }
    return {bestMove, bestScore};
  }
}

Move solveMinimax(GameState *state, int maxDepth) {
  scoredMove bestScoredMove = minimaxHelper(state, 0, maxDepth, state->isPlayer1sTurn(), NEG_INF, POS_INF);
  cout << bestScoredMove.score << " ";
  return bestScoredMove.move;
}

Move solveMinimaxTimed(GameState *state, int timeLimit) {
  Timer timer;
  double lastTime = 0;
  scoredMove bestScoredMove;
  for (int depth = 1; depth < 40; depth++) {
    timer.start();
    // do more stuff later
  }
}

Move solveMinimaxRemoveFocus(GameState *state, int maxDepth) {
  if (state->isAddingPhase()) {
    return state->getValidMoves()[0];
  }
  // removing phase
  scoredMove bestScoredMove = minimaxHelper(state, 0, maxDepth, state->isPlayer1sTurn(), NEG_INF, POS_INF);
  cout << bestScoredMove.score << " ";
  return bestScoredMove.move;
}
