#include "nim_client.h"

NimClient::NimClient(bool goesFirst, string socketAddress, int socketPort) {
  sc = new SocketClient(socketAddress, socketPort);
  order = goesFirst ? 0 : 1;

  // send client info
  json clientInfo = {
    {"name", name},
    {"order", order}
  };
  sc->sendJSON(clientInfo);

  // receive game info
  json info = receiveMove();
  initNumStones = int(info["init_stones"]);
  initNumResets = int(info["init_resets"]);
}

json NimClient::makeMove(int takeHowMany, bool useReset) {
  json moveJSON = {
    {"order", order},
    {"num_stones", takeHowMany},
    {"reset", useReset? 1: 0}
  };
  sc->sendJSON(moveJSON);
  // after making a move, immediate receive the server's response
  return receiveMove();
}

json NimClient::receiveMove() {
  return sc->receiveJSON(DATA_SIZE);
}
