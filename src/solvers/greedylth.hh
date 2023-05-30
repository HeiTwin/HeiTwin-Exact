#ifndef GREEDYLTH_HH
#define GREEDYLTH_HH

#include <algorithm>
#include <limits>

#include "../data_structures/graph.hh"
#include "../reductions/degzero.hh"

class Greedylth {
public:
  static std::string name() { return "Greedylth"; }

  static void solve(graph &g) {
    // zero degree node reduction
    int zero_deg_node = DegreeZeroReduction::reduce(g);

    // // path reduction
    // PathReduction::reduce(g);

    // zero_deg_node = TwinReduction::reduce(g, 0, zero_deg_node);
    // zero_deg_node = TwinReduction::reduce(g, 0, zero_deg_node);

    int u, v, other_u, other_v;
    u = find_node_min_deg(g.adj_list);
    while ((other_u = find_node_min_deg(g.adj_list, u))) {
      v = find_node_fewest_new_red_neighbors(u, g);
      other_v = find_node_fewest_new_red_neighbors(other_u, g);
      if (new_red_edges(other_u, other_v, g) < new_red_edges(u, v, g)) {
        u = other_u;
        v = other_v;
      }
      if (v < u) std::swap(u, v);
      g.contract(u, v);
      // std::cout << "contracting " << u << ", " << v << std::endl;
      // contract_twins(g, solution);
      if (!g.adj_list[u].deg) {
        // contract deg zero nodes
        if (zero_deg_node) {
          if (u < zero_deg_node) std::swap(zero_deg_node, u);
          g.contract(zero_deg_node, u);
        } else {
          zero_deg_node = u;
        }
        u = find_node_min_deg(g.adj_list);
      }
    }
  }

private:
  static int find_node_min_deg(const std::vector<node> &nodes, int other = 0) {
    int node{}, min{std::numeric_limits<int>::max()};
    for (int i{1}; i < other; ++i) {
      if (!nodes[i].active) continue;
      if (nodes[i].deg >= min || !nodes[i].deg) continue;
      min = nodes[i].deg;
      node = i;
    }
    for (int i{other + 1}; i < nodes.size(); ++i) {
      if (!nodes[i].active) continue;
      if (nodes[i].deg >= min || !nodes[i].deg) continue;
      min = nodes[i].deg;
      node = i;
    }
    return node;
  }

  static int new_red_edges(int u, int v, const graph &g) {
    int new_red{};
    edge *u_ptr{g.adj_list[u].first};
    edge *v_ptr{g.adj_list[v].first};
    while (u_ptr->is_edge && v_ptr->is_edge) {
      if (u_ptr->target == v_ptr->target) {
        if (u_ptr->red && v_ptr->red) --new_red;
        u_ptr = u_ptr->next;
        v_ptr = v_ptr->next;
      } else if (u_ptr->target < v_ptr->target) {
        if (u_ptr->target != v) {
          if (!u_ptr->red) ++new_red;
        } else {
          new_red -= u_ptr->red;
        }
        u_ptr = u_ptr->next;
      } else {
        if (!v_ptr->red && v_ptr->target != u) ++new_red;
        v_ptr = v_ptr->next;
      }
    }
    while (u_ptr->is_edge) {
      if (!u_ptr->red) ++new_red;
      u_ptr = u_ptr->next;
    }
    while (v_ptr->is_edge) {
      if (!v_ptr->red) ++new_red;
      v_ptr = v_ptr->next;
    }
    return new_red;
  }

  static int find_node_fewest_new_red_neighbors(int u, graph &g) {
    int node{}, min{std::numeric_limits<int>::max()};
    for (auto &&e : g.adj_list[u]) {
      int new_red = new_red_edges(u, e.target, g);
      if (new_red >= min) continue;
      min = new_red;
      node = e.target;
    }
    return node;
  }
};

#endif /* end of include guard: GREEDYLTH_HH */
