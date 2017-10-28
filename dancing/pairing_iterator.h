#ifndef PAIRING_ITERATOR_H
#define PAIRING_ITERATOR_H

#include "client.h"
#include "structs.h"

#include <vector>

struct SortedDistDancer {
  std::vector<std::vector<Point>> sortedOtherDancers;
};

class PairingIterator {
  std::vector<SortedDistDancer> sortedDancers;
  const Client *client;

  public:
    PairingIterator(Client *inputClient);
    std::vector<Pairing> getNext();
};

#endif
