#ifndef CONTRACTION_ENUMERATION_HH
#define CONTRACTION_ENUMERATION_HH

#include <list>
#include <stack>
#include <vector>

#include "graph.hh"

class ContractionEnumeration {
public:
  // store next contraction at each level
  std::vector<std::pair<node*, node*>> next_contractions;
  // store if pairs are independent at each level
  int level = 0;
  int depth;
  int n;

  ContractionEnumeration(graph& g, int _depth) : depth(_depth) {
    next_contractions.resize(g.n, std::pair<node*, node*>{g.get_first_node(), g.get_first_node()});
    n = g.n;
  }

  void skip_branch(graph& g) {
    g.uncontract();
    level--;
  }

  bool do_next_contraction(graph& g, int current_best) {
    bool contracted = false;
    while (!contracted) {
      // backtrack until there is a next contraction
      while (!increment_contraction(next_contractions[level], g)) {
        if (g.n < n) {
          // backtrack
          g.uncontract();
          --level;
        } else {
          // finished
          return false;
        }
      }
      if (g.red_degree_in_limit(next_contractions[level].first, next_contractions[level].second, current_best)) {
        // break symmetry
        // if 1. not first contraction in graph, 2. next contraction > previous contraction, 3. contractions are
        if (level > 0 && next_contractions[level].first->id > next_contractions[level - 1].first->id &&
            g.contractions_independent(next_contractions[level], next_contractions[level - 1])) {
          // independent, skip
        } else {
          g.contract(next_contractions[level].first->id, next_contractions[level].second->id);
          ++level;
          next_contractions[level] = std::pair<node*, node*>{g.get_first_node(), g.get_first_node()};
          contracted = true;
        }
      }
    }
    return true;
  }

private:
  bool increment_contraction(std::pair<node*, node*>& contraction, graph& g) {
    // depth limit
    if (depth && level == depth) return false;
    if (g.n <= 1) {
      // leaf
      return false;
    }
    if (contraction.first == contraction.second) {
      // special case: first contraction in level
      contraction.second = contraction.second->next;
      return true;
    }
    if (contraction.second->next) {
      // increment second node
      contraction.second = contraction.second->next;
      return true;
    } else {
      // second node is last node, increment first one
      contraction.first = contraction.first->next;
      if (!contraction.first->next) {
        return false;
      }
      contraction.second = contraction.first->next;
      return true;
    }
  }
};

#endif
