// header guard
#ifndef GAME_STATE_H
#define GAME_STATE_H

struct Move {
  int weight;
  int position;
};

class GameState {
  int board[65];
  int player1;
  int player2;
  bool player1Plays;
  bool addingPhase;
  int winner;
  int torqueLeft;
  int torqueRight;

  bool validMove(Move move);

public:
  const int leftSupport = -3;
  const int rightSupport = -1;
  GameState();
  bool isPlayer1sTurn();
  bool isAddingPhase();
  int getWinner();
  vector<Move> getValidMoves();
  void makeMove(Move move);
  GameState makeMoveCopy(Move move);
}

#endif
