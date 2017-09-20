#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void error(string err) {
  cerr << err << endl;
  exit(1);
}

void parse(string s) {
  stringstream ss;
  ss << s;
  // Handle first line
  string tmp;
  ss >> tmp;
  if (tmp != "node1") error("wrong first line format");
  ss >> tmp;
  if (tmp != "node2") error("wrong first line format");
  ss >> tmp;
  if (tmp != "color") error("wrong first line format");
  ss >> tmp;
  if (tmp != "traversetime") error("wrong first line format");

  string cur;
  ss >> cur;
  while (cur != "color") {
    string node1 = cur, node2, color, traversetime;
    ss >> node2 >> color >> traversetime;
    cout << node1 << ' ' << node2 << ' ' << color << ' ' << traversetime << endl;
    // Do something with this information
    // Prep for next while check
    ss >> cur;
  }
  ss >> cur;
  if (cur != "greentime") error("wrong color line format");
  ss >> cur;
  if (cur != "redtime") error("wrong color line format");
  ss >> cur;
  cout << "color" << endl;
  while (!ss.eof()) {
    string color = cur, greentime, redtime;
    ss >> greentime >> redtime;
    cout << color << ' ' << greentime << ' ' << redtime << endl;
    // Do something with them
    // Prep for next while check
    ss >> cur;
  }
}

int main() {
  string line, total = "";
  while (getline(cin, line)) {
    total += line + "\n";
  }
  parse(total);
}


