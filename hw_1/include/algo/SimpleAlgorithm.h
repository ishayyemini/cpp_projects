#ifndef SIMPLEALGORITHM_H
#define SIMPLEALGORITHM_H

#include "Algorithm.h"

class SimpleAlgorithm final : public Algorithm {
public:
    SimpleAlgorithm() = default;

    Action decideAction(const GameState &state) const override { return NONE; }

    ~SimpleAlgorithm() override = default;

    std::string getName() const override { return "SimpleAlgorithm"; }

    Action decideAction(const GameState &state) const override;

private:
    Action handleFutureThreat(const GameState &state) const;
};

#endif //SIMPLEALGORITHM_H
