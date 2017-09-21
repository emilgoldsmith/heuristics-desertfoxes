#include "parser.h"

#include <iostream>
#include <string>

using namespace std;

int main() {
  string line, total = "";
  while (getline(cin, line)) {
    total += line + "\n";
  }
  parse(total);
}
