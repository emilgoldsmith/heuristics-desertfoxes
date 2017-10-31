#ifndef DANCING_GAME_STATE
#define DANCING_GAME_STATE

#include "structs.h"
#include "../timer/timer.h"

#include <iostream>
#include <string>
#include <vector>
#ifdef DEBUG
#include <map>
#endif

class GameState {
public:
  int boardSize;
  int numColors;
  std::vector<Dancer> dancers;
  std::vector<Point> stars;
  int **board; // let row be y, col be x by convention, and -1 star, 0 empty, 1-c dancers (color is 1-indexed in game state but 0-indexed in client)

  bool randomize = false;
  Timer *t;
  std::vector<int> sortedDancerIndices;
  int numSimulations; // number of simulation tried
  ChoreographerMove currentBestSequence; // best sequence simulated so far
#ifdef DEBUG
  std::map<std::string, int> bestMovePerStrategy;
  void printStrategyStats();
#endif

  GameState(int boardSize, int numColors, std::vector<Dancer> dancers, std::vector<Point> stars, Timer *xt);
  ~GameState(); // free the board array

  void resetBoard();
  void fillBoard(std::vector<Dancer> &dancerList, std::vector<Point> &starList);
  std::vector<Dancer> cloneDancers();
  bool isConsistent(); // check if dancers, stars, and board are consistent
  void display();
  bool withinBounds(Point position);
  bool simulateOneMove(std::vector<Point> &nextPositions);
  bool atFinalPositions(std::vector<Point> &finalPositions);
  std::vector<Point> getViableNextPositions(Point dancerPosition);
  std::vector<Point> getViableNextPositions(Dancer &dancer);
  Point searchBestNext(Dancer &dancer, Point &finalPosition, std::vector<Point> &initViableNexts);
  void simulate(SolutionSpec &input, std::string strategy = "Unknown"); // returns a sequence of moves
};

#endif
