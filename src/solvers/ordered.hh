#ifndef ORDERED_HH
#define ORDERED_HH

#include <chrono>

#include "../data_structures/graph.hh"

class Ordered {
public:
  static std::string name() { return "Ordered"; }

  static void solve(graph& g) {
    node* nd = g.get_first_node();
    for (node* next = nd->next; next; next = next->next) {
      g.contract(nd->id, next->id);
    }
  }
};

#endif /* end of include guard: ORDERED_HH */
