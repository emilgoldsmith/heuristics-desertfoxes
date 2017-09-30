#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

int pipefd_write[2];
int pipefd_read[2];
FILE *pythonfd;

void init(string command) {
  if (pipe(pipefd_write) == -1) {
    cerr << "Failed to create pipe" << endl;
    exit(1);
  }
  if (pipe(pipefd_read) == -1) {
    cerr << "Failed to create pipe" << endl;
    exit(1);
  }

  string pythonProgramCommand = command + " " + to_string(pipefd_write[0]) + " " + to_string(pipefd_read[1]);

  pythonfd = popen(pythonProgramCommand.data(), "w");
}

string readMsg() {
  string msg = "";
  char buf;
  while (read(pipefd_read[0], &buf, 1) > 0) {
    msg += buf;
    if (buf == (char)0) {
      break;
    }
  }
  return msg;
}

void writeMsg(string msg) {
  string msgToSend = msg;
  write(pipefd_write[1], msg.data(), msg.size() + 1);
}

void teardown() {
  pclose(pythonfd);
  close(pipefd_write[0]);
  close(pipefd_write[1]);
  close(pipefd_read[0]);
  close(pipefd_write[1]);
}
