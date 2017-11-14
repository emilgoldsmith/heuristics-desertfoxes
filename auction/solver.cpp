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
  totalPaintingsUntil.insert(totalPaintingsUntil.begin(), numArtists, vector<int>());
  int high = 0;
  const int pidgeonHoleLimit = (numToWin - 1) * standings.size() + 1;
  for (int i = 0; i < auctionRounds.size() && high < pidgeonHoleLimit; i++) {
    totalPaintings[auctionRounds[i]]++;
    for (int j = 0; j < numArtists; j++) {
      totalPaintingsUntil[j].push_back(totalPaintings[j]);
    }
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

  int maxPaintings = standings[0].paintings[curItem];
  for (int i = 1; i < standings.size(); i++) {
    maxPaintings = max(standings[i].paintings[curItem], maxPaintings);
  }
  // No one can win on this painting
  if ((numToWin - maxPaintings) > (totalPaintingsUntil[curItem].back() - totalPaintingsUntil[curItem][curRound] + 1)) {
    return 0;
  }

  int candidateBid = recurse(curRound, 1, standings);
  for (int i = 2; i < standings.size(); i++) {
    candidateBid = max(candidateBid, recurse(curRound, i, standings));
  }

  int biggestThreat = -1;
  for (int otherPlayerId = 1; otherPlayerId < standings.size(); otherPlayerId++) {
    if (standings[otherPlayerId].paintings[curItem] == numToWin - 1) {
      biggestThreat = max(biggestThreat, standings[otherPlayerId].moneyLeft);
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
    if (money < biggestThreat + 1) {
      // We don't have enough money to outbid
      return candidateBid;
    }
    this_thread::sleep_for(chrono::seconds(5));
    return biggestThreat + 1;
  }
}

int Solver::recurse(int rd, int adversary, vector<Player> curStandings) {
  int curItem = auctionRounds[rd];
  if (totalPaintings[curItem] < numToWin) {
    return recurse(rd + 1, adversary, curStandings);
  }
  bool weCanWin = curStandings[0].paintings[curItem] == numToWin - 1;
  bool theyCanWin = curStandings[adversary].paintings[curItem] == numToWin - 1;
  if (weCanWin || theyCanWin) {
    // Check if someone else can outbid then we assume they do
    int maxMoney = -1;
    if (weCanWin) maxMoney = curStandings[0].moneyLeft;
    if (theyCanWin) maxMoney = max(maxMoney, curStandings[adversary].moneyLeft);
    for (int otherPlayer = 1; otherPlayer < standings.size(); otherPlayer++) {
      if (otherPlayer == adversary) continue;
      if (curStandings[otherPlayer].moneyLeft > maxMoney) {
        curStandings[otherPlayer].moneyLeft -= maxMoney + 1;
        return recurse(rd + 1, adversary, curStandings);
      }
    }
    // Other's can't outbid us
    if (weCanWin && theyCanWin) {
      // We have the most so we win!
      if (maxMoney == curStandings[0].moneyLeft) return maxMoney;
      // We don't have the most money so we lose
      return -1;
    } else if (weCanWin) {
      if (curStandings[adversary].moneyLeft > maxMoney) {
        curStandings[adversary].moneyLeft -= maxMoney + 1;
        curStandings[adversary].paintings[curItem]++;
        return recurse(rd + 1, adversary, curStandings);
      } else {
        // We win!
        return maxMoney;
      }
    } else {
      if (curStandings[0].moneyLeft > maxMoney) {
        curStandings[0].moneyLeft -= maxMoney + 1;
        curStandings[0].paintings[curItem]++;
        return recurse(rd + 1, adversary, curStandings);
      } else {
        // We lose as we can't outbid the opponent
        return -1;
      }
    }
  }

  // Base cases covered, now we handle the meaty part

  int lo = 0; // We never let the opponent win with just 0
  // Anything above this would be suicide
  int hi = (curStandings[0].moneyLeft + numToWin - curStandings[0].paintings[curItem] - 1) / (numToWin - curStandings[0].paintings[curItem]);
  bool canWin = false;
  vector<Player> newStandings(curStandings);
  while (lo < hi) {
    int mid = lo + ((hi - lo) >> 1);
    newStandings[adversary].paintings[curItem]++;
    newStandings[adversary].moneyLeft -= mid + 1;
    if (newStandings[adversary].moneyLeft >= 0 && recurse(rd + 1, adversary, newStandings) == -1) {
      lo = mid + 1;
    } else {
      canWin = true;
      hi = mid;
    }
    newStandings[adversary].paintings[curItem]--;
    newStandings[adversary].moneyLeft += mid + 1;
  }
  if (canWin) return lo;
  return -1;
}
