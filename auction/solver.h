#ifndef SOLVER_H
#define SOLVER_H

#include <vector>

struct Player {
  int moneyLeft;
  std::vector<int> paintings;
};

class Solver {
  public:
    int numArtists;
    int numToWin;
    int curRound = 0;
    std::vector<int> totalPaintings;
    std::vector<int> auctionRounds;
    std::vector<Player> standings;

    Solver(int artistNum, int winningNum, const std::vector<int> &itemsInAuction, int startingWealth, int numPlayers);
    void computeGameLength();
    void updateState(int winnerId, int winnerBid);
    int getBid();
    int recurse();
};

#endif
