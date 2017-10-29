#ifndef CLIENT_H
#define CLIENT_H

#include "../socket/socket_client.h"
#include "geometry.hpp"
#include "structs.h"

#include <vector>
#include <string>
#include <queue>

class Client {
  SocketClient *socket;
  std::queue<std::string> messageQueue;

  void parseInputFile(std::string inputFile);
  bool isConsistent();
  void send(std::string s);
  std::string receive();
  void parseMetaData(std::string metaData);
  void parseStars(std::string starString);
  bool starsAreValid(std::vector<Point> starsToPlace);

  public:
    const std::string name = "DesertFoxes";
    std::vector<Dancer> dancers;
    std::vector<Point> stars;
    int numColors = 0;
    int serverNumColors;
    int serverBoardSize;
    int serverNumDancers;
    const int role; // 0 for choreographer, 1 for spoiler
    bool gameOver = false;

    Client(std::string ip, int port, int playerType);
    Client(std::vector<Dancer> xdancers, std::vector<Point> xstars, int xnumColors, int xboardSize, int xnumDancers); // This is only meant to be used in testing
    ~Client();
    void makeSpoilerMove(std::vector<Point> starsToPlace);
    void makeChoreographerMove(ChoreographerMove move);
};

#endif
