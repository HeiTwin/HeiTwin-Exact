#include <chrono>
#include <iostream>
#include <stdexcept>

#include "config/config.hh"
#include "config/definitions.hh"
#include "data_structures/graph.hh"
#include "data_structures/solution.hh"
#include "reductions/reductions.hh"
#include "solvers/branch_and_bound.hh"
#include "solvers/greedy.hh"
#include "solvers/greedylth.hh"
#include "solvers/ordered.hh"
#include "solvers/random.hh"
#include "strategies/greedy.hh"
#include "strategies/local_search.hh"
#include "utils.hh"

int main(int argc, char **argv) {
  Config config = parse_params(argc, argv);

  graph g;
  g.read_from_stdin();

  solve_components(g, config);

  Solution solution = g.build_solution();
  solution.print();

  if (config.write_solution) {
    write_solution(g, solution);
  }

  return 0;
}
