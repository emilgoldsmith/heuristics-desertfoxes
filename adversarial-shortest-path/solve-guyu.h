#ifndef SOLVE_GUYU_H
#define SOLVE_GUYU_H

#include "move.h"
#include "asp_game_state.h"

Move guyuTraverser(ASPGameState *gs);
Move guyuAdversary(ASPGameState *gs);
Move smartGuyuTraverser(ASPGameState *gs);
Move smartGuyuAdversary(ASPGameState *gs, int depth);

#endif
