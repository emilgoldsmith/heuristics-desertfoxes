#ifndef EVASION_CLIENT_H
#define EVASION_CLIENT_H

#include "../socket/socket_client.h"
#include "game_state.h"
#include <string>
#include <vector>

struct WallInfo {
  int type;
  int* info;
};

struct EvasionGameUpdate {
  int playerTimeLeft;
  int gameNum;
  int tickNum;
  int maxWalls;
  int wallPlacementDelay;
  int boardSizeX;
  int boardSizeY;
  int currentWallTimer;
  int hunterXPos;
  int hunterYPos;
  int hunterXVel;
  int hunterYVel;
  int preyXPos;
  int preyYPos;
  int numWalls;
  std::vector<WallInfo> walls;
};

class EvasionClient {
public:
  // network
  const std::string TEAM = "DesertFoxes";
  const int BUFFER_SIZE = 4096;
  SocketClient *sock;

  // game
  bool isHunter;
  int cooldown;
  int maxWalls;
  GameState *state;
  EvasionGameUpdate latestUpdate;

  // methods
  EvasionClient(std::string serverIP, int serverPort);
  void receiveUpdate();
  std::string toString(HunterMove move);
  std::string toString(Position move);
  HunterMove hunterMakeMove(HunterMove (*hunterSolve)(GameState*));
  Position preyMakeMove(Position (*preySolve)(GameState*));
  Wall wallInfoToWall(WallInfo clientWall);
  HunterMove parseHunterMove();
  Position parsePreyMove();
  bool isConsistent();
};

#endif
