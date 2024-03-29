#include "../random/random.h"

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

struct Package {
  int p;
  int v;
};

struct CompRelationship {
  Package p1;
  Package p2;
};

bool adjacent[25][45][25][45]; // this is automatically initialized to 0
bool largeUsed[25][45];

int generateSmall(int numP, int numV, int numC, int maximalConfigVersion, vector<CompRelationship> &relationships) {
  int numAdded = 0;
  Random r;
  int sizeOfSubset = r.randInt(numP >> 2, 3 * (numP >> 2)); // This could both be changed to be deterministic or change the range of the random numbers
  // Make a bitmask and shuffle it randomly to choose which packages to keep
  vector<bool> bitmask(numP + 1, false);
  for (int i = 1; i <= sizeOfSubset; i++) {
    bitmask[i] = true;
  }
  shuffle(bitmask.begin() + 1, bitmask.end(), r.generator);
  // Decide all the new version numbers for the following configuration
  vector<int> smallVersions(numP + 1, maximalConfigVersion);
  for (int i = 1; i < bitmask.size(); i++) {
    if (!bitmask[i]) {
      int version = r.randInt(1, maximalConfigVersion); // Constants here can also be changed
      smallVersions[i] = version;
    }
  }
  // Add compatibility relationships
  for (int i = 1; relationships.size() < numC && i < bitmask.size(); i++) {
    if (!bitmask[i]) {
      // This package is not in the subset
      for (int j = 1; relationships.size() < numC && j <= numP; j++) {
        if (j <= i && !bitmask[j]) continue; // We already handled this in an earlier iteration
        if (!adjacent[i][smallVersions[i]][j][smallVersions[j]]) {
          relationships.push_back({{i, smallVersions[i]}, {j, smallVersions[j]}});
          adjacent[i][smallVersions[i]][j][smallVersions[j]] = true;
          adjacent[i][smallVersions[j]][i][smallVersions[j]] = true;
          numAdded++;
        }
      }
    }
  }
  return numAdded;
}

int generateLarge(int numP, int numV, int numC, int maximalConfigVersion, vector<CompRelationship> &relationships) {
  int numAdded = 0;
  Random r;
  int sizeOfSubset = numP / 2; // This could both be changed to be deterministic or change the range of the random numbers
  // Make a bitmask and shuffle it randomly to choose which packages to keep
  vector<bool> bitmask(numP + 1, false);
  for (int i = 1; i <= sizeOfSubset; i++) {
    bitmask[i] = true;
  }
  shuffle(bitmask.begin() + 1, bitmask.end(), r.generator);
  // Decide all the new version numbers for the following configuration
  vector<int> largeVersions(numP + 1, maximalConfigVersion);
  for (int i = 1; i < bitmask.size(); i++) {
    if (!bitmask[i]) {
      // check if version has been exhausted
      int k;
      for (k = maximalConfigVersion + 1; k <= numV; k++) {
        if (!largeUsed[bitmask[i]][k]) {
          break;
        }
      }
      if (k == numV + 1) {
        bitmask[i] = true;
        continue;
      }
      // generate randomly a new version
      largeUsed[bitmask[i]][k] = true;
      largeVersions[i] = k;
    }
  }
  int ignoredPackage = r.randInt(1, numP);
  // Add compatibility relationships
  for (int i = 1; relationships.size() < numC && i < bitmask.size(); i++) {
    if (i == ignoredPackage) continue;
    if (!bitmask[i]) {
      // This package is not in the subset
      for (int j = 1; relationships.size() < numC && j <= numP; j++) {
        if (j == ignoredPackage || (j <= i && !bitmask[j])) continue; // We already handled this in an earlier iteration
        if (!adjacent[i][largeVersions[i]][j][largeVersions[j]]) {
          relationships.push_back({{i, largeVersions[i]}, {j, largeVersions[j]}});
          adjacent[i][largeVersions[i]][j][largeVersions[j]] = true;
          numAdded++;
        }
      }
    }
  }
  return numAdded;
}

int main() {
  int numP, numV, numC;
  cin >> numP >> numV >> numC;
  // Since no one knows our strategy we can just as well make this deterministic as no one can take advantage of it
  int maximalConfigVersion = numV / 3; // Guyu I'm not sure if you want to maybe divide this by 4 instead of 2 or something in regard to your thoughts about mid-low range but I'm not 100% sure I understand completely what you're talking about
  vector<CompRelationship> relationships;
  relationships.reserve(numC);

  // Add the initially needed compatibility relationships
  for (int packageIndex1 = 1; packageIndex1 <= numP; packageIndex1++) {
    for (int packageIndex2 = packageIndex1 + 1; packageIndex2 <= numP; packageIndex2++) {
      relationships.push_back({{packageIndex1, maximalConfigVersion}, {packageIndex2, maximalConfigVersion}});
    }
  }
  // I'm purposefully not putting IFDEF DEBUG here as this code won't be in need of high performance
  if (relationships.size() > numC) {
    cerr << "The K input given wasn't big enough to even make one config" << endl;
  }

  // These could possibly be put in a loop if handled properly
  while (relationships.size() < numC) {
    if (generateLarge(numP, numV, numC, maximalConfigVersion, relationships) == 0) {
      break;
    }
  }

  while (relationships.size() < numC) {
    if (generateSmall(numP, numV, numC, maximalConfigVersion, relationships) == 0) {
      break;
    }
  }


  // Now we output all our relationships
  cout << relationships.size() << endl;
  for (CompRelationship cur : relationships) {
    cout << cur.p1.p << ' ' << cur.p1.v << ' ' << cur.p2.p << ' ' << cur.p2.v << endl;
  }
  // And then output our maximal configuration
  cout << 1 << endl;
  for (int i = 1; i <= numP; i++) {
    cout << maximalConfigVersion;
    if (i != numP) cout << ' ';
  }
  // cout << endl;
}
