// Header guard as read on learncpp.com
#ifndef ALGORITHM_H
#define ALGORITHM_H

// Call this before calling getMove to precompute the dp matrix
void initialize();

// shouldIReset is passed by reference to act as a second return value
int getMove(int curStones, int curMax, int nextResets, int prevResets, int currentlyReset, bool &shouldIReset);

#endif
