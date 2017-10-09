// header guard
#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
  std::chrono::duration<double> currentDuration;
  bool isRunning;
  std::chrono::system_clock::time_point lastStart;
  double allowedTime;

public:
  Timer();
  Timer(double maxTime);
  void start();
  void pause();
  double getTime();
  double timeLeft();
};

#endif
