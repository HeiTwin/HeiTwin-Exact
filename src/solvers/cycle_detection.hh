#ifndef CYCLE_DETECTION_HH
#define CYCLE_DETECTION_HH

#include <algorithm>
#include <list>
#include <vector>

#include "../data_structures/graph.hh"

class CycleDetection {
public:
  static std::string name() { return "CycleDetection"; }

  static bool has_tww2_cycle(graph& g) {
    std::vector<bool> visited;
    std::vector<unsigned int> empty = std::vector<unsigned int>(0);
    for (node* source = g.get_first_node(); source; source = source->next) {
      for (node* target = g.get_first_node(); target; target = target->next) {
        if (source->id < target->id) {
          auto path = bfs(g, source, target, empty);
          if (path.size() == 0) continue;  // no path
          auto second_path = bfs(g, source, target, path);
          if (second_path.size() == 0) continue;  // no path
          // TODO: look for paths of length at least 6 instead of 5.
          // This can be changed after the TODO below is fixed
          if (path.size() + second_path.size() >= 8) {
            std::cout << "c Chordless cycle found: ";
            for (unsigned int node : path) {
              std::cout << node << " ";
            }
            std::cout << "- ";
            for (unsigned int node : second_path) {
              std::cout << node << " ";
            }
            std::cout << std::endl;
            return true;
          }
        }
      }
    }
    return false;
  }

private:
  static std::vector<unsigned int> bfs(graph& g, node* source, node* target, std::vector<unsigned int>& previous_path) {
    std::vector<unsigned int> parent;
    parent.resize(g.adj_list.size());
    parent[source->id] = source->id;
    parent[target->id] = 0;

    std::vector<bool> visited = std::vector<bool>(g.adj_list.size(), false);
    std::vector<bool> is_previous_path = std::vector<bool>(g.adj_list.size(), false);
    visited[source->id] = true;
    // Ignore previous path
    for (unsigned int ignore : previous_path) {
      visited[ignore] = true;
      if (ignore != target->id) {
        is_previous_path[ignore] = true;
      }
    }
    // first nodes - needed to start BFS again from first node
    std::list<node*> first_queue;
    for (auto&& e : *source) {
      if (!is_previous_path[e.target]) {
        first_queue.push_back(&g.adj_list[e.target]);
        visited[e.target] = true;
      }
    }
    std::list<node*> queue;
    if (previous_path.size() == 0) {
      first_queue.clear();
      queue.push_back(source);
    }
    while (!queue.empty() || !first_queue.empty()) {
      if (queue.empty()) {
        queue.push_back(first_queue.front());
        first_queue.pop_front();
        parent[queue.back()->id] = source->id;
      }
      node* nd = queue.front();
      queue.pop_front();
      if (nd->id == target->id) break;
      for (edge&& e : *nd) {
        if (!e.is_edge) break;
        if (is_previous_path[e.target]) {
          // cycle not chordless, found node in previous path before target
          // start again from start node
          queue.clear();
          continue;
        }
        if (target->id == e.target) {
          parent[e.target] = nd->id;
          // TODO: need to check if the previous path is reachable at same depth
          goto end;
        }
        if (!visited[e.target]) {
          parent[e.target] = nd->id;
          visited[e.target] = true;
          queue.push_back(&g.adj_list[e.target]);
        }
      }
    }
  end:
    std::vector<unsigned int> result;
    if (!parent[target->id]) return result;
    unsigned int curr = target->id;
    while (parent[curr] != curr) {
      result.push_back(curr);
      curr = parent[curr];
    }
    result.push_back(source->id);
    return result;
  }
};

#endif
