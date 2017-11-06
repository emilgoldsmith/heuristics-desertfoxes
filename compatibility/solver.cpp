#include "../random/random.h"

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

struct Package {
  int p;
  int v;
};

bool adjacent[25][45][25][45]; // this is automatically initialized to 0

/*
vector<Pairing> getSANeighbour(vector<Pairing> source) {
  Random r;
  int firstIndex = r.randInt(0, source.size() - 1);
  int secondIndex = r.randInt(0, source.size() - 1);
  while (secondIndex == firstIndex) {
    secondIndex = r.randInt(0, source.size() - 1);
  }
  int colorToSwap = r.randInt(0, source[0].dancers.size() - 1);
  swap(source[firstIndex].dancers[colorToSwap], source[secondIndex].dancers[colorToSwap]);
  return source;
}

double getSACost(vector<Pairing> pairings) {
  int maxDist = -1;
  for (Pairing curPairing : pairings) {
    Point center = computeCenterBruteforce(curPairing.dancers);
    for (Point curPoint : curPairing.dancers) {
      maxDist = max(maxDist, manDist(center, curPoint));
    }
  }
  return maxDist;
}
*/

int numP, numV, numC;
vector<Package> dfs(vector<Package> currentConfig) {
  if (currentConfig.size() == numP) return currentConfig;
  int p = currentConfig.back().p + 1;
  for (int v = numV; v > 0; v--) {
    bool allValid = true;
    for (Package curPkg : currentConfig) {
      allValid = allValid && adjacent[p][v][curPkg.p][curPkg.v];
      if (!allValid) break;
    }
    if (allValid) {
      vector<Package> configCopy(currentConfig);
      configCopy.push_back({p, v});
      vector<Package> candidateConfig = dfs(configCopy);
      if (candidateConfig.size() > 0) return candidateConfig;
    }
  }
  return vector<Package>();
}


int main() {
  // First read input
  cin >> numP >> numV >> numC;
  for (int i = 0; i < numC; i++) {
    int p1, v1, p2, v2;
    cin >> p1 >> v1 >> p2 >> v2;
    adjacent[p1][v1][p2][v2] = adjacent[p2][v2][p1][v1] = true;
  }

  vector<Package> bestConfig;
  for (int v = numV; v > 0; v--) {
    vector<Package> initialConfig = {{1, v}};
    bestConfig = dfs(initialConfig);
    if (bestConfig.size() > 0) break;
  }

  // Output answer
  cout << 1 << endl;
  for (int i = 1; i <= numP; i++) {
    if (bestConfig[i-1].p != i) {
      cerr << "ERROR: bestConfig isn't sorted!" << endl;
    }
    cout << bestConfig[i-1].v;
    if (i != numP) cout << ' ';
  }
  cout << endl;

  /*
  Random r;
  vector<Pairing> oldPairing = it.getNext();
  double oldCost = getSACost(oldPairing);
  vector<Pairing> bestPairing = oldPairing;
  double bestCost = oldCost;
  double T = 1000.0;
  double T_min = 0.00001;
  double alpha = 0.945;
  while (T > T_min) {
    for (int i = 0; i < 100; i++) {
      vector<Pairing> newPairing = getSANeighbour(oldPairing);
      double newCost = getSACost(newPairing);
      double acceptanceProbability = exp((oldCost - newCost)/T);
      if (acceptanceProbability > r.randDouble(0, 1)) {
        oldPairing = newPairing;
        oldCost = newCost;
        if (oldCost < bestCost) {
          bestCost = oldCost;
          bestPairing = oldPairing;
        }
      }
    }
    T *= alpha;
  }
  */
}
