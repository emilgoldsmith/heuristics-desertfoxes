#include "client.h"
#include "pairing_iterator.h"
#include "structs.h"

#include <vector>

using namespace std;

PairingIterator::PairingIterator(Client *inputClient): client(inputClient) {
  vector<vector<Point>> dancersByColor(client->serverNumColors);
  vector<Point> dancerPoints;
  dancerPoints.reserve(client->serverNumColors * client->serverNumDancers);
  for (Dancer singleDancer : client->dancers) {
    dancerPoints.push_back(singleDancer.position);
  }
  for (int color = 0; color < client->serverNumColors; color++) {
    dancersByColor[color].insert(dancersByColor[color].begin(), dancerPoints.begin() + color * client->serverNumDancers, dancerPoints.begin() + (color + 1) * client->serverNumDancers);
  }
  for (Dancer singleDancer : client->dancers) {
    vector<vector<Point>> dancersByColorCopy(dancersByColor);
    dancersByColorCopy[singleDancer.color].clear();
    for (int color = 0; color < client->serverNumColors; color++) {
      if (color == singleDancer.color) continue;
      sort(dancersByColorCopy[color].begin(), dancersByColorCopy[color].end(),
          [singleDancer](const Point& a, const Point& b) -> bool
            { return manDist(a, singleDancer.position) < manDist(b, singleDancer.position); });
    }
    sortedDancers.push_back({dancersByColorCopy});
  }
}

vector<Pairing> PairingIterator::getNext() {
  int n = client->serverBoardSize;
  vector<vector<bool>> dancerPicked(n, vector<bool>(n, false));
  vector<Pairing> pairings;
  pairings.reserve(client->serverNumDancers);
  for (int i = 0; i < sortedDancers.size(); i++) {
    Point curDancer = client->dancers[i].position;
    if (dancerPicked[curDancer.x][curDancer.y]) continue;
    dancerPicked[curDancer.x][curDancer.y] = true;
    Pairing curPairing;
    curPairing.dancers.push_back(curDancer);
    for (vector<Point> sortedOtherDancers : sortedDancers[i].sortedOtherDancers) {
      if (sortedOtherDancers.size() == 0) continue; // This is our own color
      for (Point candidateDancer : sortedOtherDancers) {
        if (!dancerPicked[candidateDancer.x][candidateDancer.y]) {
          dancerPicked[candidateDancer.x][candidateDancer.y] = true;
          curPairing.dancers.push_back(candidateDancer);
          break;
        }
      }
    }
    pairings.push_back(curPairing);
  }
  return pairings;
}
