#ifndef DEGONE_HH
#define DEGONE_HH

#include <algorithm>
#include <deque>
#include <iostream>
#include <numeric>
#include <vector>

#include "../data_structures/graph.hh"

class DegreeOneReduction {
public:
  static std::string name() { return "DegreeOneReduction"; }

  /**
   * @brief Applies the degree one reduction to a given graph
   *
   * @param g a graph
   */
  static void reduce(graph& g) {
    // one deg one neighbor for each node
    std::vector<unsigned int> deg_one_neighbors;
    deg_one_neighbors.resize(g.adj_list.size(), 0);

    for (node* nd = g.get_first_node(); nd; nd = nd->next) {
      if (nd->deg == 1) {
        unsigned int target = nd->first->target;
        if (deg_one_neighbors[target]) {
          g.contract(nd->id, deg_one_neighbors[target]);
          deg_one_neighbors[target] = std::min(nd->id, deg_one_neighbors[target]);
        } else {
          deg_one_neighbors[target] = nd->id;
        }
      }
    }
  }
};

#endif
