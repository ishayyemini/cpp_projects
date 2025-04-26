#include <queue>
#include <unordered_set>

#include "BfsUtils.h"


Position BfsUtils::executeStatefulBfs(const GameState &state,
                                      std::unordered_map<TankState, TankState> &bfs_tree) {
    //clear the bfs tree
    bfs_tree.clear();

    //initialize the queue and visited set
    std::queue<std::pair<TankState, int> > visit_queue;
    std::unordered_set<TankState> visited;

    const Position start_position = state.getPlayerTank()->getPosition();
    const Direction::DirectionType start_direction = state.getPlayerTank()->getDirection();
    const Position enemy_position = state.getEnemyTank()->getPosition();

    TankState startState{start_position, start_direction};
    visit_queue.push({startState, 0});
    visited.insert(startState);

    TankState goalReached = startState;
    bool target_found = false;

    while (!visit_queue.empty()) {
        auto [current, cost] = visit_queue.front();
        visit_queue.pop();

        if (current.pos == enemy_position) {
            goalReached = current;
            target_found = true;
            break;
        }

        generateNextStates(state, current, cost, visit_queue, visited, bfs_tree);
    }

    if (!target_found) {
        return start_position; // No path found
    }

    return reconstructFirstMove(bfs_tree, goalReached, startState);
}


void BfsUtils::generateNextStates(
    const GameState &state,
    const TankState &current,
    int cost,
    std::queue<std::pair<TankState, int> > &q,
    std::unordered_set<TankState> &visited,
    std::unordered_map<TankState, TankState> &bfs_tree
) {
    // 1. Move Forward
    Position forwardPos = state.calcNextPosition(current.pos, current.dir);
    if (state.isSafePosition(forwardPos) && state.isEmptyPosition(forwardPos)) {
        TankState forwardState{forwardPos, current.dir};
        tryMove(state, q, visited, bfs_tree, current, forwardState, cost + 1); // Forward costs 1
    }

    // 2. Rotations: 90° Left, 45° Left, 45° Right, 90° Right
    std::vector<int> rotationAngles = {-90, -45, 45, 90}; // Degrees
    for (int angle: rotationAngles) {
        Direction::DirectionType newDir = Direction::getDirection(current.dir + angle);
        TankState rotatedState{current.pos, newDir}; // Rotation does not move
        tryMove(state, q, visited, bfs_tree, current, rotatedState, cost + 2); // Rotation costs 2
    }

    // 3. Move Backward
    Position backwardPos = state.calcNextPosition(current.pos, -current.dir);
    if (state.isSafePosition(backwardPos) && state.isEmptyPosition(backwardPos)) {
        TankState backwardState{backwardPos, current.dir}; // Position changes, direction stays
        tryMove(state, q, visited, bfs_tree, current, backwardState, cost + 3); // Backward costs 3
    }
}

void BfsUtils::tryMove(
    const GameState &state,
    std::queue<std::pair<TankState, int> > &q,
    std::unordered_set<TankState> &visited,
    std::unordered_map<TankState, TankState> &bfs_tree,
    const TankState &fromState,
    const TankState &toState,
    int newCost
) {
    if (!visited.count(toState)) {
        visited.insert(toState);
        bfs_tree[toState] = fromState;
        q.push({toState, newCost});
    }
}


Position BfsUtils::reconstructFirstMove(
    const std::unordered_map<TankState, TankState> &bfs_tree,
    const TankState &goalReached,
    const TankState &startState
) {
    // If no path exists
    if (bfs_tree.empty()) {
        return startState.pos; // No move, stay in place
    }

    // Start from goal
    TankState current = goalReached;

    // Backtrack until we reach a node whose parent is startState
    while (bfs_tree.count(current) && bfs_tree.at(current).pos != startState.pos) {
        current = bfs_tree.at(current);
    }

    // Now 'current' is the first move you should do after start
    return current.pos;
}
