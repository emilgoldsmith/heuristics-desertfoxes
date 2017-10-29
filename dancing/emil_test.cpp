#include "geometry.hpp"
#include "solve.h"
#include "spiral_iterator.h"
#include "structs.h"
#include "client.h"

#include <vector>
#include <iostream>
#include <string>
#include <utility>

using namespace std;

void assertPoint(Point a, Point b, string assertionName = "Unnamed assertion") {
  if (a != b) {
    cerr << assertionName << ':' << endl;
    cerr << "Expected point: " << b.toString() << endl;
    cerr << "Received point: " << a.toString() << endl;
  } else {
    cout << assertionName << ": PASSED" << endl;
  }
}

void assertPointVector(vector<Point> a, vector<Point> b, string assertionName = "Unnamed assertion") {
  bool passed = true;
  if (a.size() != b.size()) {
    passed = false;
    cerr << assertionName << ": assertion vectors not same size" << endl;
    return;
  }
  for (int i = 0; i < a.size(); i++) {
    if (a[i] != b[i]) {
      cerr << assertionName << ':' << endl;
      cerr << "Expected point: " << b[i].toString() << " at index " << i << endl;
      cerr << "Received point: " << a[i].toString() << endl;
      passed = false;
    }
  }
  if (passed) {
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

struct PairingsTest {
  vector<Pairing> pairings;
  vector<EndLine> expectedEndLines;
};

int main() {
  Client client(vector<Dancer>(), vector<Point>(), 0, 0, 0);
  assertPoint(Point(8, 8) / 4, Point(2, 2), "Point integer divison");
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
  vector<Point> spiralTests = {
    {0, 0},
    {-1, 1},
    {0, 1},
    {1, 1},
    {1, 0},
    {1, -1},
    {0, -1},
    {-1, -1},
    {-1, 0},
    {-2, 2},
    {-1, 2},
    {0, 2},
    {1, 2},
    {2, 2},
    {2, 1},
    {2, 0},
    {2, -1},
    {2, -2},
    {1, -2},
    {0, -2},
    {-1, -2},
    {-2, -2},
    {-2, -1},
    {-2, 0},
    {-2, 1},
    {-3, 3}
  };
  int testNum = 1;
  SpiralIterator it(0, 0);
  for (Point expectedValue : spiralTests) {
    string testName = "Spiral test for point number " + to_string(testNum);
    assertPoint(it.getNext(), expectedValue, testName);
    testNum++;
  }
  // Pairings to positions test
  client.stars.clear();
  client.serverBoardSize = 40;
  client.stars = {{12, 2}, {20, 2}, {22, 3}};
  vector<PairingsTest> pairingsTests = {
    {
      {
        {
          {{0, 0}, {0, 4}, {4, 4}, {4, 0}}
        },
        {
          {{10, 10}, {10, 18}, {18, 18}, {18, 10}}
        }
      },
      {
        {{0, 2}, {3, 2}},
        {{12, 14}, {15, 14}},
      }
    },
    {
      {
        {
          {{10, 0}, {10, 4}, {14, 4}, {14, 0}}
        }
      },
      {
        {{9, 3}, {12, 3}},
      }
    },
    {
      {
        {
          {{11, 0}, {11, 4}, {15, 4}, {15, 0}}
        }
      },
      {
        {{13, 0}, {13, 3}},
      }
    },
    { // This test has unrealistic star placement but it's just to simulate other pairs taking up these positions
      {
        {
          {{20, 0}, {20, 4}, {24, 4}, {24, 0}}
        }
      },
      {
        {{21, 1}, {21, 4}},
      }
    }
  };
  testNum = 1;
  for (PairingsTest curTest : pairingsTests) {
    SolutionSpec sol = pairingsToPositions(&client, curTest.pairings);
    vector<Point> actualOutput;
    for (EndLine e : sol.finalConfiguration) {
      actualOutput.push_back(e.start);
      actualOutput.push_back(e.end);
    }
    vector<Point> expectedOutput;
    for (EndLine e : curTest.expectedEndLines) {
      expectedOutput.push_back(e.start);
      expectedOutput.push_back(e.end);
    }
    assertPointVector(actualOutput, expectedOutput, "pairingsToPositions test number " + to_string(testNum));
    testNum++;
  }
  cout << "Tests Finished" << endl;
}
