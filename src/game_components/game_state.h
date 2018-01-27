//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_GAME_STATE_H
#define RBGGAMEMANAGER_GAME_STATE_H


#include "game_description.h"
#include "lazy_evaluator.h"
#include "moves_cache.h"

class game_state {
    const game_description& parent;

    lazy_evaluator lazy_controller;

    board current_board;
    size_t board_width_cache;
    size_t board_height_cache;
    size_t current_x,current_y;
    fsm::state_id_t current_state;

    std::vector<int> sigma;
    token_id_t current_player;

    moves_cache moves_information;

    void inc_turn() { sigma[parent.get_turn_id()]++; }
    void dec_turn() { sigma[parent.get_turn_id()]--; }

    void set_piece(token_id_t piece)
    {
        sigma[current_piece()]--;
        current_board(current_x,current_y) = piece;
        sigma[piece]++;
    }

    lazy_evaluator& lazy()
    {
        return lazy_controller;
    }

    moves_cache& get_move_evaluator()
    {
        return moves_information;
    }

public:
    game_state(const game_description& description)
        : parent(description),
          current_board(description.get_initial_board()),
          board_width_cache(current_board.width()),
          board_height_cache(current_board.height()),
          current_x(0), current_y(0),
          current_state(description.get_moves_description().get_nfa().initial()),
          sigma(description.get_variables_count()),
          current_player(description.get_starting_player())
    {
        for(size_t y = 0; y < current_board.height(); y++)
        {
            for(size_t x = 0; x < current_board.width(); x++)
            {
                sigma[current_board(x,y)]++;
            }
        }
        sigma[get_description().get_turn_id()] = 0;
    }

    fsm::state_id_t get_current_state() const
    {
        return current_state;
    }

    token_id_t current_piece() const
    {
        return current_board(current_x,current_y);
    }

    const game_description& get_description() const
    {
        return parent;
    }

    int value(token_id_t variable) const
    {
        return sigma[variable];
    }

    const board& get_board() const
    {
        return current_board;
    }

    size_t x() const
    {
        return current_x;
    }

    size_t y() const
    {
        return current_y;
    }

    size_t width() const
    {
        return board_width_cache;
    }

    size_t height() const
    {
        return board_height_cache;
    }

    int turn() const
    {
        return sigma[parent.get_turn_id()];
    }

    token_id_t player() const
    {
        return current_player;
    }

    action_result apply_action_application(const action_application& application);
    void revert_action_application(const action_application& application, const action_result& application_result);

    void make_move(const move& move)
    {

        for(const auto& block : move.get_blocks())
        {
            current_state = parent.get_moves_description().get_corresponding_state(block.get_block_id());
            current_x = block.x();
            current_y = block.y();
            while(parent.get_moves_description().get_nfa()[current_state].transitions().size() == 1 &&
                    parent.get_moves_description().get_nfa()[current_state].transitions().front().letter()->get_index() == block.get_block_id())
            {
                parent.get_moves_description().get_nfa()[current_state].transitions().front().letter()->apply(this);
                current_state = parent.get_moves_description().get_nfa()[current_state].transitions().front().target();
            }
        }
        lazy_controller.clear();
        moves_information.clear();
    }

    std::vector<move> find_moves(ssize_t max_depth=-1)
    {
        return moves_information.find_moves(this, max_depth);
    }

    // Lazy controller is separated from main class
    friend class lazy_controller;

    // Modifying actions
    friend class actions::shift;
    friend class actions::off;
    friend class actions::player_switch;
    friend class actions::semi_switch;
    friend class actions::assignment;
    friend class actions::lazy;

    // Move pattern will be able to use the state to check the condition
    // This will speed up things. (We do not have to make a copy.)
    friend class conditions::move_pattern;
};

std::ostream& operator<<(std::ostream& s,const game_state& state);

#endif //RBGGAMEMANAGER_GAME_STATE_H
