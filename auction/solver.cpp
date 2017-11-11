#ifndef SOLVER_H
#define SOLVER_H

#include <vector>

using namespace std;

Solver::Solver(int artistNum, int winningNum, vector<int> itemsInAuction, int startingWealth, int numPlayers):
  numArtists(artistNum),
  numToWin(winningNum),
  auctionRounds(itemsInAuction)
{
  standings.insert(standings.begin(), numPlayers, {startingWealth, vector<int>(artistNum, 0)});
}
