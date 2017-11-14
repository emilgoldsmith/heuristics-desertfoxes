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
  client.sendTeamName();

  // init solver
  json gameConfig = client.receiveInit();
  // map painting types to integers
  vector<int> itemsInAuction;
  for (string sType : gameConfig["auction_items"]) {
    itemsInAuction.push_back(sType.at(1) - '0');
  }
  int requiredCount = ((int) gameConfig["required_count"]) + 1;
  Solver solver(gameConfig["artists_types"], requiredCount, itemsInAuction, gameConfig["init_wealth"], gameConfig["player_count"]);

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
    cout << "Bidding " << bidAmount << " on " << bidItem << endl;

    json update = client.receiveUpdate();
    cout << update.dump() << endl;
    string bidWinner = update.value("bid_winner", "");
    int winningBid = update.value("winning_bid", 0);

    cout << "Bid winner: " << bidWinner << " Bid amount: " << winningBid << endl;
    // create new name-index mapping for bid winner if necessary
    if (nameIndexMap.find(bidWinner) == nameIndexMap.end()) {
      nameIndexMap["bid_winner"] = nextPlayerIndex;
      nextPlayerIndex++;
    }
    solver.updateState(nameIndexMap[bidWinner], winningBid);

    // check if game over
    if (update["finished"]) {
      cout << "Game over: " << update["reason"] << endl;
      break;
    }

    currentRound++;
  }
}
