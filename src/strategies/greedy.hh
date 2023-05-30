#ifndef STRAT_GREEDY_HH
#define STRAT_GREEDY_HH

#include "../config/config.hh"
#include "../data_structures/graph.hh"
#include "../utils.hh"

void greedy_depth_limited_bb(graph& g, Config& config);

void single_solver(graph& g, Config& config);

#endif
