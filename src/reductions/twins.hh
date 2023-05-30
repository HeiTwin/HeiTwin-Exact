#ifndef TWINS_HH
#define TWINS_HH

#include <algorithm>
#include <deque>
#include <iostream>
#include <numeric>
#include <vector>

#include "../data_structures/graph.hh"
#include "degone.hh"
#include "degzero.hh"


#include "../utils.hh"

class TwinReduction {
public:
  static std::string name() { return "TwinReduction"; }

  /**
   * @brief Applies the twin reduction to a given graph
   *
   * @param g a graph
   * @param max_degree the maximum degree that should be checked
   * @return int the id of the remaining node of degree zero, or 0
   */
  static void reduce(graph &g, int max_degree = 0, bool fast = false) {
    if (!max_degree) max_degree = g.n;
    int twins = 0;
    std::vector<unsigned int> checked;
    checked.resize(g.adj_list.size(), 0);
    for (node *u = g.get_first_node(); u; u = u->next) {
      if (!u->deg || u->deg > max_degree || u->num_red) continue;
      if (fast) {
        // Only check 2-neighborhood
        for (edge &&e : *u) {
          for (edge &&target : g.adj_list[e.target]) {
            if (checked[target.target] < u->id) {
              checked[target.target] = u->id;
              if (target.target <= u->id) continue;
              node *v = &g.adj_list[target.target];
              if (u->deg != v->deg) continue;
              if (!are_twins(u, v, g)) continue;
              g.contract(u->id, v->id);
              ++twins;
              goto skip_node;
            }
          }
        }
      skip_node:;
      } else {
        // Check all node pairs
        for (node *v = u->next; v; v = v->next) {
          // if (v->num_red) continue;
          if (u->deg != v->deg) continue;
          if (!are_twins(u, v, g)) continue;
          g.contract(u->id, v->id);
          ++twins;
          break;
        }
      }
    }
    if (fast) {
      // apply degree 0 and 1 reduction if only the 2-neighborhood was checked
      ::reduce<DegreeZeroReduction>(g);
      ::reduce<DegreeOneReduction>(g);
    }
  }

private:
  static bool are_twins(node *u, node *v, const graph &g) {
    edge *u_ptr{u->first};
    edge *v_ptr{v->first};
    while (u_ptr->is_edge && v_ptr->is_edge) {
      if (u_ptr->target == v_ptr->target) {
        if (u_ptr->red != v_ptr->red) return false;
        u_ptr = u_ptr->next;
        v_ptr = v_ptr->next;
      } else if (u_ptr->target == v->id) {
        u_ptr = u_ptr->next;
      } else if (v_ptr->target == u->id) {
        v_ptr = v_ptr->next;
      } else {
        return false;
      }
    }
    while (u_ptr->is_edge) {
      if (u_ptr->target != v->id) return false;
      u_ptr = u_ptr->next;
    }
    while (v_ptr->is_edge) {
      if (v_ptr->target != u->id) return false;
      v_ptr = v_ptr->next;
    }
    return true;
  }
};

#endif
