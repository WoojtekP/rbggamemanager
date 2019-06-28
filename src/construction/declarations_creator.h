//
// Created by shrum on 16.06.19.
//

#ifndef RBGGAMEMANAGER_DECLARATIONS_CREATOR_H
#define RBGGAMEMANAGER_DECLARATIONS_CREATOR_H

#include <game_state/declarations.h>

namespace rbg {
  Declarations CreateDeclarations(const rbg_parser::parsed_game &game) {
    NamesResolver pieces_resolver;
    NamesResolver variables_resolver;
    NamesResolver players_resolver;
    std::vector<variable_value_t> variables_bounds(game.get_declarations().get_legal_variables().size() +
                                                   game.get_declarations().get_legal_players().size());
    for (const auto &piece : game.get_declarations().get_legal_pieces()) {
      pieces_resolver.Id(piece.to_string());
    }
    for (const auto &player : game.get_declarations().get_legal_players()) {
      auto variable_id = variables_resolver.Id(player.first.to_string());
      players_resolver.Id(player.first.to_string());
      variables_bounds[variable_id] = player.second;
    }
    for (const auto &variable : game.get_declarations().get_legal_variables()) {
      auto variable_id = variables_resolver.Id(variable.first.to_string());
      variables_bounds[variable_id] = variable.second;
    }

    Board board(game.get_board().get_size(), game.get_declarations().get_legal_edges().size(), pieces_resolver.size());
    for (const auto &edge : game.get_declarations().get_legal_edges()) {
      board.AddEdgeName(edge.to_string());
    }
    for (uint id = 0; id < game.get_board().get_size(); id++) {
      board.AddVertexName(game.get_board().get_vertex(id).to_string());
      for (const auto &edge : game.get_board().get_outgoing_edges(id)) {
        board.AddEdge(id, board.edges_names().Id(edge.first.to_string()), edge.second);
      }
      board.initial_content().set(id, pieces_resolver.Id(game.get_board().get_starting_piece(id).to_string()));
    }

    // Add keeper player id
    player_id_t keeper_id = players_resolver.Id(">");

    return {pieces_resolver, variables_resolver, players_resolver, variables_bounds, board, keeper_id};
  }
}

#endif //RBGGAMEMANAGER_DECLARATIONS_CREATOR_H
