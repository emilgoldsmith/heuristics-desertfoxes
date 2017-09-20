#include "timer.h"

#include <iostream>

using namespace std;

int main() {
  Timer timer;
  // timer.pause() // should throw error

  // timer.start()
  // timer.start() // should throw error
  cout << "Time elapsed: " << timer.getTime() << endl;
  cout << "Time left: " << timer.timeLeft() << endl;
  timer.start();
  cout << "Time elapsed: " << timer.getTime() << endl;
  cout << "Time left: " << timer.timeLeft() << endl;
  for (int i = 0; i < 1000*1000*1000; i++) {
    i--;
    i++;
  }
  cout << "Time elapsed: " << timer.getTime() << endl;
  cout << "Time left: " << timer.timeLeft() << endl;
  cout << "pausing..." << endl;
  timer.pause();
  for (int i = 0; i < 1000*1000*1000; i++) {
    i--;
    i++;
  }

  for (int i = 0; i < 1000*1000*1000; i++) {
    i--;
    i++;
  }
  cout << "restarting..." << endl;
  timer.start();
  cout << "Time elapsed: " << timer.getTime() << endl;
  cout << "Time left: " << timer.timeLeft() << endl;
  for (int i = 0; i < 1000*1000*1000; i++) {
    i--;
    i++;
  }
  cout << "Time elapsed: " << timer.getTime() << endl;
  cout << "Time left: " << timer.timeLeft() << endl;
}
