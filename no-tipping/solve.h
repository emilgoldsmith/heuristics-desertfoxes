#ifndef SOLVE
#define SOLVE

#include "game_state.h"

Move solveNaive(GameState *state);
Move solveMinimax(GameState *state, int maxDepth);
Move solveMinimaxRemoveFocus(GameState *state, int maxDepth);

#endif
