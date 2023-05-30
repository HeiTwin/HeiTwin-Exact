#ifndef ZERODEG_HH
#define ZERODEG_HH

#include <algorithm>
#include <deque>
#include <iostream>
#include <numeric>
#include <vector>

#include "../data_structures/graph.hh"

class DegreeZeroReduction {
public:
  static std::string name() { return "DegreeZeroReduction"; }
  
  /**
   * @brief Applies the zero degree reduction to a given graph
   *
   * @param g a graph
   * @return int the id of the remaining node of degree zero, or 0
   */
  static int reduce(graph& g) {
    int z = 0;
    for (node* current_node = g.get_first_node(); current_node; current_node = current_node->next) {
      if (current_node->deg) continue;
      if (z) {
        g.contract(z, current_node->id);
      } else {
        z = current_node->id;
      }
    }
    return z;
  }
};

#endif
