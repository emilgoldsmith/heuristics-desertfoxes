#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "json.hpp"
using json = nlohmann::json;
using string = std::string;

// s is the socket fd, buf is the data buffer, and len is a pointer to length of buffer
int sendAll(int s, char *buf, int* len) {
  int total = 0; // number of bytes sent so far
  int bytesLeft = *len;
  int n;

  while (total < *len) {
    // n is the actual number of bytes sent
    n = send(s, buf + total, bytesLeft, 0);
    if (n == -1) {
      break;
    }
    total += n;
    bytesLeft -= n;
  }

  // total number of bytes sent is returned to len
  *len = total;
  // return -1 on failure
  return n == -1 ? -1: 0;
}

int sendJSON(int s, json j) {
  string jsonString = j.dump();
  int len = jsonString.length();
  char *data = (char *) jsonString.data();

  return sendAll(s, data, &len);
}

int makeMove(int s, int order, int numStones, bool useReset) {
  json j;
  j["order"] = order;
  // j["num_stones"] = numStones;
  // j["reset"] = useReset;
  j["name"] = "lolz";

  return sendJSON(s, j);
}

int main(int argc, char const *argv[])
{
  const int PORT = 9000;
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    exit(0);
  }

  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
  if (connect(fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
    exit(0);
  } else {
    makeMove(fd, 1, 2, true);
  }

  return 0;
}
