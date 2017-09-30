#include "game_state.h"

Move solve(GameState *state) {
  return state->getValidMoves()[0];
}
