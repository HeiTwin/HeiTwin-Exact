#ifndef GRAPH_HH_GV0YL8EE
#define GRAPH_HH_GV0YL8EE

#include <vector>

#include "solution.hh"

struct edge {
  unsigned int target{};
  unsigned char red{};
  unsigned char is_edge{1};
  unsigned char ph[2];
  edge *next;
  edge *prev;
  edge *other;
};

struct edge_iter {
  edge *p;

  edge operator*() { return *p; }

  edge *operator->() { return p; }

  bool operator!=(edge_iter &other) { return p != other.p; }

  edge_iter operator++() {
    p = p->next;
    return *this;
  }
};

struct node {
  unsigned int id;
  unsigned char red{};
  unsigned char is_edge{};
  unsigned char active{1};
  unsigned char ph;
  edge *first;
  edge *last;
  unsigned int deg{};
  unsigned int num_red{};
  node *next;
  node *prev;

  edge_iter begin() { return {first}; }

  edge_iter end() { return {reinterpret_cast<edge *>(this)}; }
};

struct node_iter {
  node *p;

  node operator*() { return *p; }

  node *operator->() { return p; }

  bool operator!=(node_iter &other) { return p != other.p; }

  node_iter operator++() {
    p = p->next;
    return *this;
  }
};

struct nodes {
  node *first_node;

  nodes(node *node) : first_node(node) {}

  node_iter begin() { return {first_node}; }

  node_iter end() { return {nullptr}; }
};

struct contraction {
  unsigned int u;
  unsigned int v;
  unsigned int n;
  unsigned int m;
  unsigned int twin_width;
  unsigned int ph;
  std::vector<edge *> color_changed;
  std::vector<edge> target_changed;
};

class graph {
public:
  unsigned int n, m;
  unsigned int twin_width;
  std::vector<node> adj_list;
  std::vector<unsigned int> partition;
  std::vector<unsigned int> node_mapping;

  int get_node_degree(unsigned int v) const;
  int get_red_node_degree(unsigned int v) const;
  node *get_first_node();
  nodes get_nodes();

  void contract(unsigned int u, unsigned int v);
  void uncontract(unsigned int steps = 1);
  bool red_degree_in_limit(node *u, node *v, int limit);
  bool contractions_independent(std::pair<node *, node *> c1, std::pair<node *, node *> c2);
  void read_from_stdin();
  Solution build_solution();
  void print();
  void write_to_file(std::string filename);

  int uncontractions_till_worst_tww();
  unsigned int compute_connected_components();
  void read_from_partition(graph &g, unsigned int partition_id);
  void read_complement(graph &g);
  void apply_contractions(graph &g);

  graph();
  graph(const graph &g) = delete;
  virtual ~graph();

  std::vector<contraction> con_seq;

private:
  std::vector<edge> edges;
};

#endif /* end of include guard: GRAPH_HH_GV0YL8EE */
