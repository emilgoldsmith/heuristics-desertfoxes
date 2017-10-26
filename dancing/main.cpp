#include "client.cpp"

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
  Client client(ip, port, atoi(argv[3]));
}
