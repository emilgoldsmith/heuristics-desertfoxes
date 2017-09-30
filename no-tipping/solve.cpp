#include "game_state.h"
#include <iostream>

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
  } else if (state->getWinner() == 0) {
    return 1;
  } else {
    return -1;
  }
}

// here, the max player is just the first player
scoredMove minimaxHelper(GameState *state, int depth, int maxDepth, bool isMaxPlayer, int alpha, int beta) {
  std::vector<Move> nextMoves = state->getValidMoves();
  std::cout << "Depth: " << depth << " Branching factor: " << nextMoves.size() << std::endl;
  if (depth >= maxDepth || nextMoves.size() == 0) {
    return { {-1, -1}, evaluateNaive(state) };
  }

  if (isMaxPlayer) {
    Move bestMove = {-1, -1};
    int bestScore = -9999999;
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
    Move bestMove = {-1, -1};
    int bestScore = 9999999;
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
  scoredMove bestScoredMove = minimaxHelper(state, 0, maxDepth, state->isPlayer1sTurn(), -9999999, 9999999);
  return bestScoredMove.move;
}
