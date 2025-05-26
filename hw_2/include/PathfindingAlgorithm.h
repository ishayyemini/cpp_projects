#ifndef PATHFINDINGALGORITHM_H
#define PATHFINDINGALGORITHM_H

#include "MyTankAlgorithm.h"

class PathfindingAlgorithm final : public Algorithm {
public:
    explicit PathfindingAlgorithm() = default;

    ~PathfindingAlgorithm() override = default;

    std::string getName() const override { return "PathfindingAlgorithm"; }

    Action decideAction(const GameState &state) override;

private:
    std::vector<Direction::DirectionType> computeBFS(const GameState &state);
};


#endif //PATHFINDINGALGORITHM_H
