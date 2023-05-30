#include "config.hh"

#include <stdlib.h>

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../argtable/src/argtable3.h"
#include "definitions.hh"

Config default_config = {
    .strategy = Strategy::SOLVER,
    .solver = Solver::BRANCH_AND_BOUND,
    .reductions = ReductionConfig{},
    .reduction_frequency = 1,
    .refinement_iterations = 0,
    .uncontraction_depth = 0,
    .search_depth = 0,
    .random = 0,
    .write_solution = false,
    .write_heuristic_solution = false,
};

static const std::vector<std::string> strategy_names = {"solver", "greedy-bb", "local-search"};
static const std::vector<std::string> solver_names = {
    "greedy",          "greedylth", "ordered",      "red-deg-limit",
    "only-reductions", "bnb",       "treecontract", "red-deg-limit-random",
    "greedy-strong",   "heuristic"};

Strategy parse_strategy(const std::string &arg) {
  for (size_t i = 0; i < strategy_names.size(); ++i) {
    if (arg == strategy_names[i]) {
      return (Strategy)i;
    }
  }
  throw std::invalid_argument("Strategy is invalid");
}

Solver parse_solver(const std::string &arg) {
  for (size_t i = 0; i < solver_names.size(); ++i) {
    if (arg == solver_names[i]) {
      return (Solver)i;
    }
  }
  throw std::invalid_argument("Solver is invalid");
}

Config parse_params(int argc, char **argv) {
  Config config = default_config;

  struct arg_lit *help = arg_lit0("h", "help", "Display this help");
  struct arg_lit *list = arg_lit0("l", "list", "List available strategies and solvers");
  struct arg_end *end;
  struct arg_str *strategy = arg_str0("s", "strategy", "<strategy>", "Strategy to use");
  struct arg_str *solver = arg_str0(NULL, "solver", "<solver>", "Solver to use in strategy");
  struct arg_int *red_exhaust = arg_int0(NULL, "reduce-exhaustively", NULL, "Apply reductions exhaustively");
  struct arg_int *exact_red = arg_int0(NULL, "exact-reductions", "<0|1>", "Enable all exact reductions");
  struct arg_int *heuristic_red = arg_int0(NULL, "heuristic-reductions", "<0|1>", "Enable all heuristic reductions");
  struct arg_int *red_freq = arg_int0(NULL, "reduction-frequency", NULL, "Refinement iterations");
  struct arg_int *ref_iter = arg_int0(NULL, "refinement-iterations", NULL, "Refinement iterations");
  struct arg_int *uncontr_depth = arg_int0("u", "uncontr-depth", NULL, "Uncontraction depth");
  struct arg_int *search_depth = arg_int0("d", "search-depth", NULL, "Search depth");
  struct arg_int *random = arg_int0(NULL, "random", "<0|1>", "Enable randomization");
  // exact reductions
  struct arg_int *reduction_deg_zero = arg_int0(NULL, "red-deg-zero", "<0|1>", "Enable degree zero reduction");
  struct arg_int *reduction_deg_one = arg_int0(NULL, "red-deg-one", "<0|1>", "Enable degree one reduction");
  struct arg_int *reduction_paths = arg_int0(NULL, "red-paths", "<0|1>", "Enable path reduction");
  struct arg_int *reduction_twins = arg_int0(NULL, "red-twins", "<0|1>", "Enable twin reduction");
  struct arg_int *reduction_fast_twins =
      arg_int0(NULL, "red-fast-twins", "<0|1>", "Enable fast twin reduction checking neighbors only");
  // heuristic reductions
  struct arg_int *reduction_red_deg_limit =
      arg_int0(NULL, "red-red-deg-limit", "<0|1>", "Enable red degree limit reduction");
  // debug
  struct arg_int *write_solution = arg_int0(NULL, "write-solution", "<0|1>", "Write the solution steps into files");
  struct arg_int *write_heuristic_solution = arg_int0(NULL, "write-heuristic-solution", "<0|1>",
                                                      "Write the solution steps of the heuristic solution into files");

  void *argtable[] = {help,
                      list,
                      strategy,
                      solver,
                      red_exhaust,
                      exact_red,
                      heuristic_red,
                      red_freq,
                      ref_iter,
                      uncontr_depth,
                      search_depth,
                      reduction_deg_zero,
                      reduction_deg_one,
                      reduction_paths,
                      reduction_twins,
                      reduction_red_deg_limit,
                      reduction_fast_twins,
                      random,
                      write_solution,
                      write_heuristic_solution,
                      end = arg_end(20)};

  int nerrors = arg_parse(argc, argv, argtable);

  if (help->count > 0) {
    printf("Usage: %s", argv[0]);
    arg_print_syntax(stdout, argtable, "\n");
    arg_print_glossary(stdout, argtable, "  %-25s %s\n");
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    exit(0);
  } else if (nerrors > 0) {
    // ignore invalid arguments and return default
    return config;
    // arg_print_errors(stdout, end, argv[0]);
    // printf("Try '%s --help' for more information.\n", argv[0]);
    // arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    // exit(1);
  } else if (list->count > 0) {
    std::cout << "List of available strategies:" << std::endl;
    for (auto &strat : strategy_names) {
      std::cout << "\t" << strat << std::endl;
    }
    std::cout << "List of available solvers:" << std::endl;
    for (auto &solver : solver_names) {
      std::cout << "\t" << solver << std::endl;
    }
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    exit(0);
  }

  if (strategy->count > 0) {
    config.strategy = parse_strategy(strategy->sval[0]);
  }
  if (solver->count > 0) {
    config.solver = parse_solver(solver->sval[0]);
  }
  if (ref_iter->count > 0) {
    config.refinement_iterations = ref_iter->ival[0];
  }
  if (uncontr_depth->count > 0) {
    config.uncontraction_depth = uncontr_depth->ival[0];
  }
  if (search_depth->count > 0) {
    config.search_depth = search_depth->ival[0];
  }
  if (red_exhaust->count > 0) {
    config.reductions.reduce_exhaustively = red_exhaust->ival[0];
  }
  if (exact_red->count > 0) {
    config.reductions.set_exact(exact_red->ival[0]);
  }
  if (heuristic_red->count > 0) {
    config.reductions.set_heuristic(heuristic_red->ival[0]);
  }
  if (red_freq->count > 0) {
    config.reduction_frequency = red_freq->ival[0];
  }
  if (random->count > 0) {
    config.random = random->ival[0];
  }
  // reductions
  if (reduction_deg_zero->count) {
    config.reductions.deg_zero = reduction_deg_zero->ival[0];
  }
  if (reduction_deg_one->count) {
    config.reductions.deg_one = reduction_deg_one->ival[0];
  }
  if (reduction_paths->count) {
    config.reductions.paths = reduction_paths->ival[0];
  }
  if (reduction_twins->count) {
    config.reductions.twins = reduction_twins->ival[0];
  }
  if (reduction_red_deg_limit->count) {
    config.reductions.red_deg_limit = reduction_red_deg_limit->ival[0];
  }
  if (reduction_fast_twins->count > 0) {
    config.reductions.fast_twins = reduction_fast_twins->ival[0];
  }

  // debug
  if (write_solution->count) {
    config.write_solution = write_solution->ival[0];
  }
  if (write_heuristic_solution->count) {
    config.write_heuristic_solution = write_heuristic_solution->ival[0];
  }

  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  return config;
}

const std::string get_solver_name(Solver solver) { return solver_names[(size_t)solver]; }
