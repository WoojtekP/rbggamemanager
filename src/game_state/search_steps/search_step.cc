//
// Created by shrum on 27.06.19.
//

#include "search_step.h"
#include <game_state/game_state.h>

using namespace rbg;

bool VisitedCheckSearchStep::Apply(GameState &state) {
  bool result = stack_chunk_[state.current_pos_];
  stack_chunk_.set(state.current_pos_);
  return !result;
}

bool ShiftStep::ApplyReversible(GameState &state, ShiftStep::revert_info_t &revert_info) {
  revert_info = state.current_pos_;
  state.current_pos_ = state.declarations().board_description.NextVertex(state.current_pos_, edge_id_);
  return state.current_pos_ != state.board_.vertices_count();
}

void ShiftStep::Revert(GameState &state, const ShiftStep::revert_info_t &revert_info) {
  state.current_pos_ = revert_info;
}

bool OnStep::Apply(GameState &state) {
  return pieces_[state.board_.at(state.current_pos_)];
}

bool ArithmeticLessEqualComparison::Apply(GameState &state) {
  return left_->Value(state) <= right_->Value(state);
}

bool ArithmeticLessComparison::Apply(GameState &state) {
  return left_->Value(state) < right_->Value(state);
}

bool ArithmeticEqualComparison::Apply(GameState &state) {
  return left_->Value(state) == right_->Value(state);
}

bool ArithmeticNotEqualComparison::Apply(GameState &state) {
  return left_->Value(state) != right_->Value(state);
}

bool ConditionCheckStep::Apply(GameState &state) {
  return steps_collection_->current()->EndStepReachable(state);
}

ConditionCheckStep::ConditionCheckStep(std::unique_ptr<SearchStepsCollection> steps_collection)
    : steps_collection_(std::move(steps_collection)) {}

bool NegatedConditionCheckStep::Apply(GameState &state) {
  return !steps_collection_->current()->EndStepReachable(state);
}

NegatedConditionCheckStep::NegatedConditionCheckStep(std::unique_ptr<SearchStepsCollection> steps_collection)
    : steps_collection_(std::move(steps_collection)) {}

void OffStep::ApplyReversible(GameState &state, OffStep::revert_info_t &revert_info) const {
  revert_info = state.board_.at(state.current_pos_);
  state.board_.set(state.current_pos_, piece_id_);
}

void OffStep::Revert(GameState &state, const OffStep::revert_info_t &revert_info) {
  state.board_.set(state.current_pos_, revert_info);
}

void OffStep::Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
                  std::vector<std::vector<ModifierApplication>> &moves) {
  revert_info_t revert_info;
  ApplyReversible(state, revert_info);
  applied_modifiers.push_back({state.current_pos_, index()});
  state.steps_.stack().Push();
  RunNextStep(state, applied_modifiers, moves);
  state.steps_.stack().Pop();
  applied_modifiers.pop_back();
  Revert(state, revert_info);
}

bool OffStep::ApplyFirstFound(GameState &state) {
  revert_info_t revert_info;
  ApplyReversible(state, revert_info);
  if (ApplyNextStepUntilFound(state))
    return true;
  Revert(state, revert_info);
  return false;
}

bool OffStep::EndStepReachable(GameState &state) {
  revert_info_t revert_info;
  ApplyReversible(state, revert_info);
  bool result = EndStepReachableFromNext(state);
  Revert(state, revert_info);
  return result;
}

void OffStep::Apply(GameState &state, vertex_id_t vertex) const {
  state.current_pos_ = vertex;
  revert_info_t revert_info;
  ApplyReversible(state, revert_info);
}

void PlayerSwitchStep::ApplyReversible(GameState &state, PlayerSwitchStep::revert_info_t &revert_info) const {
  revert_info = state.current_player_;
  state.current_player_ = player_id_;
}

void PlayerSwitchStep::Revert(GameState &state, const PlayerSwitchStep::revert_info_t &revert_info) {
  state.current_player_ = revert_info;
}

void PlayerSwitchStep::Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
                           std::vector<std::vector<ModifierApplication>> &moves) {
  revert_info_t revert_info;
  ApplyReversible(state, revert_info);
  applied_modifiers.push_back({state.current_pos_, index()});
  moves.push_back(applied_modifiers);
  applied_modifiers.pop_back();
  Revert(state, revert_info);
}

bool PlayerSwitchStep::ApplyFirstFound(GameState &state) {
  revert_info_t revert_info;
  ApplyReversible(state, revert_info);
  state.steps_.current_ = next_;
  return true;
}

bool PlayerSwitchStep::EndStepReachable(GameState &state) {
  revert_info_t revert_info;
  ApplyReversible(state, revert_info);
  return EndStepReachableFromNext(state);
}

void PlayerSwitchStep::Apply(GameState &state, vertex_id_t vertex) const {
  state.steps_.current_ = next_;
  state.current_pos_ = vertex;
  revert_info_t revert_info;
  ApplyReversible(state, revert_info);
}

bool AssignmentStep::ApplyReversible(GameState &state, AssignmentStep::revert_info_t &revert_info) const {
  revert_info = state.variables_values_[variable_id_];
  state.variables_values_[variable_id_] = value_->Value(state);
  return state.variables_values_[variable_id_] <= state.declarations().variables_bounds[variable_id_];
}

void AssignmentStep::Revert(GameState &state, const AssignmentStep::revert_info_t &revert_info) {
  state.variables_values_[variable_id_] = revert_info;
}

void AssignmentStep::Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
                         std::vector<std::vector<ModifierApplication>> &moves) {
  revert_info_t revert_info;
  if (ApplyReversible(state, revert_info)) {
    applied_modifiers.push_back({state.current_pos_, index()});
  state.steps_.stack().Push();
  RunNextStep(state, applied_modifiers, moves);
  state.steps_.stack().Pop();
  applied_modifiers.pop_back();
  }
  Revert(state, revert_info);
}

bool AssignmentStep::ApplyFirstFound(GameState &state) {
  revert_info_t revert_info;
  if (ApplyReversible(state, revert_info)) {
    if (ApplyNextStepUntilFound(state))
      return true;
  }
  Revert(state, revert_info);
  return false;
}

bool AssignmentStep::EndStepReachable(GameState &state) {
  revert_info_t revert_info;
  bool result = false;
  if (ApplyReversible(state, revert_info)) {
    result = EndStepReachableFromNext(state);
  }
  Revert(state, revert_info);
  return result;
}

void AssignmentStep::Apply(GameState &state, vertex_id_t vertex) const {
  state.current_pos_ = vertex;
  revert_info_t revert_info;
  ApplyReversible(state, revert_info);
}

bool PlayerCheckStep::Apply(GameState &state) {
  return state.current_player_ == player_;
}
