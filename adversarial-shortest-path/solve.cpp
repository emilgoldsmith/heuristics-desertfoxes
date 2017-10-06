#include "move.h"

#include <vector>

using namespace std;

Move getTraverseMove(vector<vector<int> > *adjacencyList) {
  return {
    0,
    (*adjacencyList)[0][0],
    0
  };
}

Move getAdversaryMove(vector<vector<int> > *adjacencyList) {
  return {
    0,
    (*adjacencyList)[0][0],
    1.5
  };
}
