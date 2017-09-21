// header guard
#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>

struct graph_edge {
  int target;
  int greentime;
  int redtime;
};

std::vector<std::vector<graph_edge> > parse(std::string s);

#endif
