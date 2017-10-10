// header guard
#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <string>
#include "json.hpp"

class SocketClient {
  std::string sockAddress;
  int sockPort;
  int sockFD;

public:
  SocketClient(std::string socketAddress, int socketPort);
  ~SocketClient();
  int sendAll(char *buf, int len);
  std::string receive(int bufferSize);
  int sendJSON(nlohmann::json j);
  nlohmann::json receiveJSON(int bufferSize);
  int sendString(std::string s);
  void closeSocket();

  // specific receives
  nlohmann::json receiveASP(int bufferSize);
};

#endif
