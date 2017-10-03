#ifndef SOLVE
#define SOLVE

#include "game_state.h"
#include "../timer/timer.h"

struct ScoredMove {
  Move move;
  int score;
};

class NoTippingSolve {
  const int POSITIVE_INF = 99999999;
  const int NEGATIVE_INF = -99999999;

  // 1 - add highest weight and distance
  // 2 - add in a way that limits the opponent's choices the most
  // 3 - add randomly
  int addStrategy;

  // 1 - remove in a way that limits the opponent's chocies the most
  // 2 - remove randomly
  int removeStrategy;

  // true indicates last year's algorithm
  // search till the end when there is less than n weights left
  // n to be determined by a calibration method based on computer power
  bool exhaustiveSearch;
  int n;

  // regular minimax has no depth limit, just uses iterative deepening
  Timer t = Timer();

  // naive evaluation, 1 for victory, -1 for loss, 0 for nothing
  // to be used with exhaustive search
  int naiveEval(GameState &gs);
  // evaluation based on number of options the next player has
  // to be used with non-exhaustive search
  int optionEval(GameState &gs);

  // generate a uniform int
  int getUniform(int from, int to);
  // return a move that limits the next player's options
  Move getOptionLimitingMove(GameState &gs);
  // get a preliminary move during adding phase
  Move getAddMove(GameState &gs);
  // get a preliminary move during removal phase
  Move getRemoveMove(GameState &gs);

  // here, we always define the first player to be the maximizing player
  ScoredMove minimax(GameState &gs, int depth, bool isMax, int alpha, int beta, double timeLimit);
public:
  NoTippingSolve(int addStrat, int removeStrat, bool exhaustive, int exhaustiveN);
  // calibrate n for exhaustive search
  Move getMove(GameState &gs);
};

#endif
