#include "greedy.hh"

#include "../reductions/reductions.hh"
#include "../solvers/branch_and_bound.hh"
#include "../utils.hh"

void greedy_depth_limited_bb(graph& g, Config& config) {
  std::cout << "c Using depth limited B&B strategy with depth " << config.search_depth
            << (config.search_depth ? "" : " (variable)") << std::endl;
  int cnt = 0;
  while (g.n > 1) {
    if (config.reduction_frequency && cnt % config.reduction_frequency == 0) {
      reduce(g, config);
    }
    // TODO: if search depth is 0, calculate dynamic depth
    solve<BranchAndBound>(g, config);
    ++cnt;
  }
}

void single_solver(graph& g, Config& config) {
  std::cout << "c Using single solver strategy" << std::endl;
  reduce(g, config);
  run_solver(g, config, config.solver);
}
