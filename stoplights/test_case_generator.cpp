#include <iostream>
#include <random>
#include <set>
#include <utility>
#include <algorithm>

using namespace std;

int main() {
  default_random_engine generator;
  uniform_int_distribution<int> numNodesDist(180, 200);
  int numNodes = numNodesDist(generator);
  int maxNumEdges = (numNodes * (numNodes - 1)) / 2;
  uniform_int_distribution<int> numEdgesDist(min((int)(0.9*maxNumEdges), 9500), min((int)(0.95*maxNumEdges), 10*1000));
  int numEdges = numEdgesDist(generator);
  uniform_int_distribution<int> nodeNameDist(1, numNodes);
  uniform_int_distribution<int> traverseTimeDist(1, 100);
  uniform_int_distribution<int> colorNameDist(0, 9);

  set<pair<int, int> > edgeSeen;

  cout << "node1 node2 color traversetime\n";
  for (int i = 0; i < numEdges; i++) {
    int sourceNode = nodeNameDist(generator);
    int targetNode = nodeNameDist(generator);
    while (targetNode == sourceNode || edgeSeen.count(make_pair(sourceNode, targetNode))) {
      sourceNode = nodeNameDist(generator);
      targetNode = nodeNameDist(generator);
    }
    edgeSeen.insert(make_pair(sourceNode, targetNode));
    edgeSeen.insert(make_pair(targetNode, sourceNode));

    cout << 'n' << sourceNode << " n" << targetNode << " c" << colorNameDist(generator) << ' ' << traverseTimeDist(generator) << '\n';
  }
  cout << "    \n";
  cout << "color greentime redtime\n";
  for (int i = 0; i < 10; i++) {
    cout << 'c' << i << ' ' << traverseTimeDist(generator) << ' ' << traverseTimeDist(generator) << '\n';
  }
  cout << "    \n";
}

