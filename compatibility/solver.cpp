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
int numP, numV, numC;

vector<Package> getSANeighbour(vector<Package> source) {
  Random r;
  // Determine the orders that we try finding a neighbor in
  vector<int> packageCheckOrder(numV, 0);
  for (int i = 0; i < numP; i++) {
    packageCheckOrder[i] = i + 1;
  }
  shuffle(packageCheckOrder.begin(), packageCheckOrder.end(), r.generator);

  vector<int> versionCheckOrder(numV, 0);
  for (int i = 0; i < numV; i++) {
    versionCheckOrder[i] = i + 1;
  }

  for (int p : packageCheckOrder) {
    shuffle(versionCheckOrder.begin(), versionCheckOrder.end(), r.generator);
    for (int v : versionCheckOrder) {
      if (v == source[p - 1].v) continue;
      bool validSwap = true;
      for (Package pkg : source) {
        if (pkg.p != p) {
          validSwap = validSwap && adjacent[p][v][pkg.p][pkg.v];
          if (!validSwap) break;
        }
      }
      if (validSwap) {
        source[p-1] = {p, v};
        return source;
      }
    }
  }
  cerr << "Reached the end of getSANeighbour! Not intended!" << endl;
  return source;
}

double getSACost(vector<Package> config) {
  int versionSum = 0;
  for (Package pkg : config) {
    versionSum += pkg.v;
  }
  return versionSum;
}

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

  Random r;
  vector<Package> oldConfig = bestConfig;
  double oldCost = getSACost(oldConfig);
  double bestCost = oldCost;
  double T = 1000.0;
  double T_min = 0.00001;
  double alpha = 0.945;
  while (T > T_min) {
    for (int i = 0; i < 100; i++) {
      vector<Package> newConfig = getSANeighbour(oldConfig);
      double newCost = getSACost(newConfig);
      double acceptanceProbability = exp((newCost - oldCost)/T); // We try to maximize instead of minimize
      if (acceptanceProbability > r.randDouble(0, 1)) {
        oldConfig = newConfig;
        oldCost = newCost;
        if (oldCost > bestCost) {
          bestCost = oldCost;
          bestConfig = oldConfig;
        }
      }
    }
    T *= alpha;
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

}
