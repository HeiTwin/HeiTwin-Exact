#ifndef RANDOM_HH
#define RANDOM_HH

#include <cstdlib>

#include "../data_structures/graph.hh"

class Random {
public:
  static std::string name() { return "Random"; }

  static void solve(graph& g) {
    srand(time(NULL));
    std::vector<int> indices;
    indices.resize(g.n);
    for (int i = 0; i < g.n; ++i) {
      indices[i] = i + 1;
    }

    while (indices.size() > 1) {
      int node_a = rand() % indices.size();
      int node_b = rand() % (indices.size() - 1);
      if (node_b >= node_a) ++node_b;
      if (node_a > node_b) std::swap(node_a, node_b);
      g.contract(indices[node_a], indices[node_b]);
      std::swap(indices[node_b], indices[indices.size() - 1]);
      indices.pop_back();
    }
  }
};

#endif /* end of include guard: RANDOM_HH */
