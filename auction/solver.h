#ifndef SOLVER_H
#define SOLVER_H

#include "../timer/timer.h"

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
    int lastRound;
    std::vector<int> totalPaintings;
    std::vector<std::vector<int>> totalPaintingsUntil;
    std::vector<int> auctionRounds;
    std::vector<Player> standings;

    double start;
    Timer *t;

    Solver(int artistNum, int winningNum, const std::vector<int> &itemsInAuction, int startingWealth, int numPlayers);
    ~Solver();
    void computeGameLength();
    void updateState(int winnerId, int winnerBid);
    int getBid();
    int recurse(int rd, int adversary, std::vector<Player> curStandings);
};

#endif
