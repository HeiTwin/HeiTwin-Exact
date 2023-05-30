#ifndef TREECONTRACT_HH
#define TREECONTRACT_HH

#include "../data_structures/graph.hh"
#include "branch_and_bound.hh"

/**
 * @brief Contract the remaining graph as fast as possible
 *        in praxis node ids that are close to each other are more likely to be connected,
 *        so this solver contracts them pairwise in a binary tree matter
 */
class TreeContract {
public:
  static std::string name() { return "TreeContract"; }

  static void solve(graph& g) {
    int n = g.n;

    node *next_node = g.get_first_node(), *partner, *new_next;
    while (g.n > 1) {
      partner = next_node->next;
      if (!partner) {
        next_node = g.get_first_node();
        partner = next_node->next;
      }
      new_next = partner->next;
      g.contract(next_node->id, partner->id);
      next_node = new_next;
      if (!next_node) next_node = g.get_first_node();
    }
  }
};

#endif /* end of include guard: TREECONTRACT_HH */
