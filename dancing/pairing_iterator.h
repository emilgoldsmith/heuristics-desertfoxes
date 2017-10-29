#ifndef PAIRING_ITERATOR_H
#define PAIRING_ITERATOR_H

#include "client.h"
#include "structs.h"
#include "../random/random.h"

#include <vector>

struct SortedDistDancer {
  std::vector<std::vector<Point>> sortedOtherDancers;
};

class PairingIterator {
  std::vector<SortedDistDancer> sortedDancers;
  const Client *client;
  std::vector<int> dancerOrdering;
  Random* r;

  public:
    PairingIterator(Client *inputClient);
    ~PairingIterator();
    std::vector<Pairing> getNext();
};

#endif
