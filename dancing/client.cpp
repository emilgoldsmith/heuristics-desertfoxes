#include "../socket/socket_client.h"
#include "geometry.h"

#include <vector>
#include <string>
#include <queue>

using namespace std;

struct Dancer {
  Point position;
  int color;
};

static bool checkNumDancers(int numDancersCurrentColor, int serverNumDancers, int currentColor) {
  if (numDancersCurrentColor != serverNumDancers) {
    cerr << "Server and client differ in number of dancers for color " << currentColor << endl;
    cerr << "Server: " << serverNumDancers << endl;
    cerr << "Client: " << numDancersCurrentColor << endl;
    return false;
  }
  return true;
}


class Client {
  SocketClient *socket;
  void parseInputFile(string inputFile) {
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

  public:
    const string name = "DesertFoxes";
    vector<Dancer> dancers;
    vector<Point> stars;
    int numColors = 0;
    int serverNumColors;
    int serverBoardSize;
    int serverNumDancers;
    const int role; // 0 for choreographer, 1 for spoiler
    queue<string> messageQueue;

    void send(string s) {
      socket->sendString(s + "&");
    }

    string receive() {
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

    Client(string ip, int port, int playerType): role(playerType) {
      socket = new SocketClient(ip, port);
      // First send name to the server
      send(name);
      // Then we receive the input file
      parseInputFile(receive());
#ifdef LOGGING
      cout << "Dancers parsed:" << endl;
      for (Dancer singleDancer : dancers) {
        cout << singleDancer.position.x << ' ' << singleDancer.position.y << ' ' << singleDancer.color << endl;
      }
#endif
      parseMetaData(receive());
#ifdef DEBUG
      if (!isConsistent()) {
        cerr << "We have inconsistencies between file parsing on server and client" << endl;
      }
#endif
      if (role == 0) {
        // We are choreographer
        parseStars(receive());
      }
#ifdef LOGGING
      cout << "Stars parsed:" << endl;
      for (Point singleStar : stars) {
        cout << singleStar.x << ' ' << singleStar.y << endl;
      }
#endif
    }

    ~Client() {
      delete socket;
    }

    void parseMetaData(string metaData) {
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

    void parseStars(string starString) {
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
    }

    bool isConsistent() {
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
};
