#include "game_state.h"

#include <vector>
#include <utility>
#include <iostream>
#include <string>

using namespace std;

int GameState::getArrayIndex(int boardIndex) {
  return boardIndex + 30;
}

int GameState::getBoardIndex(int arrayIndex) {
  return arrayIndex - 30;
}

GameState::GameState(int numWeights) {
  totalWeights = numWeights;
  for (int i = 0; i < 61; i++) {
    board[i] = 0;
  }
  board[getArrayIndex(-4)] = 3;
  player1 = (1 << 30) - 1; player2 = (1 << 30) - 1;
  player1Plays = true;
  addingPhase = true;
  winner = -1;
  torqueLeft = ((-4) - leftSupport) * 3 + (0 - leftSupport) * 3;
  torqueRight = ((-4) - rightSupport) * 3 + (0 - rightSupport) * 3;
}

bool GameState::isPlayer1sTurn() {
  return player1Plays;
}

bool GameState::isAddingPhase() {
  return addingPhase;
}

int GameState::getWinner() {
  return winner;
}

pair<int, int> GameState::computeNewTorque(Move move, bool reverse) {
  int newLeftTorque, newRightTorque;
  if ((addingPhase && !reverse) || (!addingPhase && reverse)) {
    newLeftTorque = torqueLeft + (move.position - leftSupport) * move.weight;
    newRightTorque = torqueRight + (move.position - rightSupport) * move.weight;
  } else {
    newLeftTorque = torqueLeft - (move.position - leftSupport) * move.weight;
    newRightTorque = torqueRight - (move.position - rightSupport) * move.weight;
  }
  return make_pair(newLeftTorque, newRightTorque);
}

bool GameState::validMove(Move move) {
  pair<int, int> newTorque = computeNewTorque(move);
  return newTorque.first >= 0 && newTorque.second <= 0;
}

vector<Move> GameState::getValidMoves() {
  vector<Move> moves;
  Move losingMove = {-1, -1};
  int *player;
  if (player1Plays) {
    player = &player1;
  } else {
    player = &player2;
  }
  if (addingPhase) {
    for (int weight = 1; weight <= totalWeights; weight++) {
      if ((*player & (1 << weight)) == 0) {
        // The player doesn't have this weight
        continue;
      }
      for (int boardIndex = leftSupport; boardIndex <= rightSupport; boardIndex++) {
        int arrayIndex = getArrayIndex(boardIndex);
        if (board[arrayIndex] == 0) {
          // Board is empty here so we can indeed place it here
          Move candidateMove = {weight, boardIndex};
          if (validMove(candidateMove)) {
            moves.push_back(candidateMove);
          } else {
            cerr << "Expected weights in between supports to never tip the booard" << endl;
          }
        }
      }
      for (int boardIndex = leftSupport - 1; boardIndex >= -30; boardIndex--) {
        int arrayIndex = getArrayIndex(boardIndex);
        if (board[arrayIndex] == 0) {
          // Board is empty here so we can indeed place it here
          Move candidateMove = {weight, boardIndex};
          if (validMove(candidateMove)) {
            moves.push_back(candidateMove);
          } else {
            losingMove = candidateMove;
            break;
          }
        }
      }
      for (int boardIndex = rightSupport + 1; boardIndex <= 30; boardIndex++) {
        int arrayIndex = getArrayIndex(boardIndex);
        if (board[arrayIndex] == 0) {
          // Board is empty here so we can indeed place it here
          Move candidateMove = {weight, boardIndex};
          if (validMove(candidateMove)) {
            moves.push_back(candidateMove);
          } else {
            losingMove = candidateMove;
            break;
          }
        }
      }
    }
  } else {
    for (int boardIndex = -30; boardIndex <= 30; boardIndex++) {
      int arrayIndex = getArrayIndex(boardIndex);
      if (board[arrayIndex] == 0) {
        // There is nothing to remove
        continue;
      }
      Move candidateMove = {board[arrayIndex], boardIndex};
      if (validMove(candidateMove)) {
        moves.push_back(candidateMove);
      } else {
        losingMove = candidateMove;
      }
    }
  }
  if (moves.size() == 0) {
    moves.push_back(losingMove);
  }
  return moves;
}

void GameState::makeMove(Move move) {
  int *player;
  if (player1Plays) {
    player = &player1;
  } else {
    player = &player2;
  }

  int weight = move.weight;
  int arrayIndex = getArrayIndex(move.position);
  if (addingPhase) {
    if ((*player & (1 << weight)) == 0) {
      cerr << "EXCEPTION: ATTEMPTED TO MOVE WEIGHT ALREADY USED" << endl;
    }
    *player &= ~(1 << weight);
    board[arrayIndex] = weight;
  } else {
    if (board[arrayIndex] == 0) {
      cerr << "EXCEPTION: ATTEMPTED TO REMOVE WEIGHT THAT'S NOT THERE" << endl;
    }
    board[arrayIndex] = 0;
  }

  pair<int, int> newTorque = computeNewTorque(move);
  torqueLeft = newTorque.first;
  torqueRight = newTorque.second;

  if (addingPhase) {
    bool addingDone = true;
    for (int weight = 1; weight <= totalWeights; weight++) {
      if (player2 & (1 << weight)) {
        addingDone = false;
        break;
      }
    }
    if (addingDone) {
      addingPhase = false;
    }
  }
  player1Plays = !player1Plays;

  if (torqueLeft < 0 || torqueRight > 0) {
    // We have a winner
    if (player1Plays) {
      winner = 0;
    } else {
      winner = 1;
    }
  }
}

GameState::GameState(int xtotalWeights, vector<int> vectorBoard, int xplayer1, int xplayer2, bool xplayer1Plays, bool xaddingPhase, int xwinner, int xtorqueLeft, int xtorqueRight) {
  totalWeights = xtotalWeights;
  player1 = xplayer1;
  player2 = xplayer2;
  player1Plays = xplayer1Plays;
  addingPhase = xaddingPhase;
  winner = xwinner;
  torqueLeft = xtorqueLeft;
  torqueRight = xtorqueRight;
  for (int i = 0; i < 61; i++) {
    board[i] = vectorBoard[i];
  }
}

GameState GameState::copy() {
  vector<int> vectorBoard(61);
  for (int i = 0; i < 61; i++) {
    vectorBoard[i] = board[i];
  }
  return GameState(totalWeights, vectorBoard, player1, player2, player1Plays, addingPhase, winner, torqueLeft, torqueRight);
}

string GameState::extractFeatures() {
  string features = "";
  for (int i = 0; i < 61; i++) {
    features += to_string(board[i]) + " ";
  }

  for (int i = 1; i <= totalWeights; i++) {
    if (player1 & (1 << i)) {
      features += "0";
    } else {
      features += "1";
    }
    features += " ";
  }

  for (int i = 1; i <= totalWeights; i++) {
    if (player2 & (1 << i)) {
      features += "0";
    } else {
      features += "1";
    }
    features += " ";
  }

  if (addingPhase) {
    features += "1 0 ";
  } else {
    features += "0 1 ";
  }

  features += "1 0";
}
