#include "evasion_client.h"

#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;

EvasionClient::EvasionClient(string serverIP, int serverPort) {
  sock = new SocketClient(serverIP, serverPort);

  // send team name prompt and send team name
  string prompt = sock->receive(BUFFER_SIZE, '\n');
  if (prompt != "sendname\n") {
    cerr << "Error: send name prompt incorrect" << endl;
  }
  sock->sendString(TEAM + "\n");

  // receive role
  string role = sock->receive(BUFFER_SIZE, '\n');
  if (role != "hunter\n" && role != "prey\n") {
    cerr << "Error: role information incorrect" << endl;
  }
  isHunter = role == "hunter";

  // receive first update
  receiveUpdate();
  cooldown = (short int) latestUpdate.wallPlacementDelay;
  maxWalls = (short int) latestUpdate.maxWalls;
  state = new GameState(cooldown, maxWalls);
}

void EvasionClient::receiveUpdate() {
  string updateString = sock->receive(BUFFER_SIZE, '\n');
  // check if a new game is starting
  if (updateString == "hunter\n" || updateString == "prey\n") {
    isHunter = updateString == "hunter";
    // receive the first update of the new game
    receiveUpdate();
    return;
  }

  string entry;
  istringstream iss(updateString, istringstream::in);

  // I know this is ugly, but I didn't want to implement my own split
  // and I want the game update to be a struct, not just an array of values
  iss >> entry;
  int playerTimeLeft = stoi(entry);
  iss >> entry;
  int gameNum = stoi(entry);
  iss >> entry;
  int tickNum = stoi(entry);
  iss >> entry;
  int maxWalls = stoi(entry);
  iss >> entry;
  int wallPlacementDelay = stoi(entry);
  iss >> entry;
  int boardSizeX = stoi(entry);
  iss >> entry;
  int boardSizeY = stoi(entry);
  iss >> entry;
  int currentWallTimer = stoi(entry);
  iss >> entry;
  int hunterXPos = stoi(entry);
  iss >> entry;
  int hunterYPos = stoi(entry);
  iss >> entry;
  int hunterXVel = stoi(entry);
  iss >> entry;
  int hunterYVel = stoi(entry);
  iss >> entry;
  int preyXPos = stoi(entry);
  iss >> entry;
  int preyYPos = stoi(entry);
  iss >> entry;
  int numWalls = stoi(entry);

  vector<WallInfo> walls(numWalls);
  for (int i = 0; i < numWalls; i++) {
    iss >> entry;
    int type = stoi(entry);
    int *info;
    if (type == 0 || type == 1) {
      info = new int[3];
      for (int j = 0; j < 3; j++) {
        iss >> entry;
        info[j] = stoi(entry);
      }
    } else { // type == 3 || 4
      info = new int[5];
      for (int j = 0; j < 5; j++) {
        iss >> entry;
        info[j] = stoi(entry);
      }
    }
    walls[i] = { type, info };
  }

  latestUpdate = {
    playerTimeLeft, gameNum, tickNum, maxWalls, wallPlacementDelay,
    boardSizeX, boardSizeY, currentWallTimer, hunterXPos, hunterYPos,
    hunterXVel, hunterYVel, preyXPos, preyYPos, numWalls, walls
  };
}

void EvasionClient::hunterMakeMove() {
  srand(time(0));
  int wallTypeToAdd = rand() % 5;
  bool removeWall = rand() % 4 == 0;
  int wallToRemove = rand() % latestUpdate.walls.size();
  string move = to_string(latestUpdate.gameNum) + " " + to_string(latestUpdate.tickNum) + " " + to_string(wallTypeToAdd);
  if (removeWall && latestUpdate.walls.size() > 0) {
    move += " " + to_string(wallToRemove);
  }
  sock->sendString(move + "\n");
}

void EvasionClient::preyMakeMove() {
  srand(time(0));
  int x = rand() % 2;
  int y = rand() % 2;
  string move = to_string(latestUpdate.gameNum) + " " + to_string(latestUpdate.tickNum) + " " + to_string(x) + " " + to_string (y);
  sock->sendString(move + "\n");
}
