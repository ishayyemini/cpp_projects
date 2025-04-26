#ifndef PATHFINDINGALGORITHM_H
#define PATHFINDINGALGORITHM_H

#include "Algorithm.h"

class PathfindingAlgorithm final : public Algorithm {
public:
    PathfindingAlgorithm() = default;

    Action decideAction(const GameState &state) const override { return NONE; }

    ~PathfindingAlgorithm() override = default;

    [[nodiscard]] std::string getName() const override { return "PathfindingAlgorithm"; }
};


#endif //PATHFINDINGALGORITHM_H
