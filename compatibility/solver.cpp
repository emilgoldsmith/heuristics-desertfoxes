#include "../random/random.h"
#include "../timer/timer.h"

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

struct Package {
  int p;
  int v;
};

bool adjacent[25][45][25][45]; // this is automatically initialized to 0
int numP, numV, numC;

double getCost(vector<Package> config) {
  int versionSum = 0;
  for (Package pkg : config) {
    versionSum += pkg.v;
  }
  return versionSum;
}

vector<Package> hillClimb(vector<Package> source) {
  Random r;
  vector<int> packageCheckOrder(numP, 0);
  for (int i = 0; i < numP; i++) {
    packageCheckOrder[i] = i + 1;
  }
  while (true) {
    // Determine the orders that we try finding a neighbor in
    shuffle(packageCheckOrder.begin(), packageCheckOrder.end(), r.generator);

    bool validSwap;
    for (int i : packageCheckOrder) {
      int p = source[i - 1].p;
      for (int v = source[i - 1].v + 1; v <= numV; v++) {
        validSwap = true;
        for (Package pkg : source) {
          if (pkg.p != p) {
            validSwap = validSwap && adjacent[p][v][pkg.p][pkg.v];
            if (!validSwap) break;
          }
        }
        if (validSwap) {
          source[i-1] = {p, v};
          break;
        }
      }
    }
    if (!validSwap) break;
  }
  return source;
}

vector<int> packageCheckOrder;
vector<Package> dfs(vector<Package> currentConfig) {
  if (currentConfig.size() == numP) return currentConfig;
  int p = packageCheckOrder[currentConfig.size()];
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

  for (int i = 0; i < numP; i++) {
    packageCheckOrder.push_back(i + 1);
  }
  vector<Package> bestConfig;
  int bestCost = 0;
  Random r;
  for (int i = 0; i < 100; i++) {
    shuffle(packageCheckOrder.begin(), packageCheckOrder.end(), r.generator);
    vector<Package> initialConfig;
    vector<Package> candidateConfig = hillClimb(dfs(initialConfig));
    int candidateCost = getCost(candidateConfig);
    if (candidateCost > bestCost) {
      bestCost = candidateCost;
      bestConfig = candidateConfig;
    }
  }

  // Output answer
  sort(bestConfig.begin(), bestConfig.end(),
      [](const Package &a, const Package &b) -> bool {return a.p < b.p;});
  cout << 1 << endl;
  for (int i = 1; i <= numP; i++) {
    if (bestConfig[i-1].p != i) {
      cerr << "ERROR: bestConfig isn't sorted!" << endl;
    }
    cout << bestConfig[i-1].v;
    if (i != numP) cout << ' ';
  }
  // cout << endl;
}
