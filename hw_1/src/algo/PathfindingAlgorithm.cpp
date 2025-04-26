#include "PathfindingAlgorithm.h"

PathfindingAlgorithm::PathfindingAlgorithm(int default_bfs_timer) : bfs_steps_timer(default_bfs_timer) {
}

Action PathfindingAlgorithm::decideAction(const GameState &state) const {
    int enemy_tank_distance = state.getEnemyDistance();

    // step 1: check if the tank is at risk
    if (enemy_tank_distance <= 3 || state.getApproachingShellsPosition().size() > 0) {
        return handleImmediateThreat(state, enemy_tank_distance);
    }
    return chase(state);
}

Action PathfindingAlgorithm::chase(const GameState &state) const {
    return NONE;
}
