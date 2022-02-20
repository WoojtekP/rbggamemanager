#ifndef RBGGAMEMANAGER_CALCULATE_SIM_H
#define RBGGAMEMANAGER_CALCULATE_SIM_H

#include "game_state/game_state.h"
#include "utility/printer.h"
#define RBG_RANDOM_GENERATOR 0
#include "rbg_random_generator.hpp"

namespace rbg {

struct SimResult {
  std::vector<unsigned long> sum_goals;
  unsigned long nodes_count, sim_count;
  bool moves_limit_exceeded;

  SimResult(uint players_count) : nodes_count(0), sim_count(0), moves_limit_exceeded(false) {
    sum_goals.resize(players_count, 0);
  }
};

SimResult RandomSimulations(GameState &state, uint sim_count,
                            unsigned long seed, uint depth_limit, uint moves_limit) {
  RBGRandomGenerator rng(seed);
  std::vector<variable_id_t> player_variable_ids;
  uint players_count = state.declarations().players_resolver().size() - 1;
  for (uint r = 0; r < players_count; r++) {
    auto player_name = state.declarations().players_resolver().Name(r + 1);
    player_variable_ids.push_back(
        state.declarations().variables_resolver().Id(player_name));
  }
  SimResult result(players_count);
  for (uint i = 0; i < sim_count; i++) {
    uint depth = 0;
    while (depth < depth_limit) {
      auto moves = state.Moves();
      if (moves.size() == 0 || moves.size() > moves_limit)
        break;
      uint chosen_move = rng.uniform_choice(moves.size());
      state.Apply(moves[chosen_move]);
      depth++;
    }
    if (depth >= depth_limit) {
      continue;
    }
    if (state.Moves().size() > moves_limit) {
      result.moves_limit_exceeded = true;
      return result;
    }
    result.nodes_count += depth;
    result.sim_count++;
    for (uint rvi = 0; rvi < player_variable_ids.size(); rvi++)
      result.sum_goals[rvi] +=
          state.variables_values()[player_variable_ids[rvi]];
    state.Reset();
  }
  return result;
}

} // namespace rbg

#endif // RBGGAMEMANAGER_CALCULATE_SIM_H
