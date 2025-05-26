#ifndef PATHFINDINGALGORITHM_H
#define PATHFINDINGALGORITHM_H

#include "MyTankAlgorithm.h"

class PathfindingAlgorithm : public MyTankAlgorithm {
public:
    explicit PathfindingAlgorithm() = default;

    ~PathfindingAlgorithm() override = default;

    std::string getName() const override { return "PathfindingAlgorithm"; }

    ActionRequest getAction() override;

private:
    std::vector<Direction::DirectionType> computeBFS(const MyBattleInfo &state);
};


#endif //PATHFINDINGALGORITHM_H
