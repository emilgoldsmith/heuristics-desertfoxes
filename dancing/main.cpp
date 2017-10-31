#include "client.h"
#include "geometry.hpp"
#include "solve.h"
#include "game_state.h"
#include "pairing_iterator.h"
#include "../random/random.h"
#include "../timer/timer.h"

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

/*
vector<Pairing> getSANeighbour(vector<Pairing> source) {
  Random r;
  int firstIndex = r.randInt(0, source.size() - 1);
  int secondIndex = r.randInt(0, source.size() - 1);
  while (secondIndex == firstIndex) {
    secondIndex = r.randInt(0, source.size() - 1);
  }
  int colorToSwap = r.randInt(0, source[0].dancers.size() - 1);
  swap(source[firstIndex].dancers[colorToSwap], source[secondIndex].dancers[colorToSwap]);
  return source;
}

double getSACost(vector<Pairing> pairings) {
  int maxDist = -1;
  for (Pairing curPairing : pairings) {
    Point center = computeCenterBruteforce(curPairing.dancers);
    for (Point curPoint : curPairing.dancers) {
      maxDist = max(maxDist, manDist(center, curPoint));
    }
  }
  return maxDist;
}
*/

int main(int argc, char **argv) {
  if (argc != 4) {
    cout << "Usage: ./main.out IP HOST (0 == choreographer, 1 == spoiler)" << endl;
    exit(1);
  }
  string ip = argv[1];
  int port = atoi(argv[2]);
  int role = atoi(argv[3]);
  Client client(ip, port, role);
  Timer t(60 + 58);
  t.start();
  GameState state(client.serverBoardSize, client.serverNumColors, client.dancers, client.stars, &t);
  if (role == 1) {
    client.makeSpoilerMove(choreoPlaceStars(&client, t));
  } else {

    /*
    // Then we compute a center for all dancers and place them in there
    vector<Point> placeholder;
    SolutionSpec solutionSpec = solveManyPoints(&client, client.dancers, placeholder);
    state.simulate(solutionSpec, "compute center for all dancers");
    */

    /*
    // First do a round of Simulated Annealing
    Random r;
    vector<Pairing> oldPairing = it.getNext();
    double oldCost = getSACost(oldPairing);
    vector<Pairing> bestPairing = oldPairing;
    double bestCost = oldCost;
    double T = 1000.0;
    double T_min = 0.00001;
    double alpha = 0.945;
    while (T > T_min) {
      for (int i = 0; i < 100; i++) {
        vector<Pairing> newPairing = getSANeighbour(oldPairing);
        double newCost = getSACost(newPairing);
        double acceptanceProbability = exp((oldCost - newCost)/T);
        if (acceptanceProbability > r.randDouble(0, 1)) {
          oldPairing = newPairing;
          oldCost = newCost;
          if (oldCost < bestCost) {
            bestCost = oldCost;
            bestPairing = oldPairing;
          }
        }
      }
      T *= alpha;
    }
    // Test the result of SA
    SolutionSpec solutionSpec = pairingsToPositions(&client, bestPairing);
    for (int i = 0; i < 10; i++) {
      // Simulate it several times as simulate has some random stuff going on
      state.simulate(solutionSpec, "Simulated Annealing");
    }
#ifdef DEBUG
    cout << "Simulated Annealing finished after " << t.getTime() << " seconds" << endl;
#endif

    */
    // Then we just run pairingsToPositions
    PairingIterator it(&client);
    // Keep going until we're out of time
#ifdef DEBUG
    for (int i = 0; i < 20; i++) {
#else
    while (t.timeLeft() > 0) {
#endif
      SolutionSpec solutionSpec = pairingsToPositions(&client, it.getNext());
      state.simulate(solutionSpec, "pairingsToPositions");
    }

    // Then we send the best solution we could find
    ChoreographerMove solution = state.currentBestSequence;
    client.makeChoreographerMove(solution);
#ifdef DEBUG
    state.printStrategyStats();
#endif
    cout << "We simulated " << state.numSimulations << " simulations. Wow! How awesome aren't we??" << endl;
  }
}
