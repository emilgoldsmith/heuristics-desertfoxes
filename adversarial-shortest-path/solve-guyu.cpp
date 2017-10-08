#include "solve-guyu.h"
#include "asp_game_state.h"
#include "move.h"

#include <utility>
#include <vector>
#include <algorithm>

using namespace std;

Move guyuTraverser(ASPGameState *gs) {
  return { gs->currentNode, gs->parentNodes[gs->currentNode] };
}

Move guyuAdversary(ASPGameState *gs) {
  return guyuTraverser(gs);
}
