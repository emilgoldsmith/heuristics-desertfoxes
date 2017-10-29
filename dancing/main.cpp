#include "client.h"
#include "geometry.hpp"
#include "solve.h"
#include "game_state.h"
#include "pairing_iterator.h"
#include "../random/random.h"

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

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

double getSACost(vector<Pairing> pairings, Client *client, GameState *state) {
  SolutionSpec solutionSpec = pairingsToPositions(client, pairings);
  return state->simulate(solutionSpec).dancerMoves.size();
}

int main(int argc, char **argv) {
  if (argc != 4) {
    cout << "Usage: ./main.out IP HOST (0 == choreographer, 1 == spoiler)" << endl;
    exit(1);
  }
  string ip = argv[1];
  int port = atoi(argv[2]);
  int role = atoi(argv[3]);
  Client client(ip, port, role);
  GameState state(client.serverBoardSize, client.serverNumColors, client.dancers, client.stars);
  if (role == 1) {
    client.makeSpoilerMove(dummyPlaceStars(&client));
  } else {

    // First we do a few runs with pairings that are close to each other
    PairingIterator it(&client);
    for (int i = 0; i < 10; i++) {
      SolutionSpec solutionSpec = pairingsToPositions(&client, it.getNext());
      state.simulate(solutionSpec);
    }


    // Then we try simulated annealing
    Random r;
    vector<Pairing> oldPairing = it.getNext();
    double oldCost = getSACost(oldPairing, &client, &state);
    double T = 1.0;
    double T_min = 0.00001;
    double alpha = 0.5;
    while (T > T_min) {
      for (int i = 0; i < 10; i++) {
        vector<Pairing> newPairing = getSANeighbour(oldPairing);
        double newCost = getSACost(newPairing, &client, &state);
        double acceptanceProbability = exp((oldCost - newCost)/T);
        if (acceptanceProbability > r.randDouble(0, 1)) {
          oldPairing = newPairing;
          oldCost = newCost;
        }
      }
      T *= alpha;
    }


    // Then we send the best solution we could find
    ChoreographerMove solution = state.currentBestSequence;
    client.makeChoreographerMove(solution);
  }
}
