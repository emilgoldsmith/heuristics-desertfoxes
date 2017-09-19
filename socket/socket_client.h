// header guard
#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <string>
#include "json.hpp"
using json = nlohmann::json;
using string = std::string;

class SocketClient {
  string sockAddress;
  int sockPort;
  int sockFD;

public:
  SocketClient(string socketAddress, int socketPort);
  int sendAll(char *buf, int len);
  string receive(int bufferSize);
  int sendJSON(json j);
  json receiveJSON(int bufferSize);
};

#endif
