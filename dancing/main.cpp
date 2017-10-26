#include "client.cpp"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv) {
  if (argc != 3) {
    cout << "Usage: ./main.out IP HOST" << endl;
    exit(1);
  }
  string ip = argv[1];
  int port = atoi(argv[2]);
  Client client(ip, port);
}
