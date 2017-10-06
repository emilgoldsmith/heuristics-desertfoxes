#ifndef SOLVE_H
#define SOLVE_H

#include "move.h"

#include <vector>

Move getTraverseMove(std::vector<std::vector<int> > *adjacencyList);
Move getAdversaryMove(std::vector<std::vector<int> > *adjacencyList);

#endif
