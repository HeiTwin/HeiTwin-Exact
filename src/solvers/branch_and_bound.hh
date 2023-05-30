#ifndef BRANCH_AND_BOUND_HH
#define BRANCH_AND_BOUND_HH

#include "../data_structures/contraction_enumeration.hh"
#include "../data_structures/graph.hh"
#include "../solvers/greedy.hh"
#include "../utils.hh"
#include "cycle_detection.hh"
#include "greedy_red_deg_limit.hh"

class BranchAndBound {
public:
  static std::string name() { return "BranchAndBound"; }

  static void solve(graph &g, Config &config) {
    int desired_n =
        config.search_depth ? std::max<int>(g.n - config.search_depth, 1) : 1;

    unsigned int tww = -1;
    Solution solution;
    bool skip_bnb = false;

    // Heuristic solution
    int old_n = g.n;
    if (config.search_depth) {
      Config heuristic_config = config;
      heuristic_config.reductions.set_heuristic(false);
      heuristic_config.reductions.set_exact(true);
      heuristic_config.reductions.reduce_exhaustively = false;
      // For GreedyRedDegreeLimit reductions lead to much longer runtime
      heuristic_config.reduction_frequency = 0;
      run_solver(g, heuristic_config, config.solver);
      tww = g.twin_width;
      solution = g.build_solution();
      g.uncontract(old_n - g.n);
      std::cout << "c Heuristic tww: " << tww << std::endl;
    } else {
      if (g.n < 1000) { // Greedy B&B with depth 3
        Config c{};
        c.strategy = Strategy::GREEDY_BB;
        c.solver = Solver::RED_DEG_LIMIT;
        c.search_depth = 3;
        run_strategy(g, c, Strategy::GREEDY_BB);
        tww = g.twin_width;
        solution = g.build_solution();
        g.uncontract(old_n - g.n);
      }
      if (g.n < 500) { // Greedy B&B with depth 4
        Config c{};
        c.strategy = Strategy::GREEDY_BB;
        c.solver = Solver::RED_DEG_LIMIT;
        c.search_depth = 4;
        run_strategy(g, c, Strategy::GREEDY_BB);
        if (g.twin_width < tww) {
          tww = g.twin_width;
          solution = g.build_solution();
        }
        g.uncontract(old_n - g.n);
      }
      if (g.n < 100) { // Greedy B&B with depth 5+
        for (int i = 5; i <= 10; ++i) {

          Config c{};
          c.strategy = Strategy::GREEDY_BB;
          c.solver = Solver::RED_DEG_LIMIT;
          c.search_depth = i;
          run_strategy(g, c, Strategy::GREEDY_BB);
          if (g.twin_width < tww) {
            tww = g.twin_width;
            solution = g.build_solution();
          }
          g.uncontract(old_n - g.n);
        }
      }
      if (g.n < 300) { // Greedy
        Config c{};
        c.strategy = Strategy::SOLVER;
        c.solver = Solver::GREEDY;
        run_strategy(g, c, Strategy::SOLVER);
        if (g.twin_width < tww) {
          tww = g.twin_width;
          solution = g.build_solution();
        }
        g.uncontract(old_n - g.n);
      }
      // Plain red deg limit
      Config c{};
      c.strategy = Strategy::SOLVER;
      c.solver = Solver::RED_DEG_LIMIT;
      run_strategy(g, c, Strategy::SOLVER);
      if (g.twin_width < tww) {
        tww = g.twin_width;
        solution = g.build_solution();
      }
      g.uncontract(old_n - g.n);
      // Red deg limit random
      Config conf{};
      conf.strategy = Strategy::SOLVER;
      conf.solver = Solver::RED_DEG_LIMIT_RANDOM;
      conf.reductions.reduce_exhaustively = true;
      for (int i = 0; i < 25; ++i) {
        run_strategy(g, c, Strategy::SOLVER);
        if (g.twin_width < tww) {
          tww = g.twin_width;
          solution = g.build_solution();
          std::cout << "c Improved by random red deg limit: " << tww
                    << std::endl;
        }
        g.uncontract(old_n - g.n);
        std::cout << "c random run " << i << std::endl;
      }
      std::cout << "c Best heuristic tww: " << tww << std::endl;
      if (config.write_heuristic_solution) {
        write_solution(g, solution);
      }

      // Check for cycle, if this is an exact solution
      if (tww == 2) {
        bool has_cycle = CycleDetection::has_tww2_cycle(g);
        std::cout << "c Has cycle: " << has_cycle << std::endl;
        if (has_cycle) {
          skip_bnb = true;
        }
      }
    }

    if (skip_bnb) {
      std::cout << "c Skipping B&B because solution cannot be better"
                << std::endl;
    } else {
      ContractionEnumeration contraction_enumeration{g, config.search_depth};
      while (contraction_enumeration.do_next_contraction(g, tww)) {
        if (g.n == desired_n) { // leaf
          if (g.twin_width < tww) {
            if (!config.search_depth) {
              std::cout << "c Branch and bound improved tww from " << tww
                        << " to " << g.twin_width << std::endl;
              // If tww 2 was reached, check for a cycle to stop
              if (g.twin_width == 2 && CycleDetection::has_tww2_cycle(g)) {
                tww = g.twin_width;
                solution = g.build_solution();
                break;
              }
            }
            tww = g.twin_width;
            solution = g.build_solution();
          }
        } else if (g.twin_width >= tww) {
          contraction_enumeration.skip_branch(g);
        }
        // TODO: possible to add reductions?
      }
    }

    // apply best solution
    g.uncontract(solution.contractions.size());
    for (auto &contr : solution.contractions) {
      g.contract(contr.u, contr.v);
    }
  }
};

#endif /* end of include guard: BRANCH_AND_BOUND_HH */
