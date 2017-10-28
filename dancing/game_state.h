#ifndef DANCING_GAME_STATE
#define DANCING_GAME_STATE

#include "structs.h"

#include <iostream>
#include <string>
#include <vector>

class GameState {
public:
  int boardSize;
  int numColors;
  std::vector<Dancer> dancers;
  std::vector<Point> stars;
  int **board; // let row be x, col be y by convention, and -1 star, 0 empty, 1-c dancers (color is 1-indexed in game state but 0-indexed in client)

  int numSimulations; // number of simulation tried
  std::vector<ChoreographerMove> currentBestSequence; // best sequence simulated so far

  GameState(int boardSize, int numColors, std::vector<Dancer> dancers, std::vector<Point> stars);
  ~GameState(); // free the board array
  bool isConsistent(); // check if dancers, stars, and board are consistent
  void display();
  std::vector<ChoreographerMove> simulate(std::vector<Point> finalPositions); // returns a sequence of moves
};

#endif
