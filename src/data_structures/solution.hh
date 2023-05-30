#ifndef SOLUTION_HH
#define SOLUTION_HH

#include <iostream>
#include <vector>

struct Contraction {
  unsigned int u;
  unsigned int v;
};

class Solution {
public:
  int twin_width;
  std::vector<Contraction> contractions;

  void add_contraction(unsigned int u, unsigned int v) { contractions.emplace_back(Contraction{u, v}); }

  void print() {
    std::cout << "c solution:\n";
    std::cout << "c twin width: " << twin_width << '\n';
    std::cout << "c contractions:\n";
    for (auto&& e : contractions) {
      std::cout << e.u << " " << e.v << "\n";
    }
  }
};

#endif  // SOLUTION_HH
