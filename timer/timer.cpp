#include "timer.h"

#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

Timer::Timer() {
  isRunning = false;
  allowedTime = 60 + 55;
  currentDuration = duration<double>(0);
}

Timer::Timer(double maxTime) {
  isRunning = false;
  allowedTime = maxTime;
  currentDuration = duration<double>(0);
}

void Timer::start() {
  if (isRunning) {
    cerr << "Attempted starting timer while it was already running" << endl;
    exit(1);
  }
  isRunning = true;
  lastStart = system_clock::now();
}

void Timer::pause() {
  if (!isRunning) {
    cerr << "Attempted pausing already paused timer" << endl;
    exit(1);
  }
  isRunning = false;
  system_clock::time_point now = system_clock::now();
  currentDuration += duration_cast<duration<double> >(now - lastStart);
}

double Timer::getTime() {
  double timeElapsed = currentDuration.count();
  if (isRunning) {
    system_clock::time_point now = system_clock::now();
    timeElapsed += duration_cast<duration<double> >(now - lastStart).count();
  }
  return timeElapsed;
}

double Timer::timeLeft() {
  return allowedTime - getTime();
}
