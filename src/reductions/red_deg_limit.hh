#ifndef REDDEGLIMIT_HH
#define REDDEGLIMIT_HH

#include <algorithm>
#include <deque>
#include <iostream>
#include <numeric>
#include <vector>

#include "../data_structures/graph.hh"

class RedDegLimitReduction {
public:
  static std::string name() { return "RedDegreeLimitReduction"; }

  /**
   * @brief Contracts all node pairs, that do not lead to a red degree higher than the limit
   *
   * @param g a graph
   */
  static void reduce(graph& g, bool exhaustively = false, int limit = 0, int node_limit = 0) {
    if (!node_limit) node_limit = g.n;
    // default: current twin width
    if (limit == 0) limit = g.twin_width;

    unsigned int old_n = 0;
    do {
      old_n = g.n;
      for (node* u = g.get_first_node(); u; u = u->next) {
        for (node* v = u->next; v; v = v->next) {
          if (g.red_degree_in_limit(u, v, limit)) {
            g.contract(u->id, v->id);
            --node_limit;
            if (!node_limit) return;
          }
        }
      }
    } while (exhaustively && old_n != g.n);
  }
};

#endif
