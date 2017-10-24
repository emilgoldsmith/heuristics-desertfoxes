#ifndef EVASION_SOLVE_H
#define EVASION_SOLVE_H

#include "game_state.h"
#include "structs.h"

Position solvePreyRandom(GameState *state);
HunterMove solveHunterRandom(GameState *state);
Position solvePreyHeuristic(GameState *state);
HunterMove solveHunterHeuristic(GameState *state);
HunterMove solveHunterGuyu(GameState *state);
Score miniMax(GameState *state, int currentBest);

#endif
