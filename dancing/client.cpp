#include "client.h"
#include "../socket/socket_client.h"
#include "geometry.hpp"
#include "structs.h"

#include <vector>
#include <string>
#include <queue>
#include <sstream>
#include <iostream>

using namespace std;

Client::Client(std::vector<Dancer> xdancers, std::vector<Point> xstars, int xnumColors, int xboardSize, int xnumDancers): dancers(xdancers), stars(xstars), numColors(xnumColors), serverNumColors(xnumColors), serverBoardSize(xboardSize), serverNumDancers(xnumDancers), role(0) {
#ifndef TEST
  cerr << "ERROR: Used test client constructor in non test environment" << endl;
#endif
}
Client::Client(string ip, int port, int playerType): role(playerType) {
  socket = new SocketClient(ip, port);
  // First send name to the server
  send(name);
  // Then we get the meta data
  parseMetaData(receive());
  // Then we receive the input file
  parseInputFile(receive());
#ifdef LOGGING
  cout << "Dancers parsed:" << endl;
  for (Dancer singleDancer : dancers) {
    cout << singleDancer.position.x << ' ' << singleDancer.position.y << ' ' << singleDancer.color << endl;
  }
#endif
#ifdef DEBUG
  if (!isConsistent()) {
    cerr << "We have inconsistencies between file parsing on server and client" << endl;
  }
#endif
  if (role == 0) {
    // We are choreographer
    parseStars(receive());
#ifdef DEBUG
    if (!starsAreValid(stars)) {
      cerr << "The stars we received were invalid" << endl;
    }
#endif
#ifdef LOGGING
    cout << "Stars parsed:" << endl;
    for (Point singleStar : stars) {
      cout << singleStar.x << ' ' << singleStar.y << endl;
    }
#endif
  }
}

Client::~Client() {
  delete socket;
}

void Client::send(string s) {
  socket->sendString(s + "&");
}

string Client::receive() {
  if (messageQueue.empty()) {
    string receivedMessage = socket->receive(1000, '&'); // The return value includes the last &
#ifdef LOGGING
    cout << "Received: " << receivedMessage << endl;
#endif
    int startOfLastMessage = 0;
    for (int i = 0; i < receivedMessage.size(); i++) {
      if (receivedMessage[i] == '&') {
        messageQueue.push(receivedMessage.substr(startOfLastMessage, i - startOfLastMessage));
        startOfLastMessage = i + 1;
      }
    }
  }
  string nextMessage = messageQueue.front();
  messageQueue.pop();
  return nextMessage;
}

void Client::parseInputFile(string inputFile) {
  for (int lineStart = 0; lineStart < inputFile.size();) {
    string line = "";
    for (int lineIndex = lineStart; inputFile[lineIndex] != '\n' && lineIndex < inputFile.size(); lineIndex++) {
      line += inputFile[lineIndex];
    }
    if (line[0] == 'D') { // it is the start of a new color
      numColors++;
    } else {
      // It is a dancer of color numColors--
      string xCord = "";
      string yCord = "";
      bool nextCoordinate = false;
      for (char c : line) {
        if (c != ' ' && c != '\n') {
          if (!nextCoordinate) {
            xCord += c;
          } else {
            yCord += c;
          }
        } else {
          if (!nextCoordinate) {
            nextCoordinate = true;
          } else {
            break;
          }
        }
      }
      int x = stoi(xCord);
      int y = stoi(yCord);
      dancers.push_back({{x, y}, numColors - 1});
    }
    lineStart += line.size() + 1; // The extra 1 is for the newline
  }
}

void Client::parseMetaData(string metaData) {
  string values[3] = {"", "", ""};
  int valueIndex = 0;
  for (char c : metaData) {
    if (c == ' ') {
      valueIndex++;
    } else {
      values[valueIndex] += c;
    }
  }
  serverBoardSize = stoi(values[0]);
  serverNumColors = stoi(values[1]);
  serverNumDancers = stoi(values[2]);
}

void Client::parseStars(string starString) {
  string values[2] = {"", ""};
  int valueIndex = 0;
  for (char c : starString) {
    if (c == ' ') {
      valueIndex++;
      if (valueIndex == 2) {
        // We have parsed a full star
        stars.push_back({stoi(values[0]), stoi(values[1])});
        valueIndex = 0;
        values[0] = values[1] = "";
      }
    } else {
      values[valueIndex] += c;
    }
  }
  if (values[0] != "" && values[1] != "") {
    // There was no trailing space so we need to remember to parse last star as well!
    stars.push_back({stoi(values[0]), stoi(values[1])});
  }
}

static bool checkNumDancers(int numDancersCurrentColor, int serverNumDancers, int currentColor) {
  if (numDancersCurrentColor != serverNumDancers) {
    cerr << "Server and client differ in number of dancers for color " << currentColor << endl;
    cerr << "Server: " << serverNumDancers << endl;
    cerr << "Client: " << numDancersCurrentColor << endl;
    return false;
  }
  return true;
}

bool Client::isConsistent() {
  bool consistent = true;
  if (serverNumColors != numColors) {
    consistent = false;
    cerr << "Server and client differ in number of colors" << endl;
    cerr << "Server: " << serverNumColors << endl;
    cerr << "Client: " << numColors << endl;
  }
  int currentColor = 0;
  int numDancersCurrentColor = 0;
  for (Dancer curDancer : dancers) {
    if (curDancer.position.x < 0 || curDancer.position.x >= serverBoardSize ||
        curDancer.position.y < 0 || curDancer.position.y >= serverBoardSize) {
      consistent = false;
      cerr << "Invalid dancer position in relation to board size" << endl;
      cerr << "Dancer position: " << curDancer.position.x << ' ' << curDancer.position.y << endl;
      cerr << "Boardsize " << serverBoardSize << endl;
    }
    if (curDancer.color == currentColor) {
      numDancersCurrentColor++;
    } else {
      // We're done with this color as we assume our vector is sorted by colors due to parsing order
      if (!checkNumDancers(numDancersCurrentColor, serverNumDancers, currentColor)) consistent = false;
      numDancersCurrentColor = 1;
      currentColor++;
    }
  }
  if (!checkNumDancers(numDancersCurrentColor, serverNumDancers, currentColor)) consistent = false;
  if (dancers.size() != serverNumColors * serverNumDancers) {
    consistent = false;
    cerr << "Total number of dancers doesn't match up" << endl;
    cerr << "Expected: " << serverNumDancers * serverNumColors << endl;
    cerr << "Actual value: " << dancers.size() << endl;
  }
  return consistent;
}

bool Client::starsAreValid(vector<Point> starsToPlace) {
  bool valid = true;
  // Make sure the manhattan distance is at least k+1 between all stars
  for (Point firstStar : starsToPlace) {
    bool foundItself = false;
    for (Point secondStar : starsToPlace) {
      if (!foundItself && firstStar == secondStar) {
        // We allow one duplicate since we are iterating through the same list
        foundItself = true;
        continue;
      }
      if (manDist(firstStar, secondStar) <= serverNumColors) {
        valid = false;
        cerr << "Star " << firstStar.toString() << " and star " << secondStar.toString() << " are closer than k + 1 = " << serverNumDancers + 1 << " in manhattan distance" << endl;
      }
    }
  }
  // Make sure no stars coincide with a dancer
  for (Point star : starsToPlace) {
    for (Dancer curDancer : dancers) {
      if (star == curDancer.position) {
        valid = false;
        cerr << "Star and dancer coincided at " << star.toString() << endl;
      }
    }
  }
  return valid;
}

void Client::makeSpoilerMove(vector<Point> starsToPlace) {
#ifdef DEBUG
  if (role != 1) {
    cerr << "Attempted making spoiler move while not spoiler" << endl;
    cerr << "Aborting spoiler move attempt" << endl;
    return;
  }
  if (gameOver) {
    cerr << "Attempted to place stars more than once, this is not allowed" << endl;
    cerr << "Aborting spoiler move attempt" << endl;
    return;
  }
  // Make sure we are actually sending k stars (sending less would never be beneficial)
  if (starsToPlace.size() != serverNumDancers) {
    cerr << "Not sending k stars, k is " << serverNumDancers << " and attempted number of stars sent was " << starsToPlace.size() << endl;
  }
  if (!starsAreValid(starsToPlace)) {
    cerr << "The attempted star placement was invalid" << endl;
    cerr << "Aborting spoiler move attempt" << endl;
    return;
  }
#endif

  stringstream ss;
  for (Point star : starsToPlace) {
    ss << star.x << ' ' << star.y << ' ';
  }
  string starString = ss.str();
  starString = starString.substr(0, starString.size() - 1); // Remove last space
  send(starString);
  gameOver = true;
}

void Client::makeChoreographerMove(ChoreographerMove move) {
#ifdef DEBUG
  if (move.finalPosition.size() != serverNumDancers) {
    cerr << "Final position for choreographer invalid, there " << move.finalPosition.size() << " lines instead of " << serverNumDancers << endl;
  }
#endif
  // We leave the rest of the validation to the server as we'll never see this data again anyway

  // First send all the moves we're gonna make
  for (vector<DancerMove> singleStep : move.dancerMoves) {
    stringstream ss;
    ss << singleStep.size() << ' ';
    for (DancerMove singleMove : singleStep) {
      ss << singleMove.from.x << ' ' << singleMove.from.y << ' ' << singleMove.to.x << ' ' << singleMove.to.y << ' ';
    }

    string stepString = ss.str();
#ifdef LOGGING
    cout << "MOVES WE ARE SENDING:" << endl;
    cout << stepString << endl << endl;
#endif
    // We purposefully don't strip the last space as it is actually needed for the server to parse it correctly
    send(stepString);
  }
  // Signify that we are done making moves
  send("DONE");

  // Tell the server the configuration of the final position
  stringstream ss;
  for (EndLine singleLine : move.finalPosition) {
    ss << singleLine.start.x << ' ' << singleLine.start.y << ' ' << singleLine.end.x << ' ' << singleLine.end.y << ' ';
  }
  string finalPositionString = ss.str();
  finalPositionString = finalPositionString.substr(0, finalPositionString.size() - 1); // Remove last space
#ifdef LOGGING
  cout << "FINAL POSITIONS WE ARE SENDING:" << endl;
  cout << finalPositionString << endl;
#endif
  send(finalPositionString);
}
