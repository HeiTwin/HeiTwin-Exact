#ifndef GREEDY_RED_DEG_LIMIT_HH
#define GREEDY_RED_DEG_LIMIT_HH

#include "../config/config.hh"
#include "../data_structures/graph.hh"
#include "../reductions/red_deg_limit.hh"
#include "../reductions/red_deg_limit_random.hh"
#include "../utils.hh"

class GreedyRedDegLimit {
public:
  static std::string name() { return "GreedyRedDegLimit"; }

  static void solve(graph& g, Config& config, unsigned int initial_tww = 1) {
    int depth = config.search_depth;
    if (!depth) depth = g.n;

    // Do one contraction to find initial tww
    Config c;
    c.search_depth = 1;
    config.random = false;
    run_solver(g, c, Solver::GREEDY_STRONG);

    for (int i = std::max<unsigned int>(initial_tww, g.twin_width); g.n > 1 && depth > 0; i++) {
      // TODO: depth limit not respected by reductions
      unsigned int old_n = g.n;
      if (config.reduction_frequency && i % config.reduction_frequency == 0) {
        reduce(g, config);
      }
      depth -= old_n - g.n;
      old_n = g.n;
      if (config.random) {
        reduce<RedDegLimitRandomReduction>(g, config.reductions.reduce_exhaustively, i, depth);
      } else {
        reduce<RedDegLimitReduction>(g, config.reductions.reduce_exhaustively, i, depth);
      }
      depth -= old_n - g.n;
    }
  }
};

#endif /* end of include guard: GREEDY_RED_DEG_LIMIT_HH */
