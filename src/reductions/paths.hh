#ifndef PATHS_HH
#define PATHS_HH

#include <algorithm>
#include <deque>
#include <iostream>
#include <list>
#include <numeric>
#include <vector>

#include "../data_structures/graph.hh"

class PathReduction {
public:
  static std::string name() { return "PathReduction"; }

  static void reduce(graph& g, bool exact = false) {
    std::vector<bool> checked;  // nodes already checked
    checked.resize(g.adj_list.size(), false);
    for (node nd : g.get_nodes()) {
      if (!checked[nd.id]) {
        if (nd.deg == 1 || nd.deg == 2) {
          checked[nd.id] = true;
          // check if there is a path long enough
          node* start_of_path = get_end_of_path(g, &nd, checked);
          std::list<node*> path = find_path(g, start_of_path, checked);
          trim_path(path, exact);
          if (path.size() == 3 && path.front()->deg == 1 && path.back()->deg == 1) {
            // Special case: isolated path of length 3 has tww 0
            g.contract(path.front()->id, path.back()->id);
            // first node is always smaller than last
            path.pop_back();
            g.contract(path.front()->id, path.back()->id);
          } else if (path.size() > 1) {
            // std::cout << "c contracting path of length " << path.size() << std::endl;
            node* result = path.front();
            std::reduce(path.begin(), path.end(), result, [&](node* a, node* b) {
              if (a != b) {
                g.contract(a->id, b->id);
              }
              return a->id < b->id ? a : b;
            });
          }
        }
      }
    }
  }

private:
  static node* get_end_of_path(graph& g, node* nd, std::vector<bool>& checked) {
    if (nd->deg == 1) return nd;
    node* next = nd;
    unsigned int last_id = 0;
    int degree;
    while ((degree = next->deg) <= 2) {
      checked[next->id] = true;
      if (degree == 1) {
        return next;
      } else if (degree == 2) {
        node* old_next = next;
        next = next->first->target == last_id ? &g.adj_list[next->last->target] : &g.adj_list[next->first->target];
        last_id = old_next->id;
        if (next->id == nd->id) return nd;  // cycle
      }
    }
    return &g.adj_list[last_id];
  }

  static std::list<node*> find_path(graph& g, node* nd, std::vector<bool>& checked) {
    std::list<node*> path;
    path.push_back(nd);
    unsigned int last_id = nd->id;
    node* next = &g.adj_list[nd->first->target];
    if (next->deg > 2) {
      next = &g.adj_list[nd->last->target];
    }
    if (next->deg > 2) {
      // not a path, only single degree 2 node
      return path;
    }

    int degree;
    while ((degree = next->deg) <= 2) {
      checked[next->id] = true;
      if (degree == 1) {
        path.push_back(next);
        return path;
      } else if (degree == 2) {
        path.push_back(next);
        node* old_next = next;
        next = next->first->target == last_id ? &g.adj_list[next->last->target] : &g.adj_list[next->first->target];
        last_id = old_next->id;
        if (next->id == nd->id) break;  // cycle
      }
    }
    return path;
  }

  static void trim_path(std::list<node*>& path, bool exact) {
    if (path.size() > 1) {
      if (path.front()->id == path.back()->id) {
        // cycle, do not contract node twice, but contract whole cycle
        path.pop_back();
      }
      if (exact) {
        if (path.front()->deg != 1 || path.back()->deg != 1) {
          path.clear();
        }
      } else {
        // do not contract the last nodes if they have degree 2
        if (path.front()->deg != 1) {
          path.pop_front();
        }
        if (path.size() > 1 && path.back()->deg != 1) {
          path.pop_back();
        }
      }
    }
  }
};

#endif
