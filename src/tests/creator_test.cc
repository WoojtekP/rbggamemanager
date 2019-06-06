//
// Created by shrum on 03.06.19.
//

//
// Created by shrum on 03.06.19.
//
#include <parser/parser_wrapper.h>
#include "test.h"
#include <construction/graph_creator.h>
#include <queue>

using namespace rbg;
using namespace std;

const char *kSmallGame = R"LIM(
#players = red(100), blue(100)
#pieces = e
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [e,e]
    [e,e])
//       1       2    3      4      5        6                 7
#rules = ->red ( up + down + left + right )* [$ turn = turn+1] ->blue
)LIM";

vector<vector<const Move *>> ShortestPaths(const Graph<unique_ptr<Move>> &graph, node_t initial, node_t final) {
  unordered_map<node_t, vector<vector<const Move *>>> shortest;
  shortest[initial].push_back({});
  queue<node_t> q;
  q.push(initial);
  while (!q.empty()) {
    node_t u = q.front();
    q.pop();
    if (u == final) {
      break;
    }
    for (const auto &transition : graph.Transitions(u)) {
      node_t v = transition.to;
      if (shortest.find(v) == shortest.end() || shortest.at(v).front().size() == shortest.at(u).front().size() + 1) {
        if (shortest.find(v) == shortest.end()) {
          q.push(v);
        }
        vector<vector<const Move *>> parent_paths = shortest[u];
        for (auto parent_path : parent_paths) {
          parent_path.push_back(transition.content.get());
          shortest[v].push_back(parent_path);
        }
      }
    }
  }
  return shortest[final];
}

vector<int> SortedActionIndices(const vector<const Move *> &path) {

  vector<int> result;
  result.reserve(path.size());
  for (const auto &move : path) {
    if (move) {
      result.push_back(move->index_in_expression());
    }
  }
  sort(result.begin(), result.end());
  return result;
}

int main() {
  auto pg = ParseGame(kSmallGame);
  auto result = CreateGraph(*pg->get_moves());
  auto paths = ShortestPaths(result.graph, result.initial, result.final);

  for (const auto &path : paths) {
    ASSERT(SortedActionIndices(path) == vector<int>({1, 6, 7}))
  }

  cout << "The graph looks like this:\n";
  cout << GraphDescription(result.graph, [](const unique_ptr<Move> &move) { return move ? move->to_rbg() : "eps"; })
       << "\n";
  cout << "The shortest move:\n";
  for (const auto &path : paths) {
    for (const auto &move : path) {
      if (move) {
        cout << move->to_rbg() << " (" << move->index_in_expression() << ") ";
      } else {
        cout << "eps" << " ";
      }
    }
    cout << "\n";
  }

}