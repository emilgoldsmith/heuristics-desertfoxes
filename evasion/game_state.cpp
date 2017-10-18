#include "structs.h"
#include "game_state.h"

#include <vector>
#include <iostream>

using namespace std;

GameState::GameState(short int cooldownVariable, short int maxWallsVariable):
  cooldown(cooldownVariable),
  maxWalls(maxWallsVariable)
{}

void GameState::makeMove(HunterMove moveForHunter, Position moveForPrey) {
  // Simple move validations
  if ((moveForPrey.x != 2 || moveForPrey.y != 2) && !preyMoves) {
    cerr << "Tried to make move for prey while not allowed" << endl;
  }
  if (moveForHunter.wallType != 0 && cooldownTimer > 0) {
    cerr << "Attempted to create wall while on cooldown" << endl;
  }
  for (short int index : moveForHunter.indicesToDelete) {
    if (index >= walls.size() || index < 0) {
      cerr << "Attempted to remove wall with incorrect index" << endl;
    }
  }

  // Actual logic to come here through calling of more smaller functions

  // Update general game state variables
  preyMoves = !preyMoves;
  if (cooldownTimer > 0) cooldownTimer--;
}
