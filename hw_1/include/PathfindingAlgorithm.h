#ifndef PATHFINDINGALGORITHM_H
#define PATHFINDINGALGORITHM_H

#include "Algorithm.h"
#include "TankState.h"
#include "BfsUtils.h"

class PathfindingAlgorithm final : public Algorithm {
public:
    explicit PathfindingAlgorithm() = default;

    explicit PathfindingAlgorithm(int default_bfs_timer);

    ~PathfindingAlgorithm() override = default;

    std::string getName() const override { return "PathfindingAlgorithm"; }

    Action decideAction(const GameState &state) override;

private:
    const int default_bfs_timer_execution = 5;
    std::unordered_map<TankState, TankState> bfs_tree;
    int bfs_steps_timer = default_bfs_timer_execution;

    Action chase(const GameState &state);

    void set_bfs_timer(const GameState &state, int threshold = 3);

    Action getActionFromBfsTree(const GameState &state) const;
};


#endif //PATHFINDINGALGORITHM_H
