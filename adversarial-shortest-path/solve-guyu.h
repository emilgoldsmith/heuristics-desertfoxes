#ifndef SOLVE_GUYU_H
#define SOLVE_GUYU_H

#include "move.h"
#include "asp_game_state.h"

// Move guyuTraverser(ASPGameState *gs);
// Move guyuAdversary(ASPGameState *gs);
// Move smartGuyuTraverser(ASPGameState *gs);
// Move mehGuyuAdversary(ASPGameState *gs, int depth);
Move smartGuyuAdversary(ASPGameState *gs);

#endif
