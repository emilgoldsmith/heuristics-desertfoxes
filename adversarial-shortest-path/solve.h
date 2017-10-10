#ifndef SOLVE_H
#define SOLVE_H

#include "move.h"
#include "asp_game_state.h"
#include "../timer/timer.h"

Move getMove(ASPGameState *state, int role, int type, Timer *t, double deadline);

#endif
