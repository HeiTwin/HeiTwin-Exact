#ifndef REDDEGLIMIT_RANDOM_HH
#define REDDEGLIMIT_RANDOM_HH

#include <algorithm>
#include <deque>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#include "../data_structures/graph.hh"

class RedDegLimitRandomReduction {
public:
  static std::string name() { return "RedDegreeLimitRandomReduction"; }

  /**
   * @brief Contracts all node pairs, that do not lead to a red degree higher than the limit
   *
   * @param g a graph
   */
  static void reduce(graph& g, bool exhaustively = false, int limit = 0, int node_limit = 0) {
    std::random_device rd;
    std::mt19937 gen(rd());

    if (!node_limit) node_limit = g.n;
    // default: current twin width
    if (limit == 0) limit = g.twin_width;

    std::vector<std::pair<node*, node*>> candidates;

    unsigned int old_n = 0;
    do {
      for (node* u = g.get_first_node(); u; u = u->next) {
        for (node* v = u->next; v; v = v->next) {
          if (g.red_degree_in_limit(u, v, limit)) {
            candidates.push_back({u, v});
            // g.contract(u->id, v->id);
            // --node_limit;
            // if (!node_limit) return;
          }
        }
      }

      old_n = g.n;

      std::shuffle(candidates.begin(), candidates.end(), gen);

      for (auto&& pair : candidates) {
        if (pair.first->active && pair.second->active && g.red_degree_in_limit(pair.first, pair.second, limit)) {
          g.contract(pair.first->id, pair.second->id);
        }
      }

    } while (exhaustively && old_n != g.n);
  }
};

#endif
