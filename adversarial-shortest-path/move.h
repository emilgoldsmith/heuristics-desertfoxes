#ifndef MOVE_H
#define MOVE_H

struct Move {
  int node1; // This can either mean arbitrary node or start node depending on context
  int node2; // This can either mean arbitrary node or end node depending on context
  double costRelatedInfo; // This can either mean multiplication factor, updated cost of edge, or new total cost of current traversal depending on context
};

#endif
