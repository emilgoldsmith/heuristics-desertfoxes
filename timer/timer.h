// header guard
#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
  std::chrono::duration<double> currentDuration;
  bool isRunning;
  std::chrono::system_clock::time_point lastStart;
  const double allowedTime = 60+55; // 1:55 minutes for a 5 second buffer

public:
  Timer();
  void start();
  void pause();
  double getTime();
  double timeLeft();
};

#endif
