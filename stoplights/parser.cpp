#include "parser.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct edge {
  int node1;
  int node2;
  int color;
  int traversetime;
};

vector<edge> edges;

struct color {
  int color;
  int greentime;
  int redtime;
};

vector<color> colors;

void error(string err);

vector<vector<graph_edge> > createGraph(int maxNodeNumber);

void error(string err) {
  cerr << err << endl;
  exit(1);
}

vector<vector<graph_edge> > parse(string s, int &startIndex, int &endIndex) {
  stringstream ss;
  ss << s;
  // Get start and end node
  string startIndexString, endIndexString;
  ss >> startIndexString >> endIndexString;
  startIndexString = startIndexString.substr(1, string::npos);
  endIndexString = endIndexString.substr(1, string::npos);
  startIndex = stoi(startIndexString);
  endIndex = stoi(endIndexString);
  // Handle first text line
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
  int maxNodeNumber = 0;
  while (cur != "color") {
    string node1 = cur, node2, color, traversetime;
    ss >> node2 >> color >> traversetime;
    // Remove all the prefixes of n for nodes and c for colors
    node1 = node1.substr(1, string::npos);
    node2 = node2.substr(1, string::npos);
    color = color.substr(1, string::npos);
    struct edge currentEdge = {
      stoi(node1),
      stoi(node2),
      stoi(color),
      stoi(traversetime),
    };
    maxNodeNumber = max(maxNodeNumber, max(stoi(node1), stoi(node2)));
    edges.push_back(currentEdge);
    // Do something with this information
    // Prep for next while check
    ss >> cur;
  }
  ss >> cur;
  if (cur != "greentime") error("wrong color line format");
  ss >> cur;
  if (cur != "redtime") error("wrong color line format");
  ss >> cur;
  // We use this for checking that input is structured as expected
  int index = 0;
  while (!ss.eof()) {
    string color = cur, greentime, redtime;
    ss >> greentime >> redtime;
    // Remove all the prefixed cs for colors
    color = color.substr(1, string::npos);
    struct color currentColor = {
      stoi(color),
      stoi(greentime),
      stoi(redtime),
    };
    colors.push_back(currentColor);
    if (stoi(color) != (index++)) error("color ordering invalid");
    // Prep for next while check
    ss >> cur;
  }
  return createGraph(maxNodeNumber);
}

vector<vector<graph_edge> > createGraph(int maxNodeNumber) {
  vector<vector<graph_edge> > graph(maxNodeNumber + 1);
  for (struct edge currentEdge : edges) {
    struct color currentColor = colors[currentEdge.color];
    struct graph_edge equivalentGraphEdgeA = {
      currentEdge.node2,
      currentEdge.traversetime,
      currentColor.greentime,
      currentColor.redtime,
    };

    graph[currentEdge.node1].push_back(equivalentGraphEdgeA);

    struct graph_edge equivalentGraphEdgeB = {
      currentEdge.node1,
      currentEdge.traversetime,
      currentColor.greentime,
      currentColor.redtime,
    };

    graph[currentEdge.node2].push_back(equivalentGraphEdgeB);
  }
  return graph;
}
