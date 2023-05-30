#ifndef GREEDY_STRONG_HH
#define GREEDY_STRONG_HH

#include <climits>
#include <random>

#include "../config/config.hh"
#include "../data_structures/graph.hh"
#include "../reductions/degzero.hh"

class GreedyStrong {
public:
  static std::string name() { return "Greedy (strong)"; }

  static void solve(graph& g, Config& config) {
    int depth = config.search_depth ? config.search_depth : g.n;
    int contractions = 0;

    // init random
    std::random_device rd;
    std::mt19937 gen(rd());
    int idx = 0;

    std::vector<std::pair<node*, node*>> candidates;
    while (g.n >= 2 && contractions < depth) {
      unsigned int deg = -1;
      candidates.clear();
      for (node* u = g.get_first_node(); u; u = u->next) {
        for (node* v = u->next; v; v = v->next) {
          unsigned int new_deg = get_generated_red_deg(g, u, v);
          if (new_deg < deg) {
            deg = new_deg;
            candidates.clear();
            candidates.push_back({u, v});
          } else if (new_deg == deg) {
            candidates.push_back({u, v});
          }
        }
      }
      if (config.random) {
        std::uniform_int_distribution<> distr(0, candidates.size() - 1);
        idx = distr(gen);
      }
      g.contract(candidates[idx].first->id, candidates[idx].second->id);
      ++contractions;
    }
  }

private:
  static unsigned int get_generated_red_deg(graph& g, node* u, node* v) {
    unsigned int red_deg = 0;

    edge* u_ptr = u->first;
    edge* v_ptr = v->first;

    while (u_ptr->is_edge && v_ptr->is_edge) {
      if (u_ptr->target == v->id) {
        // edge uv
        u_ptr = u_ptr->next;
      } else if (v_ptr->target == u->id) {
        // edge vu
        v_ptr = v_ptr->next;
      } else if (u_ptr->target == v_ptr->target) {
        // common neighbor
        if (u_ptr->red || v_ptr->red) {
          ++red_deg;
        }
        u_ptr = u_ptr->next;
        v_ptr = v_ptr->next;
      } else if (u_ptr->target < v_ptr->target) {
        ++red_deg;
        u_ptr = u_ptr->next;
      } else {
        ++red_deg;
        v_ptr = v_ptr->next;
      }
    }
    while (u_ptr->is_edge) {
      ++red_deg;
      u_ptr = u_ptr->next;
    }
    while (v_ptr->is_edge) {
      ++red_deg;
      v_ptr = v_ptr->next;
    }

    return red_deg;
  }
};

#endif /* end of include guard: GREEDY_STRONG_HH */
