#include "auction_client.h"
#include "solver.h"
#include <map>

using namespace std;
using json = nlohmann::json;


int main(int argc, char **argv) {
  if (argc != 3) {
    cout << "Usage: ./main.out <ip> <port>" << endl;
    exit(1);
  }
  // init client
  string ip = argv[1];
  int port = atoi(argv[2]);
  AuctionClient client(ip, port);

  // init solver
  json gameConfig = client.receiveInit();
  // map painting types to integers
  vector<int> itemsInAuction;
  for (string sType : gameConfig["auction_items"]) {
    itemsInAuction.push_back(sType.at(1) - '0');
  }
  // right now numPlayers is hardcoded
  Solver solver(gameConfig["artists_types"], gameConfig["required_count"], itemsInAuction, gameConfig["init_wealth"], 2);

  // init playerName map
  map<string, int> nameIndexMap;
  nameIndexMap[client.TEAM_NAME] = 0;
  int nextPlayerIndex = 1;

  // main loop
  int currentRound = 0;
  while (true) {
    int bidAmount = solver.getBid();
    string bidItem = gameConfig["auction_items"][currentRound];
    client.makeBid(bidItem, bidAmount);

    json update = client.receiveUpdate();
    // someone won the bid
    if (update["bid_winner"]) {
      cout << "Bid winner: " << update["bid_winner"] << " Bid amount: " << update["winning_bid"] << endl;
      // create new name-index mapping for bid winner if necessary
      if (nameIndexMap.find(update["bid_winner"]) == nameIndexMap.end()) {
        nameIndexMap["bid_winner"] = nextPlayerIndex;
        nextPlayerIndex++;
      }
      solver.updateState(nameIndexMap[update["bidwinner"]], update["winning_bid"]);
    }
    // game over
    if (update["finished"]) {
      cout << "Game over: " << update["reason"] << endl;
      break;
    }

    currentRound++;
  }
}
