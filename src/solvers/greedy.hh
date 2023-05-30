#ifndef GREEDY_HH
#define GREEDY_HH

#include <climits>

#include "../data_structures/graph.hh"

// TODO: this greedy strategy is shit, needs improvement
class Greedy {
public:
  static std::string name() { return "Greedy"; }

  static void solve(graph &g, unsigned int depth = 0) {
    if (!depth) depth = g.n;
    int n = g.n, contractions = 0;
    while (g.n >= 2) {
      std::pair<unsigned int, unsigned int> best_nodes = find_node_most_common_neighbors(g);
      g.contract(best_nodes.first, best_nodes.second);
      ++contractions;
      if (contractions == depth) break;
    }
  }

private:
  static int red_degree_delta(node const *u, node const *v) {
    int red_deg_delta = 0;

    edge *u_ptr{u->first};
    edge *v_ptr{v->first};

    while (u_ptr->is_edge && v_ptr->is_edge) {
      if (u_ptr->target == v_ptr->target) {
        if (u_ptr->red) --red_deg_delta;
        u_ptr = u_ptr->next;
        v_ptr = v_ptr->next;
      } else if (u_ptr->target < v_ptr->target) {
        ++red_deg_delta;
        u_ptr = u_ptr->next;
      } else {
        ++red_deg_delta;
        v_ptr = v_ptr->next;
      }
    }
    while (u_ptr->is_edge) {
      u_ptr = u_ptr->next;
      ++red_deg_delta;
    }
    while (v_ptr->is_edge) {
      v_ptr = v_ptr->next;
      ++red_deg_delta;
    }
    return red_deg_delta;
  }

  static std::pair<unsigned int, unsigned int> find_node_most_common_neighbors(graph &g) {
    int min = INT_MAX;
    node *best_u, *best_v;
    for (node *u = g.get_first_node(); u; u = u->next) {
      for (node *v = u->next; v; v = v->next) {
        int delta = red_degree_delta(u, v);
        if (delta < min) {
          min = delta;
          best_u = u;
          best_v = v;
        }
      }
    }
    return {best_u->id, best_v->id};
  }
};

#endif /* end of include guard: GREEDY_HH */
