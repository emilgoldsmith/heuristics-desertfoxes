#ifndef SOLVE_H
#define SOLVE_H

#include "move.h"
#include "asp_game_state.h"

#include <vector>

Move miniMaxAdversary(ASPGameState *state);
Move miniMaxTraverser(ASPGameState *state);
Move getTraverseMove(ASPGameState *state);
Move getAdversaryMove(ASPGameState *state);

#endif
