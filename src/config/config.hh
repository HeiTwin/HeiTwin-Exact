#ifndef CONFIG_HH
#define CONFIG_HH

#include <string>

#include "definitions.hh"

struct ReductionConfig {
  bool reduce_exhaustively;

  bool paths;
  bool twins;
  bool fast_twins;
  bool deg_zero;
  bool deg_one;
  bool red_deg_limit;

  ReductionConfig() {
    reduce_exhaustively = true;
    set_exact(true);
    set_heuristic(false);
    twins = false;
  }

  void set_exact(bool enable) {
    paths = enable;
    fast_twins = enable;
    twins = enable;
    deg_one = enable;
    deg_zero = enable;
  }

  void set_heuristic(bool enable) { red_deg_limit = enable; }
};

struct Config {
  std::string graph_filename;

  Strategy strategy;

  Solver solver;

  ReductionConfig reductions;

  int reduction_frequency;  // Apply reductions exhaustively every reduction_frequency iterations

  Stop_Condition stop_condition;

  int refinement_iterations;
  int uncontraction_depth;
  int search_depth;
  bool random;

  bool write_solution;
  bool write_heuristic_solution;
};

Config parse_params(int argc, char** argv);

const std::string get_solver_name(Solver solver);

#endif  // CONFIG_HH
