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
        Position position = BfsUtils::execute_stateful_bfs(state, bfs_tree);
        set_bfs_timer(state);
        return state.getActionToPosition(position);
    }
    return getActionFromBfsTree(state);
}

Action PathfindingAlgorithm::getActionFromBfsTree(const GameState &state) const{
    const auto* player = state.getPlayerTank();
    TankState current{player->getPosition(), player->getDirection()};
    if (bfs_tree.find(current) == bfs_tree.end()) {
        return NONE; // No known move
    }

    TankState nextState = bfs_tree.at(current);

    // Case 1: Moving to a new position (forward or backward)
    if (nextState.pos != current.pos) {
        // Moving forward or backward
        auto expectedForward = state.calcNextPosition(current.pos, current.dir);
        if (expectedForward == nextState.pos) {
            return MOVE_FORWARD;
        }
        auto expectedBackward = state.calcNextPosition(current.pos, -current.dir);
        if (expectedBackward == nextState.pos) {
            return MOVE_BACKWARD;
        }
        // Should never happen if BFS is correct
        return NONE;
    }

    // Case 2: Same position, different direction (rotation)
    int angleDiff = Direction::getAngleDifference(current.dir, nextState.dir);

    if (angleDiff == 45) return Action::ROTATE_45_RIGHT;
    if (angleDiff == 90) return Action::ROTATE_90_RIGHT;
    if (angleDiff == 315) return Action::ROTATE_45_LEFT; // 360-45
    if (angleDiff == 270) return Action::ROTATE_90_LEFT; // 360-90

    // No change or invalid
    return Action::NONE;

}

