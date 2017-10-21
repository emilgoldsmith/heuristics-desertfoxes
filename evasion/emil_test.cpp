#include "game_state.h"
#include "evasion_solve.h"
#include "constants.h"

using namespace std;

int main() {
  GameState g(1, 2);
  miniMax(&g, INF);
}
