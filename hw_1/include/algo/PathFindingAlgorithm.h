#ifndef PATHFINDINGALGORITHM_H
#define PATHFINDINGALGORITHM_H
#include "Algorithm.h"

class PathfindingAlgorithm final : public Algorithm {
public:
    PathfindingAlgorithm(int default_bfs_timer);

    ~PathfindingAlgorithm() override;

    std::string getName() const override { return "PathfindingAlgorithm"; }

    Action decideAction(const GameState &state) const override;


private:
    int bfs_steps_timer;
    Action chase(const GameState &state) const;
};


#endif //PATHFINDINGALGORITHM_H
