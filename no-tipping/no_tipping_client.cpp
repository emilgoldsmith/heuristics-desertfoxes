#include "no_tipping_client.h"

#include <sstream>
#include <iostream>

int *NoTippingClient::splitStringBySpace(string s) {
  std::istringstream iss(s, std::istringstream::in);
  int *stateArr = new int[LENGTH];
  string word;
  int i = 0;
  while (iss >> word) {
    if (i >= LENGTH) {
      std::cerr << "Input string too long: " << s << std::endl;
      exit(1);
    }
    stateArr[i] = stoi(word);
    i++;
  }
  return stateArr;
}

void NoTippingClient::checkConsistency() {
  bool isAdding = currentStateArr[0] == 0 ? true : false;
  if (state->isAddingPhase() != isAdding) {
    std::cerr << "Adding phase inconsistency" << std::endl;
    exit(1);
  }
  bool gameOver = currentStateArr[LENGTH - 1] == 1 ? true : false;
  bool stateGameover = state->getWinner() != -1;
  if (gameOver != stateGameover) {
    std::cerr << "Game over inconsistency" << std::endl;
    exit(1);
  }
}

void NoTippingClient::printCSA() {
  for (int i = 0; i < LENGTH; i++) {
    std::cout << currentStateArr[i] << " ";
  }
  std::cout << std::endl;
}

NoTippingClient::NoTippingClient(bool goesFirst, string socketAddress, int socketPort) {
  sc = new SocketClient(socketAddress, socketPort);
  first = goesFirst ? 1 : 0;

  string initContact = name + " " + std::to_string(first);
  sc->sendString(initContact);

  int numWeights = stoi(sc->receive(DATA_SIZE));
  state = new GameState(numWeights);

  for (int i = 0; i < 63; i++) {
    currentStateArr[i] = 0;
  }
  currentStateArr[-4 + 30 + 1] = 3;
}

void NoTippingClient::receiveMove() {
  std::cout << "Receiving move..." << std::endl;
  // always check consistency first
  checkConsistency();

  // receive and split data
  string newStateStr = sc->receive(DATA_SIZE);
  int *newStateArr = splitStringBySpace(newStateStr);

  std::cout << newStateStr << std::endl;

  // get new move by comparing stateArr and currentStateArr
  int weight = -1;
  int position = -1;
  for (int i = 1; i < LENGTH - 1; i++) {
    if (newStateArr[i] != currentStateArr[i]) {
      if (state->isAddingPhase()) {
        weight = newStateArr[i];
        position = i - 1;
        break;
      } else {
        weight = 0;
        position = i - 1;
        break;
      }
    }
  }

  // no new move, just return
  if (weight == -1 || position == -1) {
    return;
  }

  // update game state
  Move m = { weight, position - 30 };
  std::cout << "Opponent is placing weight " << m.weight << " at position " << m.position << std::endl;
  state->makeMove(m);
  for (int i = 0; i < LENGTH; i++) {
    currentStateArr[i] = newStateArr[i];
  }
  delete [] newStateArr;

  // check consistency again to be safe
  checkConsistency();
  std::cout << "Move OK" << std::endl;
}

void NoTippingClient::makeMove(int weight, int position) {
  std::cout << "Making move..." << std::endl;
  checkConsistency();
  bool isAdding = state->isAddingPhase();

  // make move locally
  Move m = { weight, position };
  std::cout << "I am placing weight " << m.weight << " at pos " << m.position << std::endl;
  state->makeMove(m);
  if (isAdding) {
    currentStateArr[position + 30 + 1] = weight;
  } else {
    currentStateArr[position + 30 + 1] = 0;
  }
  printCSA();

  // send move to server
  string moveString;
  if (isAdding) {
    moveString = std::to_string(weight) + " " + std::to_string(position);
  } else {
    moveString = std::to_string(position);
  }
  sc->sendString(moveString);
  std::cout << "Move OK" << std::endl;
}
