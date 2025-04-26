#ifndef SIMPLEALGORITHM_H
#define SIMPLEALGORITHM_H

#include "Algorithm.h"

class SimpleAlgorithm final : public Algorithm {
public:
    SimpleAlgorithm() = default;

    ~SimpleAlgorithm() override = default;

    std::string getName() const override { return "SimpleAlgorithm"; }

    Action decideAction(const GameState &state) override;

private:
    Action handleFutureThreat(const GameState &state) const;
};

#endif //SIMPLEALGORITHM_H
