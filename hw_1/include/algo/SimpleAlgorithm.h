#ifndef SIMPLEALGORITHM_H
#define SIMPLEALGORITHM_H

#include "Algorithm.h"

class SimpleAlgorithm final : public Algorithm {
public:
    SimpleAlgorithm();

    ~SimpleAlgorithm() override;

    std::string getName() const override { return "SimpleAlgorithm"; }

    Action decideAction(const GameState &state) const override;

private:

    Action escape(const GameState &state, bool can_change_direction, Action default_action=NONE) const;

    Action handleImmediateThreat(const GameState &state, int tankDistance, int threat_threshold = 2) const;

    Action handleFutureThreat(const GameState &state) const;

};

#endif //SIMPLEALGORITHM_H
