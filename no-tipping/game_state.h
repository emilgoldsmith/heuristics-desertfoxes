// header guard
#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <vector>
#include <utility>

struct Move {
  int weight;
  int position;
};

class GameState {
  int board[61];
  int totalWeights;
  int player1;
  int player2;
  bool player1Plays;
  bool addingPhase;
  int winner;
  int torqueLeft;
  int torqueRight;

  bool validMove(Move move);
  std::pair<int, int> computeNewTorque(Move move, bool reverse = false);
  int getArrayIndex(int boardIndex);
  int getBoardIndex(int arrayIndex);

public:
  const int leftSupport = -3;
  const int rightSupport = -1;
  GameState(int numWeights);
  bool isPlayer1sTurn();
  bool isAddingPhase();
  int getWinner();
  std::vector<Move> getValidMoves();
  void makeMove(Move move);
  GameState(int xtotalWeights, std::vector<int> vectorBoard, int xplayer1, int xplayer2, bool xplayer1Plays, bool xaddingPhase, int xwinner, int xtorqueLeft, int xtorqueRight);
  GameState copy();
};

#endif
