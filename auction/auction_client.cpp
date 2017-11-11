#include "auction_client.h"

using namespace std;
using json = nlohmann::json;

AuctionClient::AuctionClient(std::string serverIP, int serverPort) {
  sock = new SocketClient(serverIP, serverPort);
}

json AuctionClient::receiveInit() {
  return sock->receiveJSON(BUFFER_SIZE);
}

json AuctionClient::receiveUpdate() {
  return sock->receiveJSON(BUFFER_SIZE);
}

void AuctionClient::makeBid(string bidItem, int bidAmount) {
  json j = {
    {"bid_item", bidItem},
    {"bid_amount", bidAmount}
  };
  sock->sendJSON(j);
}
