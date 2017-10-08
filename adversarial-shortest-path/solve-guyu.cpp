#include "solve-guyu.h"
#include "asp_game_state.h"
#include "move.h"

#include <utility>
#include <vector>
#include <algorithm>

using namespace std;

Move guyuTraverser(ASPGameState *gs) {
  int minDistNode = -1;
  long double minDist = gs->INF;
  for (int neighbor : (*gs->graph)[gs->currentNode]) {
    if (gs->distances[neighbor] < minDist) {
      minDistNode = neighbor;
      minDist = gs->distances[neighbor];
    }
  }
  return { gs->currentNode, minDistNode };
}

Move guyuAdversary(ASPGameState *gs) {
  return guyuTraverser(gs);
}
