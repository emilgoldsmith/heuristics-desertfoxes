#include "client.h"
#include "geometry.hpp"
#include "solve.h"
#include "game_state.h"
#include "pairing_iterator.h"

#include <iostream>
#include <string>

using namespace std;

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
    PairingIterator it(&client);
    SolutionSpec solutionSpec = pairingsToPositions(&client, it.getNext());
    state.simulate(solutionSpec.dancerMapping);
    ChoreographerMove solution = state.currentBestSequence;
    solution.finalPosition = solutionSpec.finalConfiguration;
    client.makeChoreographerMove(solution);
  }
}
