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
#ifdef DEBUG
  if (prompt != "sendname\n") {
    cerr << "Error: send name prompt incorrect" << endl;
  }
#endif
  sock->sendString(TEAM + "\n");

  // receive role
  string role = sock->receive(BUFFER_SIZE, '\n');
#ifdef DEBUG
  if (role != "hunter\n" && role != "prey\n") {
    cerr << "Error: role information incorrect" << endl;
  }
#endif
  isHunter = role == "hunter";

  // receive first update
  receiveUpdate();
  cooldown = latestUpdate.wallPlacementDelay;
  maxWalls = latestUpdate.maxWalls;
  state = new GameState(cooldown, maxWalls);
}

void EvasionClient::receiveUpdate() {
  string updateString = sock->receive(BUFFER_SIZE, '\n');
  cout << "Received: " << updateString << endl;
  // check if a new game is starting
  if (updateString == "hunter\n" || updateString == "prey\n") {
    isHunter = updateString == "hunter";
    state = new GameState(state->cooldown, state->maxWalls);
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
      // x, y1, y2 or y, x1, x2
      info = new int[3];
      for (int j = 0; j < 3; j++) {
        iss >> entry;
        info[j] = stoi(entry);
      }
    } else {
      // x1, x2, y1, y2, build-direction
      info = new int[5];
      for (int j = 0; j < 5; j++) {
        iss >> entry;
        info[j] = stoi(entry);
      }
    }
    walls[i] = { type, info };
  }

  prevUpdate = latestUpdate; // This should also work fine for first moves as by the time
  // parse(Prey/Hunter) move has been called 2 receiveUpdates have already been called, 1 in constructor
  // and one in the while loop
  latestUpdate = {
    playerTimeLeft, gameNum, tickNum, maxWalls, wallPlacementDelay,
    boardSizeX, boardSizeY, currentWallTimer, hunterXPos, hunterYPos,
    hunterXVel, hunterYVel, preyXPos, preyYPos, numWalls, walls
  };
}

string EvasionClient::toString(HunterMove move)  {
  string moveString = to_string(latestUpdate.gameNum) + " " + to_string(latestUpdate.tickNum) + " ";
  moveString += to_string(move.wallType);
  for (int i : move.indicesToDelete) {
    moveString += " " + to_string(i);
  }
  moveString += "\n";
  return moveString;
}

string EvasionClient::toString(Position move) {
  string moveString = to_string(latestUpdate.gameNum) + " " + to_string(latestUpdate.tickNum) + " ";
  moveString += to_string(move.x) + " " + to_string(move.y) + "\n";
  return moveString;
}

HunterMove EvasionClient::hunterMakeMove(HunterMove (*hunterSolve)(GameState*)) {
  HunterMove move = hunterSolve(state);
  string moveString = toString(move);
  cout << "Sending: " << moveString << endl;
  sock->sendString(moveString);
  return move;
}

Position EvasionClient::preyMakeMove(Position (*preySolve)(GameState*)) {
  if (!state->preyMoves) {
    Position move = { 2, 2 };
    sock->sendString(toString(move));
    return move;
  }
  Position move = preySolve(state);
  string moveString = toString(move);
  cout << "Sending: " << moveString << endl;
  sock->sendString(moveString);
  return move;
}

Wall EvasionClient::wallInfoToWall(WallInfo clientWall) {
  Position cwStart, cwEnd;
  // y x1 x2
  if (clientWall.type == 0) {
    cwStart = { clientWall.info[1], clientWall.info[0] };
    cwEnd = { clientWall.info[2], clientWall.info[0] };
  // x y1 y2
  } else if (clientWall.type == 1) {
    cwStart = { clientWall.info[0], clientWall.info[1] };
    cwEnd = { clientWall.info[0], clientWall.info[2] };
  // x1 x2 y1 y2 build-direction
  } else {
    cwStart = { clientWall.info[0], clientWall.info[1] };
    cwEnd = { clientWall.info[2], clientWall.info[3] };
  }

  // we are disregarding the creationPoint entry
  return {cwStart, cwEnd, -1};
}

HunterMove EvasionClient::parseHunterMove() {
  // compare prev update with latest update
  vector<WallInfo> latestWalls = latestUpdate.walls;
  vector<WallInfo> prevWalls = prevUpdate.walls;
  int newWallType = 0;
  vector<int> deletedWallIndices;
  // check for new wall
  for (WallInfo cw : latestWalls) {
    Wall cwall = wallInfoToWall(cw);
    bool found = false;
    for (WallInfo wallRepresentation : prevWalls) {
      Wall sw = wallInfoToWall(wallRepresentation);
      if (cwall.start == sw.start && cwall.end == sw.end) {
        found = true;
        break;
      }
    }
    if (!found) {
      newWallType = cw.type + 1;
      break;
    }
  }
  // check for deleted walls
  for (int i = 0; i < prevWalls.size(); i++) {
    Wall sw = wallInfoToWall(prevWalls[i]);
    bool found = false;
    for (WallInfo cw: latestWalls) {
      Wall cwall = wallInfoToWall(cw);
      if (sw.start == cwall.start && sw.end == cwall.end) {
        found = true;
        break;
      }
    }
    if (!found) {
      deletedWallIndices.push_back(i);
    }
  }

  return { newWallType, deletedWallIndices };
}

Position EvasionClient::parsePreyMove() {
  // Compare prev update with latest update
  int dx = latestUpdate.preyXPos - prevUpdate.preyXPos;
  int dy = latestUpdate.preyYPos - prevUpdate.preyYPos;
  if (latestUpdate.tickNum % 2 == 0) {
    return {2, 2};
  }
  return {dx, dy};
}

bool EvasionClient::isConsistent() {
  if (latestUpdate.boardSizeX != state->boardSize.x || latestUpdate.boardSizeY != state->boardSize.y) {
    cerr << "Board size mismatch" << endl;
    return false;
  }
  if (latestUpdate.tickNum != state->score) {
    cerr << "TickNum/score Mismatch: " << latestUpdate.tickNum << ", " << state->score << endl;
    return false;
  }
  if (latestUpdate.currentWallTimer != state->cooldownTimer) {
    cerr << "Wall cooldown mismatch" << endl;
    return false;
  }
  if (latestUpdate.hunterXPos != state->hunter.x || latestUpdate.hunterYPos != state->hunter.y) {
    cerr << "Hunter position mismatch" << endl;
    return false;
  }
  if (latestUpdate.hunterXVel != state->hunterDirection.x || latestUpdate.hunterYVel != state->hunterDirection.y) {
    cerr << "Hunter velocity/direction mismatch" << endl;
    return false;
  }
  if (latestUpdate.preyXPos != state->prey.x ||latestUpdate.preyYPos != state->prey.y) {
    cerr << "Prey position mismatch" << endl;
    return false;
  }
  if (latestUpdate.walls.size() != state->walls.size()) {
    cerr << "Wall number mismatch" << endl;
    return false;
  }
  for (int i = 0; i < latestUpdate.walls.size(); i++) {
    WallInfo clientWall = latestUpdate.walls[i];
    Wall stateWall = state->walls[i];
    // horizontal, y, x1, x2
    if (clientWall.type == 0) {
      if (stateWall.start.x != clientWall.info[1] || stateWall.start.y != clientWall.info[0]) {
        cerr << "Horizontal wall start/end mismatch" << endl;
        return false;
      }
      if (stateWall.end.x != clientWall.info[2] || stateWall.end.y != clientWall.info[0]) {
        cerr << "Horizontal wall start/end mismatch" << endl;
        return false;
      }
    // vertical, x, y1, y2
    } else if (clientWall.type == 1) {
      if (stateWall.start.x != clientWall.info[0] || stateWall.start.y != clientWall.info[1]) {
        cerr << "Vertical wall start/end mismatch" << endl;
        return false;
      }
      if (stateWall.end.x != clientWall.info[0] || stateWall.end.y != clientWall.info[2]) {
        cerr << "Vertical wall start/end mismatch" << endl;
        return false;
      }
    // diagonal/counterdiagonal, x1, x2, y1, y2, build-direction
    } else if (clientWall.type == 3 || clientWall.type == 4) {
      // check start/end points
      if (stateWall.start.x != clientWall.info[0] || stateWall.start.y != clientWall.info[2]) {
        cerr << "(Counter)diagonal wall start/end mismatch" << endl;
        return false;
      }
      if (stateWall.end.x != clientWall.info[1] || stateWall.end.y != clientWall.info[3]) {
        cerr << "(Counter)diagonal wall start/end mismatch" << endl;
        return false;
      }
      // check diagonal vs. counterdiagonal
      int parity = (stateWall.end.x - stateWall.start.x) * (stateWall.end.y - stateWall.start.y);
      if ((clientWall.type == 3 && parity < 0) || (clientWall.type == 4 && parity > 0)) {
        cerr << "Diagonality and coordinates mismatch" << endl;
        return false;
      }
      // check build direction
      // diagonal and build x first
      if (clientWall.type == 3 && clientWall.info[4] == 0) {

      // diagonal and build y first
      } else if (clientWall.type == 3 && clientWall.info[4] == 1)  {

      // counter diagonal and build x first
      } else if (clientWall.type == 4 && clientWall.info[4] == 0)  {

      // counter diagonal and build y first
      } else if (clientWall.type == 4 && clientWall.info[4] == 1)  {

      }
    }
  }

  return true;
}
