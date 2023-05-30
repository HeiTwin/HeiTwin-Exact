#include "local_search.hh"

void local_search(graph& g, Config& config) {
  std::cout << "c Using B&B local search strategy with " << get_solver_name(config.solver) << std::endl;

  int depth = config.search_depth;
  config.search_depth = 0;

  reduce(g, config);
  // Initial solution
  run_solver(g, config, config.solver);
  std::cout << "c Initial solver twin width: " << g.twin_width << std::endl;
  std::cerr << 0 << ";" << g.twin_width << std::endl;

  for (int i = 1; i <= config.refinement_iterations; ++i) {
    if (i % config.reduction_frequency == 0) {
      reduce(g, config);
    }
    // Find position in contraction sequence, where the worst tww occurs first
    int uncontractions_till_worst = g.uncontractions_till_worst_tww();
    // Uncontract until before the worst point
    std::cout << "c Uncontracting " << uncontractions_till_worst << " + " << config.uncontraction_depth
              << " till worst tww" << std::endl;
    g.uncontract(uncontractions_till_worst + config.uncontraction_depth);
    // Try to skip the worst point using B&B
    std::cout << "c B&B local search at " << g.n << " nodes and tww " << g.twin_width << std::endl;
    solve<BranchAndBound>(g, config);
    std::cout << "c Local search result tww: " << g.twin_width << ", n: " << g.n << std::endl;
    run_solver(g, config, config.solver);
    // TODO: how to output?
    std::cerr << i << ";" << g.twin_width << std::endl;
  }
}