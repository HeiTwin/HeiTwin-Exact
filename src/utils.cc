#include "utils.hh"

#include <chrono>
#include <stdexcept>
#include <string>

#include "config/definitions.hh"
#include "reductions/degzero.hh"
#include "reductions/red_deg_limit.hh"
#include "reductions/reductions.hh"
#include "solvers/branch_and_bound.hh"
#include "solvers/greedy.hh"
#include "solvers/greedy_red_deg_limit.hh"
#include "solvers/greedy_strong.hh"
#include "solvers/greedylth.hh"
#include "solvers/heuristic.hh"
#include "solvers/ordered.hh"
#include "solvers/tree_contract.hh"
#include "strategies/greedy.hh"
#include "strategies/local_search.hh"

void reduce(graph &g, Config &config) {
  int old_n = g.n, previous_n = 0;
  auto start = std::chrono::high_resolution_clock::now();
  do {
    previous_n = g.n;
    if (config.reductions.deg_zero) {
      reduce<DegreeZeroReduction>(g);
    }
    if (config.reductions.deg_one) {
      reduce<DegreeOneReduction>(g);
    }
    if (config.reductions.fast_twins) {
      reduce<TwinReduction>(g, 0, true);
    }
    if (config.reductions.twins) {
      reduce<TwinReduction>(g, false);
    }
    if (config.reductions.paths) {
      reduce<PathReduction>(g);
    }
    if (config.reductions.red_deg_limit) {
      reduce<RedDegLimitReduction>(g, config.reductions.reduce_exhaustively);
    }
  } while (config.reductions.reduce_exhaustively && g.n != previous_n);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "c Reductions ran in " << ((double)duration.count()) / 1e6 << "s and contracted " << old_n - g.n
            << " nodes";
  if (config.reductions.reduce_exhaustively) {
    std::cout << " (exhaustively)";
  }
  std::cout << std::endl;
  std::cout << "c Reductions;" << old_n << ";" << g.n << ";" << (g.n / (double)old_n) << std::endl;
}

void run_solver(graph &g, Config &config, Solver solver) {
  auto start = std::chrono::high_resolution_clock::now();
  switch (solver) {
    case Solver::GREEDY:
      Greedy::solve(g, config.search_depth);
      break;
    case Solver::GREEDYLTH:
      Greedylth::solve(g);
      break;
    case Solver::ORDERED:
      Ordered::solve(g);
      break;
    case Solver::RED_DEG_LIMIT:
      GreedyRedDegLimit::solve(g, config);
      break;
    case Solver::ONLY_REDUCTIONS:
      break;
    case Solver::BRANCH_AND_BOUND:
      config.solver = Solver::RED_DEG_LIMIT;
      solve<BranchAndBound>(g, config);
      break;
    case Solver::TREE_CONTRACT:
      solve<TreeContract>(g);
      break;
    case Solver::GREEDY_STRONG:
      solve<GreedyStrong>(g, config);
      break;
    case Solver::HEURISTIC:
      solve<Heuristic>(g, config);
      break;
    default:
      throw std::invalid_argument("Solver not covered");
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "c Solver " << get_solver_name(solver) << " ran in " << ((double)duration.count()) / 1e6
            << "s, n: " << g.n << std::endl;
}

void run_strategy(graph &g, Config &config, Strategy strategy) {
  switch (strategy) {
    case Strategy::GREEDY_BB:
      greedy_depth_limited_bb(g, config);
      break;
    case Strategy::SOLVER:
      single_solver(g, config);
      break;
    case Strategy::LOCAL_SEARCH:
      local_search(g, config);
      break;
    default:
      throw std::invalid_argument("Strategy not covered");
  }
}

void solve_complement_components(graph &g, Config &config) {
  std::cout << "c Computing complement" << std::endl;
  graph complement;
  complement.read_complement(g);
  unsigned int num_components = complement.compute_connected_components();
  Config config_copy = Config{config};
  if (num_components <= 1) {
    // complement is connected
    if (g.m < complement.m) {
      run_strategy(g, config_copy, config_copy.strategy);
    } else {
      std::cout << "c Complement has fewer edges, solving complement" << std::endl;
      run_strategy(complement, config_copy, config_copy.strategy);
      g.apply_contractions(complement);
    }
  } else {
    // complement is disconnected
    std::cout << "c Complement is disconnected, solving " << num_components << " components" << std::endl;
    // FIXME: unnecessary work because of recursion; will stop tough because graph must be connected afterwards
    solve_components(complement, config);
    g.apply_contractions(complement);
  }
}

void solve_components(graph &g, Config &config) {
  unsigned int num_components = g.compute_connected_components();
  if (num_components <= 1) {
    std::cout << "c Only one component in original graph, solving the whole graph" << std::endl;
    solve_complement_components(g, config);
  } else {
    std::vector<int> counts;
    counts.resize(g.n + 1, 0);
    for (node *nd = g.get_first_node(); nd; nd = nd->next) {
      if (nd->active) ++counts[g.partition[nd->id]];
    }
    for (unsigned int i = 0; i <= num_components; ++i) {
      if (counts[i] > 1) {
        std::cout << "c Solving component " << i << " with " << counts[i] << " nodes" << std::endl;
        graph component;
        component.read_from_partition(g, i);
        solve_complement_components(component, config);
        g.apply_contractions(component);
      }
    }
    std::cout << "c Contracting last nodes of components" << std::endl;
    // TODO: does this work with modules?
    reduce<DegreeZeroReduction>(g);
  }
}

void write_solution(graph &g, Solution &solution) {
  Solution restore = g.build_solution();
  g.uncontract(restore.contractions.size());
  for (int i = 0; i < solution.contractions.size(); ++i) {
    g.write_to_file(".out/graph/" + std::to_string(i) + ".gr");
    g.contract(solution.contractions[i].u, solution.contractions[i].v);
  }
  g.uncontract(solution.contractions.size());
  for (auto &&c : restore.contractions) {
    g.contract(c.u, c.v);
  }
}
