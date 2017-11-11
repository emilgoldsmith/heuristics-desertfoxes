#ifndef SOLVER_H
#define SOLVER_H

#include <vector>

struct Player {
  int moneyLeft;
  std::vector<int> paintings;
};

class Solver {
  public:
    int numArtist;
    int numToWin;
    int curRound = 0;
    vector<int> totalPaintings;
    vector<int> auctionRounds;
    std::vector<Player> standings;

    Solver(int artistNum, int winningNum, const std::vector<int> &itemsInAuction, int startingWealth, int numPlayers);
    void computeGameLength();
    void updateState(int winnerId, int winnerBid);
    int getBid();
    int recurse();
};

#endif
