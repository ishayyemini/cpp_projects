#ifndef PATHFINDINGALGORITHM_H
#define PATHFINDINGALGORITHM_H

#include "Algorithm.h"

class PathfindingAlgorithm final : public Algorithm {
public:
    explicit PathfindingAlgorithm() = default;

    explicit PathfindingAlgorithm(int default_bfs_timer);

    ~PathfindingAlgorithm() override = default;

    std::string getName() const override { return "PathfindingAlgorithm"; }

    Action decideAction(const GameState &state) const override;


private:
    const int default_bfs_timer_execution = 5;

    int bfs_steps_timer = default_bfs_timer_execution;

    Action chase(const GameState &state);
    void set_bfs_timer(const GameState &state, int threshold = 3);
};


#endif //PATHFINDINGALGORITHM_H
