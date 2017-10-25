#include "geometry.h"
#include "solve.h"

#include <vector>
#include <iostream>
#include <string>

using namespace std;

void customAssert(Point a, Point b, string assertionName = "Unnamed assertion") {
  if (a != b) {
    cerr << assertionName << ':' << endl;
    cerr << "Expected point: " << b.toString() << endl;
    cerr << "Received point: " << a.toString() << endl;
  }
}

int main() {
  customAssert(Point(8, 8) / 4, Point(2, 2), "Point integer divison");
  vector<vector<Point>> tests = {
    {{0, 0}, {0, 4}, {4, 4}, {4, 0}}
  };
  for (vector<Point> singleTest : tests) {
    string testName = "Test with the following points:";
    for (Point singlePoint : singleTest) {
      testName += " " + singlePoint.toString();
    }
    customAssert(computeCenter(singleTest), {2, 2}, testName);
  }
}
