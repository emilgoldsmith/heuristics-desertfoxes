#ifndef AUCTION_CLIENT_H
#define AUCTION_CLIENT_H

#include "../socket/socket_client.h"
#include <string>

class AuctionClient {
public:
  const std::string TEAM_NAME = "DesertFoxes";
  const int BUFFER_SIZE = 8192;
  SocketClient *sock;

  AuctionClient(std::string serverIP, int serverPort);
  nlohmann::json receiveInit();
  nlohmann::json receiveUpdate();
  void makeBid(int bidItem, int bidAmount);
};

#endif
