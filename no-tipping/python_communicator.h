#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <string>

void init(std::string command);
std::string readMsg();
void writeMsg();
void teardown();

#endif
