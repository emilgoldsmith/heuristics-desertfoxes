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

void assertCenter(Point a, Point b, vector<Point> points, string assertionName = "Unnamed assertion") {
  Point furthestA = getFurthestPoint(a, points);
  int distA = manDist(a, furthestA);
  Point furthestB = getFurthestPoint(b, points);
  int distB = manDist(b, furthestB);
  if (distA != distB) {
    cerr << assertionName << ':' << endl;
    cerr << "Expected minimum furthest distance : " << distB << " such as from point " << b.toString() << endl;
    cerr << "Received minimum furthest distance : " << distA << " from point " << a.toString() << endl;
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
    string testName = "Smart Center Test with the following points:";
    for (Point singlePoint : test.testCase) {
      testName += " " + singlePoint.toString();
    }
    assertCenter(computeCenter(test.testCase), test.expectedCenter, test.testCase, testName);
  }
  for (CenterTest test : centerTests) {
    string testName = "Bruteforce Center Test with the following points:";
    for (Point singlePoint : test.testCase) {
      testName += " " + singlePoint.toString();
    }
    assertCenter(computeCenterBruteforce(test.testCase), test.expectedCenter, test.testCase, testName);
  }
  cout << "Tests Finished" << endl;
}
