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

void PathfindingAlgorithm::set_bfs_timer(const GameState &state, int threshold) {
    auto distance = state.getEnemyDistance();
    bfs_steps_timer = (distance > threshold) ? default_bfs_timer_execution : 0;
}

Action PathfindingAlgorithm::chase(const GameState &state){
    if (bfs_steps_timer == 0) {
        //timer expired, so we can start a new bfs
        Position position = execute_bfs(state);
        set_bfs_timer(state);
        return state.getActionToPosition(position);
    }
    return decideOfflineAction(state);
}
