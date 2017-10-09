#ifndef SOLVE_H
#define SOLVE_H

#include "move.h"
#include "asp_game_state.h"

#include <vector>
#include <bitset>
#include <utility>
#include <set>

typedef std::pair<int, int> piild;
typedef std::set<piild> customSet;

Move getTraverseMove(ASPGameState *state, int minimax);
Move getAdversaryMove(ASPGameState *state, int minimax);

#endif
