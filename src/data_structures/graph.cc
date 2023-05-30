#include "graph.hh"

#include <assert.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

int graph::get_node_degree(unsigned int v) const { return adj_list[v].deg; }

int graph::get_red_node_degree(unsigned int v) const { return adj_list[v].num_red; }

node *graph::get_first_node() { return &adj_list[1]; }

nodes graph::get_nodes() { return {&adj_list[1]}; }

void graph::contract(unsigned int u, unsigned int v) {
  if (u > v) std::swap(u, v);
  edge *u_ptr{adj_list[u].first};
  edge *v_ptr{adj_list[v].first};
  con_seq.push_back({u, v, n, m, twin_width});
  contraction &con{con_seq.back()};

  while (u_ptr->is_edge && v_ptr->is_edge) {
    if (u_ptr->target == v_ptr->target) {
      if (v_ptr->red) {
        if (u_ptr->red) {
          --adj_list[u_ptr->target].num_red;
        } else {
          u_ptr->red = 1;
          u_ptr->other->red = 1;
          ++adj_list[u].num_red;
          con.color_changed.push_back(u_ptr);
        }
      }
      v_ptr->other->prev->next = v_ptr->other->next;
      v_ptr->other->next->prev = v_ptr->other->prev;
      --adj_list[u_ptr->target].deg;
      m -= 2;
      u_ptr = u_ptr->next;
      v_ptr = v_ptr->next;
    } else if (u_ptr->target < v_ptr->target) {
      if (u_ptr->target == v) {
        if (u_ptr->red) --adj_list[u].num_red;
        --adj_list[u].deg;
        u_ptr->prev->next = u_ptr->next;
        u_ptr->next->prev = u_ptr->prev;
        con.target_changed.emplace_back(*u_ptr);
        con.target_changed.back().other = u_ptr;
        u_ptr = u_ptr->next;
        m -= 2;
        continue;
      }
      if (!u_ptr->red) {
        u_ptr->red = 1;
        u_ptr->other->red = 1;
        ++adj_list[u].num_red;
        if (++adj_list[u_ptr->target].num_red > twin_width) twin_width = adj_list[u_ptr->target].num_red;
        con.color_changed.push_back(u_ptr);
      }
      u_ptr = u_ptr->next;
    } else if (u_ptr->target > v_ptr->target) {
      if (v_ptr->target == u) {
        v_ptr = v_ptr->next;
        continue;
      }
      con.target_changed.emplace_back(*v_ptr);
      con.target_changed.back().other = v_ptr;
      if (!v_ptr->red) {
        v_ptr->red = 1;
        v_ptr->other->red = 1;
        if (++adj_list[v_ptr->target].num_red > twin_width) twin_width = adj_list[v_ptr->target].num_red;
      }
      // insert *v_ptr before *u_ptr
      v_ptr->prev->next = v_ptr->next;
      v_ptr->next->prev = v_ptr->prev;
      v_ptr->prev = u_ptr->prev;
      u_ptr->prev->next = v_ptr;
      u_ptr->prev = v_ptr;
      v_ptr->other->target = u;
      v_ptr = v_ptr->next;
      u_ptr->prev->next = u_ptr;

      ++adj_list[u].num_red;
      ++adj_list[u].deg;

      edge *tmp{u_ptr->prev->other->prev};
      if (tmp->is_edge && tmp->target > u) {
        tmp = tmp->prev;
        while (tmp->is_edge && tmp->target > u) tmp = tmp->prev;
        // insert u_ptr->prev->other after tmp
        u_ptr->prev->other->next->prev = u_ptr->prev->other->prev;
        u_ptr->prev->other->prev->next = u_ptr->prev->other->next;
        tmp->next->prev = u_ptr->prev->other;
        u_ptr->prev->other->next = tmp->next;
        tmp->next = u_ptr->prev->other;
        tmp->next->prev = tmp;
      }
    }
  }
  while (u_ptr->is_edge) {
    if (u_ptr->target == v) {
      if (u_ptr->red) --adj_list[u].num_red;
      --adj_list[u].deg;
      u_ptr->prev->next = u_ptr->next;
      u_ptr->next->prev = u_ptr->prev;
      con.target_changed.emplace_back(*u_ptr);
      con.target_changed.back().other = u_ptr;
      u_ptr = u_ptr->next;
      m -= 2;
      continue;
    }
    if (!u_ptr->red) {
      u_ptr->red = 1;
      u_ptr->other->red = 1;
      ++adj_list[u].num_red;
      if (++adj_list[u_ptr->target].num_red > twin_width) twin_width = adj_list[u_ptr->target].num_red;
      con.color_changed.push_back(u_ptr);
    }
    u_ptr = u_ptr->next;
  }
  while (v_ptr->is_edge) {
    if (v_ptr->target == u) {
      v_ptr = v_ptr->next;
      continue;
    }
    con.target_changed.emplace_back(*v_ptr);
    con.target_changed.back().other = v_ptr;
    if (!v_ptr->red) {
      v_ptr->red = 1;
      v_ptr->other->red = 1;
      if (++adj_list[v_ptr->target].num_red > twin_width) twin_width = adj_list[v_ptr->target].num_red;
    }
    // insert *v_ptr before *u_ptr
    v_ptr->prev->next = v_ptr->next;
    v_ptr->next->prev = v_ptr->prev;
    v_ptr->prev = u_ptr->prev;
    u_ptr->prev->next = v_ptr;
    u_ptr->prev = v_ptr;
    v_ptr->other->target = u;
    v_ptr = v_ptr->next;
    u_ptr->prev->next = u_ptr;

    ++adj_list[u].num_red;
    ++adj_list[u].deg;

    edge *tmp{u_ptr->prev->other->prev};
    if (tmp->is_edge && tmp->target > u) {
      tmp = tmp->prev;
      while (tmp->is_edge && tmp->target > u) tmp = tmp->prev;
      // insert u_ptr->prev->other after tmp
      u_ptr->prev->other->next->prev = u_ptr->prev->other->prev;
      u_ptr->prev->other->prev->next = u_ptr->prev->other->next;
      tmp->next->prev = u_ptr->prev->other;
      u_ptr->prev->other->next = tmp->next;
      tmp->next = u_ptr->prev->other;
      tmp->next->prev = tmp;
    }
  }
  adj_list[v].active = 0;
  adj_list[v].prev->next = adj_list[v].next;
  if (adj_list[v].next) adj_list[v].next->prev = adj_list[v].prev;

  --n;
  if (adj_list[u].num_red > twin_width) twin_width = adj_list[u].num_red;
}

void graph::uncontract(unsigned int steps) {
  while (steps-- && con_seq.size()) {
    contraction &con{con_seq.back()};

    n = con.n;
    m = con.m;
    twin_width = con.twin_width;

    edge *v_ptr{adj_list[con.v].last};
    while (v_ptr->is_edge) {
      if (v_ptr->target == con.u) {
        v_ptr = v_ptr->prev;
        continue;
      }
      if (v_ptr->red) ++adj_list[v_ptr->target].num_red;
      ++adj_list[v_ptr->target].deg;
      v_ptr->other->prev->next = v_ptr->other;
      v_ptr->other->next->prev = v_ptr->other;
      v_ptr = v_ptr->prev;
    }
    for (std::vector<edge>::reverse_iterator ep{con.target_changed.rbegin()}; ep != con.target_changed.rend(); ++ep) {
      edge *v_ptr{ep->other};
      if (ep->target == con.v) {
        if (v_ptr->red) ++adj_list[con.u].num_red;
        ++adj_list[con.u].deg;
        v_ptr->prev->next = v_ptr;
        v_ptr->next->prev = v_ptr;
        continue;
      }
      if (!ep->red) {
        v_ptr->red = 0;
        v_ptr->other->red = 0;
        --adj_list[v_ptr->target].num_red;
      }
      v_ptr->prev->next = v_ptr->next;
      v_ptr->next->prev = v_ptr->prev;
      v_ptr->prev = ep->prev;
      v_ptr->next = ep->next;
      v_ptr->prev->next = v_ptr;
      v_ptr->next->prev = v_ptr;
      v_ptr->other->target = con.v;

      --adj_list[con.u].num_red;
      --adj_list[con.u].deg;

      edge *tmp{v_ptr->other->next};
      if (tmp->is_edge && tmp->target < con.v) {
        tmp = tmp->next;
        while (tmp->is_edge && tmp->target < con.v) tmp = tmp->next;
        // insert v_ptr->other before tmp
        v_ptr->other->next->prev = v_ptr->other->prev;
        v_ptr->other->prev->next = v_ptr->other->next;
        tmp->prev->next = v_ptr->other;
        v_ptr->other->prev = tmp->prev;
        tmp->prev = v_ptr->other;
        tmp->prev->next = tmp;
      }
    }
    for (auto &&ep : con.color_changed) {
      ep->red = 0;
      ep->other->red = 0;
      --adj_list[ep->target].num_red;
      --adj_list[ep->other->target].num_red;
    }
    adj_list[con.v].active = 1;
    adj_list[con.v].prev->next = &adj_list[con.v];
    if (adj_list[con.v].next) adj_list[con.v].next->prev = &adj_list[con.v];

    con_seq.pop_back();
  }
}

bool graph::red_degree_in_limit(node *u, node *v, int limit) {
  if (v->deg > u->deg) {
    if (u->num_red + v->deg - u->deg > limit) return false;
  } else {
    if (v->num_red + u->deg - v->deg > limit) return false;
  }

  unsigned int red_deg = 0;

  edge *u_ptr = u->first;
  edge *v_ptr = v->first;

  while (u_ptr->is_edge && v_ptr->is_edge) {
    if (u_ptr->target == v->id) {
      // edge uv
      u_ptr = u_ptr->next;
    } else if (v_ptr->target == u->id) {
      // edge vu
      v_ptr = v_ptr->next;
    } else if (u_ptr->target == v_ptr->target) {
      // common neighbor
      if (u_ptr->red || v_ptr->red) {
        ++red_deg;
      }
      u_ptr = u_ptr->next;
      v_ptr = v_ptr->next;
    } else if (u_ptr->target < v_ptr->target) {
      ++red_deg;
      if (adj_list[u_ptr->target].num_red + (u_ptr->red ? 0 : 1) > limit) {
        return false;
      }
      u_ptr = u_ptr->next;
    } else {
      ++red_deg;
      if (adj_list[v_ptr->target].num_red + (v_ptr->red ? 0 : 1) > limit) {
        return false;
      }
      v_ptr = v_ptr->next;
    }
  }
  while (u_ptr->is_edge) {
    ++red_deg;
    if (adj_list[u_ptr->target].num_red + (u_ptr->red ? 0 : 1) > limit) {
      return false;
    }
    u_ptr = u_ptr->next;
  }
  while (v_ptr->is_edge) {
    ++red_deg;
    if (adj_list[v_ptr->target].num_red + (v_ptr->red ? 0 : 1) > limit) {
      return false;
    }
    v_ptr = v_ptr->next;
  }

  return red_deg <= limit;
}

std::vector<unsigned int> split_and_parse(std::string &s) {
  std::vector<unsigned int> res;
  std::string tmp;
  std::stringstream ss{s};
  while (getline(ss, tmp, ' ')) {
    res.push_back(atoi(tmp.c_str()));
  }
  return res;
}

void graph::read_from_stdin() {
  std::string line = "c";
  std::vector<unsigned int> input;
  std::vector<std::vector<unsigned int>> targets;
  unsigned int count{};
  while (line[0] != 'p') {
    std::getline(std::cin, line);
  }
  input = split_and_parse(line.erase(0, 6));
  n = input[0];
  m = 2 * input[1];
  twin_width = 0;
  edges.resize(m);
  con_seq.reserve(n);
  adj_list.resize(n + 1);
  partition.resize(n + 1, 0);
  targets.resize(n + 1, std::vector<unsigned int>{});

  adj_list[0].first = reinterpret_cast<edge *>(&adj_list[0]);
  adj_list[0].last = reinterpret_cast<edge *>(&adj_list[0]);
  adj_list[0].active = 0;
  for (unsigned int source{1}; source <= n; ++source) {
    adj_list[source].id = source;
    adj_list[source].first = reinterpret_cast<edge *>(&adj_list[source]);
    adj_list[source].last = reinterpret_cast<edge *>(&adj_list[source]);
    adj_list[source].prev = &adj_list[source - 1];
    adj_list[source - 1].next = &adj_list[source];
  }
  adj_list[1].prev = 0;
  adj_list[n].next = 0;

  while (std::getline(std::cin, line)) {
    if (!line.size() || line[0] == 'c') continue;
    input = split_and_parse(line);
    targets[input[0]].push_back(input[1]);
  }
  for (unsigned int source{1}; source <= n; ++source) {
    std::sort(targets[source].begin(), targets[source].end());

    for (auto &&target : targets[source]) {
      if (source >= target) continue;
      edges[count] = edge{target};
      adj_list[source].last->next = &edges[count];
      edges[count].prev = adj_list[source].last;
      adj_list[source].last = &edges[count];
      ++count;
      edges[count] = edge{source};
      adj_list[target].last->next = &edges[count];
      edges[count].prev = adj_list[target].last;
      adj_list[target].last = &edges[count];
      edges[count - 1].other = &edges[count];
      edges[count].other = &edges[count - 1];
      ++count;
      ++adj_list[source].deg;
      ++adj_list[target].deg;
    }
    adj_list[source].last->next = reinterpret_cast<edge *>(&adj_list[source]);
  }
}

void graph::print() {
  std::cout << "n: " << n << ", m: " << m << ", twin width: " << twin_width << std::endl;
  for (auto &&n : adj_list) {
    if (!n.active) continue;
    std::cout << "id: " << n.id << ", deg: " << n.deg << ", num_red: " << n.num_red << ", edges:";
    unsigned c{};
    for (auto &&e : n) {
      if (++c > 10) break;
      std::cout << ' ' << e.target << (e.red ? 'r' : ' ');
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

Solution graph::build_solution() {
  Solution solution{};
  for (auto &&con : con_seq) {
    solution.add_contraction(con.u, con.v);
  }
  solution.twin_width = twin_width;
  return solution;
}

int graph::uncontractions_till_worst_tww() {
  int worst_tww = 0;
  int worst_tww_n = 0;
  for (int i = 0; i < con_seq.size(); ++i) {
    if (con_seq[i].twin_width > worst_tww) {
      worst_tww = con_seq[i].twin_width;
      worst_tww_n = i;
    }
  }
  return std::max<int>(0, con_seq.size() - worst_tww_n);
}

unsigned int graph::compute_connected_components() {
  std::vector<bool> visited;
  visited.resize(adj_list.size() + 1, false);
  std::list<node *> queue;
  unsigned int partition_id = 0;
  for (node *nd = get_first_node(); nd; nd = nd->next) {
    if (!visited[nd->id]) {
      queue.push_back(nd);
      visited[nd->id] = true;
      partition[nd->id] = partition_id;
      while (!queue.empty()) {
        node *next_nd = queue.front();
        queue.pop_front();
        for (auto &&e : *next_nd) {
          if (!visited[e.target]) {
            queue.push_back(&adj_list[e.target]);
            visited[e.target] = true;
            partition[e.target] = partition_id;
          }
        }
      }
      ++partition_id;
    }
  }
  return partition_id;
}

void graph::read_from_partition(graph &g, unsigned int partition_id) {
  unsigned int count = 0;
  std::vector<unsigned int> mapping;
  mapping.resize(g.adj_list.size() + 1);
  unsigned int node_count = 0;

  n = 0;
  m = 0;
  for (node *nd = g.get_first_node(); nd; nd = nd->next) {
    if (g.partition[nd->id] == partition_id) {
      mapping[nd->id] = ++node_count;
      n++;
      for (auto &&e : *nd) {
        if (g.partition[e.target] == partition_id) {
          m++;
        }
      }
    }
  }

  std::vector<std::vector<unsigned int>> targets;
  node_mapping.resize(n + 1);

  twin_width = 0;
  edges.resize(m);
  con_seq.reserve(n);
  adj_list.resize(n + 1);
  partition.resize(n + 1, 0);
  targets.resize(n + 1, std::vector<unsigned int>{});

  adj_list[0].first = reinterpret_cast<edge *>(&adj_list[0]);
  adj_list[0].last = reinterpret_cast<edge *>(&adj_list[0]);
  adj_list[0].active = 0;
  for (unsigned int source{1}; source <= n; ++source) {
    adj_list[source].id = source;
    adj_list[source].first = reinterpret_cast<edge *>(&adj_list[source]);
    adj_list[source].last = reinterpret_cast<edge *>(&adj_list[source]);
    adj_list[source].prev = &adj_list[source - 1];
    adj_list[source - 1].next = &adj_list[source];
  }
  adj_list[1].prev = 0;
  adj_list[n].next = 0;

  // compute targets
  for (node *nd = g.get_first_node(); nd; nd = nd->next) {
    if (g.partition[nd->id] == partition_id) {
      node_mapping[mapping[nd->id]] = nd->id;
      for (auto &&e : *nd) {
        if (g.partition[e.target] == partition_id && nd->id < e.target) {
          targets[mapping[nd->id]].push_back(mapping[e.target]);
        }
      }
    }
  }

  for (unsigned int source{1}; source <= n; ++source) {
    std::sort(targets[source].begin(), targets[source].end());

    for (auto &&target : targets[source]) {
      if (source >= target) continue;
      edges[count] = edge{target};
      adj_list[source].last->next = &edges[count];
      edges[count].prev = adj_list[source].last;
      adj_list[source].last = &edges[count];
      ++count;
      edges[count] = edge{source};
      adj_list[target].last->next = &edges[count];
      edges[count].prev = adj_list[target].last;
      adj_list[target].last = &edges[count];
      edges[count - 1].other = &edges[count];
      edges[count].other = &edges[count - 1];
      ++count;
      ++adj_list[source].deg;
      ++adj_list[target].deg;
    }
    adj_list[source].last->next = reinterpret_cast<edge *>(&adj_list[source]);
  }
}

void graph::read_complement(graph &g) {
  unsigned int count = 0;
  unsigned int node_count = 0;

  n = g.n;
  m = n * (n - 1) - g.m;

  node_mapping.resize(n + 1);
  for (int i = 1; i < node_mapping.size(); ++i) {
    node_mapping[i] = i;
  }

  std::vector<std::vector<unsigned int>> targets;

  twin_width = 0;
  edges.resize(m);
  con_seq.reserve(n);
  adj_list.resize(n + 1);
  partition.resize(n + 1, 0);
  targets.resize(n + 1, std::vector<unsigned int>{});

  adj_list[0].first = reinterpret_cast<edge *>(&adj_list[0]);
  adj_list[0].last = reinterpret_cast<edge *>(&adj_list[0]);
  adj_list[0].active = 0;
  for (unsigned int source{1}; source <= n; ++source) {
    adj_list[source].id = source;
    adj_list[source].first = reinterpret_cast<edge *>(&adj_list[source]);
    adj_list[source].last = reinterpret_cast<edge *>(&adj_list[source]);
    adj_list[source].prev = &adj_list[source - 1];
    adj_list[source - 1].next = &adj_list[source];
  }
  adj_list[1].prev = 0;
  adj_list[n].next = 0;

  // compute targets
  for (node *nd = g.get_first_node(); nd; nd = nd->next) {
    unsigned int last = 1;
    for (auto &&e : *nd) {
      for (unsigned int i = last + 1; i < e.target; ++i) {
        if (i != nd->id) targets[nd->id].push_back(i);
      }
      last = e.target;
    }
    // add last nodes
    for (unsigned int i = last + 1; i <= n; ++i) {
      if (i != nd->id) targets[nd->id].push_back(i);
    }
  }

  for (unsigned int source{1}; source <= n; ++source) {
    std::sort(targets[source].begin(), targets[source].end());

    for (auto &&target : targets[source]) {
      if (source >= target) continue;
      edges[count] = edge{target};
      adj_list[source].last->next = &edges[count];
      edges[count].prev = adj_list[source].last;
      adj_list[source].last = &edges[count];
      ++count;
      edges[count] = edge{source};
      adj_list[target].last->next = &edges[count];
      edges[count].prev = adj_list[target].last;
      adj_list[target].last = &edges[count];
      edges[count - 1].other = &edges[count];
      edges[count].other = &edges[count - 1];
      ++count;
      ++adj_list[source].deg;
      ++adj_list[target].deg;
    }
    adj_list[source].last->next = reinterpret_cast<edge *>(&adj_list[source]);
  }
}

void graph::apply_contractions(graph &g) {
  Solution solution = g.build_solution();
  for (auto &c : solution.contractions) {
    if (g.node_mapping[c.u] && g.node_mapping[c.v]) contract(g.node_mapping[c.u], g.node_mapping[c.v]);
  }
}

bool graph::contractions_independent(std::pair<node *, node *> c1, std::pair<node *, node *> c2) {
  // c1.first is strictly the smallest id in the contractions
  assert(c1.first->id > c2.first->id);
  // c2 was already contracted, c2.second is not active
  assert(!c2.second->active);
  if (c1.second->id == c2.first->id) {
    return false;
  }

  edge *c2_1_ptr = c2.first->first;

  auto c1_max = std::max(c1.first->id, c1.second->id);
  while (c2_1_ptr->is_edge && c2_1_ptr->target <= c1_max) {
    if (c2_1_ptr->target == c1.first->id || c2_1_ptr->target == c1.second->id) return false;
    c2_1_ptr = c2_1_ptr->next;
  }

  return true;
}

void graph::write_to_file(std::string filename) {
  std::ofstream file{filename};
  if (file.is_open()) {
    file << n << " " << m << std::endl;
    for (node *nd = get_first_node(); nd; nd = nd->next) {
      for (auto &&e : *nd) {
        if (e.target < nd->id) {
          file << nd->id << " " << e.target << " " << (e.red ? 1 : 0) << std::endl;
        }
      }
    }
  }
}

graph::graph() {}

graph::~graph() {}
