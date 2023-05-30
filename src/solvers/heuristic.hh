#ifndef HEURISTIC_HH
#define HEURISTIC_HH

#include <chrono>

#include "../data_structures/graph.hh"
#include "../solvers/greedy_red_deg_limit.hh"
#include "../utils.hh"
#include "config/definitions.hh"

class Heuristic {
public:
  static std::string name() { return "Heuristic"; }

  static void solve(graph& g, Config& config) {
    unsigned int old_n = g.n;
    // TODO: only contract half?
    run_solver(g, config, Solver::TREE_CONTRACT);
    unsigned int tww = g.twin_width;
    g.uncontract(old_n - g.n);
    GreedyRedDegLimit::solve(g, config, tww/4);
  }
};

#endif /* end of include guard: HEURISTIC_HH */
