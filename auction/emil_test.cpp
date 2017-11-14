#include "solver.h"

#include <iostream>

using namespace std;

int main() {
  vector<int> itemsInAuction = {2, 0, 0, 1, 0, 2, 2, 1, 2, 0, 0, 2, 0, 2, 1, 1, 2, 0, 0, 2, 0, 2, 1, 1, 2, 1, 2, 1, 0, 1, 1, 1, 0, 2, 1, 1, 2, 0, 1, 1, 1, 2, 2, 1, 1, 0, 1, 1, 2, 2, 1, 0, 2, 2, 0, 2, 0, 0, 2, 0, 0, 1, 1, 0, 1, 0, 1, 2, 1, 0, 2, 1, 2, 2, 2, 1, 1, 2, 0, 0, 0, 1, 1, 0, 0, 0, 2, 1, 1, 2, 1, 2, 2, 2, 0, 0, 2, 1, 1, 0};
  // vector<int> itemsInAuction = {0, 0, 0, 0, 0, 0, 0};
  Solver solver(3, 5, itemsInAuction, 100, 4);
  cout << "Totalpaintings:" << endl;
  for (int num : solver.totalPaintings) {
    cout << num << ' ';
  }
  cout << endl;
  cout << "First 15 items:" << endl;
  for (int i = 0; i < 15; i++) {
    cout << itemsInAuction[i] << endl;
  }
  cout << endl;
  cout << "First round we would've bid " << solver.getBid() << endl;
  cout << "Now we win each painting with a bid of 1 each time and see what we would've bid" << endl;
  for (int i = 0; i < 15; i++) {
    solver.updateState(1, 1);
    cout << "Round " << (i + 1) << ": " << solver.getBid() << endl;
  }
  Solver solver2(3, 5, itemsInAuction, 100, 2);
  for (int i = 0; i < 10; i++) {
    solver2.updateState(1, 2);
  }
  cout << "If player 1 can win and has 80 left we would bid " << solver2.getBid() << endl;
}
