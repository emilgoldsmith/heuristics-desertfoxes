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
    std::vector<Player> standings;

    Solver(int artistNum, int winningNum, std::vector<int> itemsInAuction, int startingWealth);

    void updateState(int winnerId, int winnerBid);
    int getBid();
};

#endif
