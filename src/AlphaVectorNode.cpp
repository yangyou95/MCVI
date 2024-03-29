#include "../include/AlphaVectorNode.h"

#include <algorithm>
#include <limits>

std::unordered_map<int64_t, double> AlphaVectorNode::InitDoubleKeys(
    const std::vector<int64_t>& action_space) const {
  std::unordered_map<int64_t, double> v;
  for (const auto& a : action_space) v[a] = 0.0;
  return v;
}

std::unordered_map<int64_t, bool> AlphaVectorNode::InitBoolKeys(
    const std::vector<int64_t>& action_space) const {
  std::unordered_map<int64_t, bool> v;
  for (const auto& a : action_space) v[a] = false;
  return v;
}

AlphaVectorNode::ValueMap AlphaVectorNode::InitValueMap(
    const std::vector<int64_t>& action_space,
    const std::vector<int64_t>& observation_space) const {
  ValueMap v;
  for (const auto& a : action_space) {
    std::unordered_map<int64_t, std::unordered_map<int64_t, double>> v_a;
    for (const auto& o : observation_space) v_a[o] = {};
    v[a] = v_a;
  }
  return v;
}

AlphaVectorNode::AlphaVectorNode(const BeliefParticles& state_particles,
                                 const std::vector<int64_t>& action_space,
                                 const std::vector<int64_t>& observation_space)
    : _state_particles(state_particles),
      _Q_action(InitDoubleKeys(action_space)),
      _R_action(InitDoubleKeys(action_space)),
      _V_a_o_n(InitValueMap(action_space, observation_space)),
      _V_node_s(),
      _V_node_s_count(),
      _V_node(0.0),
      _best_action_update(InitBoolKeys(action_space)) {}

int64_t AlphaVectorNode::GetBestAction() const {
  const auto best_action =
      std::max_element(std::begin(_Q_action), std::end(_Q_action),
                       [](const std::pair<int64_t, double>& p1,
                          const std::pair<int64_t, double>& p2) {
                         return p1.second < p2.second;
                       });
  return best_action->first;
}

int64_t AlphaVectorNode::SampleParticle() const {
  return _state_particles.SampleOneState();
}

void AlphaVectorNode::UpdateBestValue() {
  const int64_t best_action = GetBestAction();
  _V_node = _Q_action.at(best_action);
}

const std::unordered_map<int64_t, double>&
AlphaVectorNode::GetActionObservationValues(int64_t a, int64_t o) const {
  return _V_a_o_n.at(a).at(o);
}

void AlphaVectorNode::ReInit(const std::vector<int64_t>& action_space,
                             const std::vector<int64_t>& observation_space) {
  _R_action = InitDoubleKeys(action_space);
  _Q_action = InitDoubleKeys(action_space);
  _V_a_o_n = InitValueMap(action_space, observation_space);
}

void AlphaVectorNode::UpdateValue(int64_t action, int64_t observation,
                                  int64_t nI, double val) {
  _V_a_o_n[action][observation][nI] += val;
}
