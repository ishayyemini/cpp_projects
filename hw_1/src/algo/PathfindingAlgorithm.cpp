#include "PathfindingAlgorithm.h"

#include <queue>
#include <set>
#include <iostream>

bool tried_path_without_success = false;
// a boolean to indicate if we are stuck in the current BFS path and need to recompute the path
Position last_enemy_pos = {-1, -1}; // invalid value for the start
std::vector<Direction::DirectionType> current_path; // the current path we got from the BFS computation

std::vector<Direction::DirectionType> PathfindingAlgorithm::computeBFS(const GameState &state) {
    struct Node {
        Position pos;
        std::vector<Direction::DirectionType> path;

        Node(const Position &pos, const std::vector<Direction::DirectionType> &path): pos(pos), path(path) {
        }
    };

    // Logger::debug(
    //     "Player 1 - BFS: starting BFS from position (" + std::to_string(self.getPosition().x) + "," + std::to_string(
    //         self.getPosition().y) + ")");

    int width = state.getBoard().getWidth(), height = state.getBoard().getHeight();
    std::queue<Node> q;
    std::set<Position> visited;
    q.push(Node(state.getPlayerTank()->getPosition(), {}));

    std::vector<Direction::DirectionType> best_path;
    size_t shortest_length = std::numeric_limits<size_t>::max();

    while (!q.empty()) {
        auto [pos, path] = q.front();
        q.pop();
        if (visited.contains(pos)) continue;
        visited.insert(pos);
        // checking if the next step in the queue can lead to shooting the enemy directly
        if (state.canShootEnemy(pos)) {
            if (path.size() < shortest_length) {
                best_path = path;
                shortest_length = path.size();
            }
            continue;
        }

        for (int i = 0; i < 8; ++i) {
            const Direction::DirectionType dir = Direction::getDirectionFromIndex(i);
            const Position next = state.getBoard().wrapPosition(pos + dir);
            //
            // if (next.x < 0 || next.x >= width || next.y < 0 || next.y >= height)
            //     continue;

            if (visited.contains(next)) continue;
            if (state.getBoard().isMine(next)) continue;

            std::vector<Direction::DirectionType> new_path = path;
            new_path.push_back(dir);
            q.push(Node(next, new_path));
        }
    }
    return best_path;
}

Action PathfindingAlgorithm::decideAction(const GameState &state) {
    if (!state.getPlayerTank() || state.getPlayerTank()->isDestroyed() ||
        !state.getEnemyTank() || state.getEnemyTank()->isDestroyed()) {
        return NONE;
    }

    if (state.isShellApproaching(state.getPlayerTank()->getPosition())) {
        // Logger::debug("Player 1: Threatened By Shells");
        return moveIfThreatened(state);
    }

    if (state.getPlayerTank()->getAmmunition()) {
        // Logger::debug("Player 1: No ammo");
        return NONE;
    }

    // Logger::debug("Player 1: Checking shooting condition - canShoot: " +
    //               std::string(canShoot(self, enemy, board) ? "true" : "false") +
    //               ", shootingStatus: " + std::to_string(self.getShootingStatus()));

    if (state.canShootEnemy()) {
        // Logger::debug(
        //     "Player 1: Enemy in direction " + std::to_string(self.getDirection().getDirection()) + ". Shooting now.");
        tried_path_without_success = false;
        return SHOOT;
    }


    for (int dir_index = 0; dir_index < 8; ++dir_index) {
        Direction::DirectionType dir = Direction::getDirectionFromIndex(dir_index);
        if (state.getPlayerTank()->getDirection() == dir) continue;
        if (state.canShootEnemy(dir)) {
            // Logger::debug("Player 1: Enemy in direction " + std::to_string(dir_index) + ", turning toward it.");
            current_path.clear();
            tried_path_without_success = false;
            return state.rotateTowards(dir);
        }
    }

    const bool enemy_moved = state.getEnemyTank()->getPosition() != last_enemy_pos;

    if (current_path.empty() || enemy_moved || tried_path_without_success) {
        // const std::string reason = current_path.empty() ? "empty" : (enemy_moved ? "enemy moved" : "triedPathWithoutSuccess");
        // Logger::debug("Player 1: Computing BFS (reason: " + reason + ")");
        current_path = computeBFS(state);
        tried_path_without_success = current_path.empty();
        last_enemy_pos = state.getEnemyTank()->getPosition();
    }

    if (current_path.empty()) {
        if (!state.canShootEnemy()) {
            // Logger::debug("Player 1: Stuck, rotating randomly");
            tried_path_without_success = true;

            if (state.canShoot()) {
                // Logger::debug("Player 1: Shooting randomly due to stuck state");
                return SHOOT;
            }

            return ROTATE_RIGHT_EIGHTH;
        }

        // Logger::debug("Player 1: No path but can shoot directly");
        tried_path_without_success = false;
        return SHOOT;
    }

    Direction::DirectionType target_dir = current_path.front();
    if (state.getPlayerTank()->getDirection() == target_dir) {
        const Position next_pos = state.getPlayerTank()->getPosition() + target_dir;

        if (state.getBoard().isMine(next_pos)) {
            // Logger::debug("Player 1: Mine ahead – aborting move and resetting path");
            current_path.clear();
            tried_path_without_success = true;
            return NONE;
        }

        if (state.getBoard().isWall(next_pos)) {
            if (state.canShoot()) {
                // Logger::debug("Player 1: Wall ahead – shooting it");
                tried_path_without_success = false;
                return SHOOT;
            }

            return NONE;
        }

        current_path.erase(current_path.begin());
        // Logger::debug(
        //     "Player 1: Moving forward to (" + std::to_string(next_pos.x) + "," + std::to_string(next_pos.y) + ")");
        tried_path_without_success = false;
        return MOVE_FORWARD;
    }
    // if he isn't able to do anything, just rotate and maybe it will help in the next steps
    // Logger::debug(
    //     "Player 1: Rotating from " + std::to_string(self.getDirection().getDirection()) + " to " +
    //     std::to_string(target_dir));
    tried_path_without_success = false;
    return state.rotateTowards(target_dir);
}

