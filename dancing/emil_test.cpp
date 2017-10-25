#include "geometry.h"
#include "solve.h"

#include <vector>
#include <iostream>
#include <string>
#include <utility>

using namespace std;

void customAssert(Point a, Point b, string assertionName = "Unnamed assertion") {
  if (a != b) {
    cerr << assertionName << ':' << endl;
    cerr << "Expected point: " << b.toString() << endl;
    cerr << "Received point: " << a.toString() << endl;
  } else {
    cout << assertionName << ": PASSED" << endl;
  }
}

struct CenterTest {
  vector<Point> testCase;
  Point expectedCenter;
};

int main() {
  customAssert(Point(8, 8) / 4, Point(2, 2), "Point integer divison");
  vector<CenterTest> centerTests = {
    {{{0, 0}, {0, 4}, {4, 4}, {4, 0}}, {2, 2}},
    {{{0, 0}, {0, 8}, {2, 8}, {2, 0}}, {1, 4}},
    {{{0, 6}, {5, 12}, {6, 8}, {8, 11}, {8, 0}}, {6, 6}}
  };
  for (CenterTest test : centerTests) {
    string testName = "Center Test with the following points:";
    for (Point singlePoint : test.testCase) {
      testName += " " + singlePoint.toString();
    }
    customAssert(computeCenter(test.testCase), test.expectedCenter, testName);
  }
  cout << "Tests Finished" << endl;
}
