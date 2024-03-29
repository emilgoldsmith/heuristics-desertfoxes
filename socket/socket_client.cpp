#include "socket_client.h"

#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstdio>

using namespace std;
using json = nlohmann::json;

SocketClient::SocketClient(string socketAddress, int socketPort) {
  sockAddress = socketAddress;
  sockPort = socketPort;
  sockFD = socket(AF_INET, SOCK_STREAM, 0);
  if (sockFD < 0) {
    printf("Socket initialization failed on %s:%d\n", sockAddress.data(), sockPort);
    exit(0);
  }

  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(sockPort);
  inet_pton(AF_INET, sockAddress.data(), &address.sin_addr);
  if (connect(sockFD, (struct sockaddr *) &address, sizeof(address)) < 0) {
    printf("Socket failed to connect to %s:%d\n", sockAddress.data(), sockPort);
    exit(0);
  }
}

SocketClient::~SocketClient() {
  close(sockFD);
}

// send all of the first len bytes of buf
int SocketClient::sendAll(char *buf, int len) {
  int total = 0; // number of bytes sent so far
  int bytesLeft = len;
  int n;

  while (total < len) {
    // n is the actual number of bytes sent
    n = send(sockFD, buf + total, bytesLeft, 0);
    if (n == -1) { // error
      break;
    }
    total += n;
    bytesLeft -= n;
  }

  // return -1 on failure
  return n == -1 ? -1: 0;
}

// receive a string
string SocketClient::receive(int bufferSize, char end) {
  char *data = new char [bufferSize];
  string dataString = "";

  // end of message is not specified
  if (end == ' ') {
    int bytesReceived = recv(sockFD, data, bufferSize, 0);
    dataString = string(data, bytesReceived);
  // end of message is specified
  } else {
    while (true) {
      int bytesReceived = recv(sockFD, data, bufferSize, 0);
      dataString += string(data, bytesReceived);
      if (dataString[dataString.size() - 1] == end) {
        break;
      }
    }
  }

  delete[] data;
  return dataString;
}

// send string
int SocketClient::sendString(string s) {
  char *ptrToString = (char *)s.data(); // convert to C string
  return sendAll(ptrToString, s.length()); // We don't need the null character at the end so therefore we don't add + 1 to the length
}

// send JSON
int SocketClient::sendJSON(json j) {
  string jsonString = j.dump();
  return sendString(jsonString);
}

// receive JSON
json SocketClient::receiveJSON(int bufferSize, char end) {
  string jsonString = SocketClient::receive(bufferSize, end);
  return json::parse(jsonString);
}

void SocketClient::closeSocket() {
  close(sockFD);
}

// receive a potentially large graph from server
// the last two characters to be received are "}}"
json SocketClient::receiveASP(int bufferSize) {
  char *data = new char [bufferSize];
  int totalReceived = 0;
  while (true) {
    cout << "Receiving" << endl;
    int bytesReceived = recv(sockFD, data + totalReceived, bufferSize, 0);
    cout << "Received" << endl;
    totalReceived += bytesReceived;
    if (data[totalReceived - 1] == '}' && data[totalReceived - 2] == '}') {
      break;
    }
  }

  string dataString(data, totalReceived);
  delete [] data;
  return json::parse(dataString);
}
