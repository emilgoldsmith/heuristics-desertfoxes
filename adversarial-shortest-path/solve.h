#ifndef SOLVE_H
#define SOLVE_H

#include "move.h"
#include "asp_game_state.h"

#include <vector>
#include <bitset>

Move miniMaxAdversary(ASPGameState *state, std::bitset<1010> visited, long double alpha, long double beta);
Move miniMaxTraverser(ASPGameState *state, std::bitset<1010> visited, long double alpha, long double beta);
Move getTraverseMove(ASPGameState *state);
Move getAdversaryMove(ASPGameState *state);

#endif
