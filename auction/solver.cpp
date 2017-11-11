#include "solver.h"

#include <vector>
#include <chrono>
#include <thread>

using namespace std;

Solver::Solver(int artistNum, int winningNum, const vector<int> &itemsInAuction, int startingWealth, int numPlayers):
  numArtists(artistNum),
  numToWin(winningNum),
  auctionRounds(itemsInAuction)
{
  // We are always player id 0
  standings.insert(standings.begin(), numPlayers, {startingWealth, vector<int>(artistNum, 0)});
  computeGameLength();
}

void Solver::computeGameLength() {
  totalPaintings.insert(totalPaintings.begin(), numArtists, 0);
  int high = 0;
  const int pidgeonHoleLimit = (numToWin - 1) * standings.size() + 1;
  for (int i = 0; i < auctionRounds.size() && high < pidgeonHoleLimit; i++) {
    totalPaintings[auctionRounds[i]]++;
    high = max(high, totalPaintings[auctionRounds[i]]);
  }
}

void Solver::updateState(int winnerId, int winnerBid) {
  standings[winnerId].moneyLeft -= winnerBid;
  int curItem = auctionRounds[curRound];
  standings[winnerId].paintings[curItem]++;
  curRound++;
}

int Solver::getBid() {
  int curItem = auctionRounds[curRound];
  int money = standings[0].moneyLeft;

  if (standings[0].paintings[curItem] == numToWin - 1) {
    return money;
  }

  if (totalPaintings[curItem] < numToWin) {
    return 0;
  }

  // We could also do a check here maybe that checks if anyone can win based on current distribution of paintings

  // Maybe do this for each player and pick max bid
  int candidateBid = recurse();

  int biggestThreat = -1;
  for (int otherPlayerId = 1; otherPlayerId < standings.size(); otherPlayerId++) {
    if (standings[otherPlayerId].paintings[curItem] == numToWin - 1) {
      max(biggestThreat, standings[otherPlayerId].moneyLeft);
    }
  }
  if (biggestThreat == -1) return candidateBid;
  // Someone else can win
  if (candidateBid > biggestThreat + 1) {
    // We really want this one, not just outbid
    return candidateBid;
  } else {
    // We hope someone else outbids, so this is just our backup
    // Do some time management on this
    this_thread::sleep_for(chrono::seconds(5));
    return biggestThreat + 1;
  }
}

int Solver::recurse() {
  return 5;
}
