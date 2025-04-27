#include "PathfindingAlgorithm.h"

#include <queue>
#include <set>

#include "Logger.h"

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

    Logger::getInstance().log(
        "Player " + std::to_string(state.getPlayerId()) + ": BFS: starting BFS from position (" +
        std::to_string(state.getPlayerTank()->getPosition().x) + "," + std::to_string(
            state.getPlayerTank()->getPosition().y) + ")");

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

        for (const auto dir: state.getSafeDirections(pos)) {
            const Position next = state.getBoard().wrapPosition(pos + dir);
            if (visited.contains(next)) continue;

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
        Logger::getInstance().log("Player " + std::to_string(state.getPlayerId()) + ": Threatened by shells.");
        return moveIfThreatened(state);
    }

    if (state.getPlayerTank()->getAmmunition() == 0) {
        Logger::getInstance().log("Player " + std::to_string(state.getPlayerId()) + ": No ammo.");
        return NONE;
    }

    Logger::getInstance().log(
        "Player " + std::to_string(state.getPlayerId()) + ": Checking shooting condition - canShoot: " +
        std::string(state.canShootEnemy() ? "true" : "false"));

    if (state.canShootEnemy()) {
        Logger::getInstance().log(
            "Player " + std::to_string(state.getPlayerId()) + ": Enemy in direction " + std::to_string(
                state.getPlayerTank()->getDirection()) + ". Shooting now.");
        tried_path_without_success = false;
        return SHOOT;
    }


    for (int dir_index = 0; dir_index < 8; ++dir_index) {
        Direction::DirectionType dir = Direction::getDirectionFromIndex(dir_index);
        if (state.getPlayerTank()->getDirection() == dir) continue;
        if (state.canShootEnemy(dir)) {
            Logger::getInstance().log(
                "Player " + std::to_string(state.getPlayerId()) + ": Enemy in direction " + std::to_string(dir_index) +
                ", turning toward it.");
            current_path.clear();
            tried_path_without_success = false;
            return state.rotateTowards(dir);
        }
    }

    const bool enemy_moved = state.getEnemyTank()->getPosition() != last_enemy_pos;

    if (current_path.empty() || enemy_moved || tried_path_without_success) {
        const std::string reason = current_path.empty()
                                       ? "empty"
                                       : (enemy_moved ? "enemy moved" : "triedPathWithoutSuccess");
        Logger::getInstance().log(
            "Player " + std::to_string(state.getPlayerId()) + ": Computing BFS (reason: " + reason + ")");
        current_path = computeBFS(state);
        tried_path_without_success = current_path.empty();
        last_enemy_pos = state.getEnemyTank()->getPosition();
    }

    if (current_path.empty()) {
        if (!state.canShootEnemy()) {
            Logger::getInstance().log("Player " + std::to_string(state.getPlayerId()) + ": Stuck, rotating randomly");
            tried_path_without_success = true;

            if (state.canShoot()) {
                Logger::getInstance().log(
                    "Player " + std::to_string(state.getPlayerId()) + ": Shooting randomly due to stuck state");
                return SHOOT;
            }

            return ROTATE_RIGHT_EIGHTH;
        }

        Logger::getInstance().log("Player " + std::to_string(state.getPlayerId()) + ": No path but can shoot directly");
        tried_path_without_success = false;
        return SHOOT;
    }

    Direction::DirectionType target_dir = current_path.front();
    if (state.getPlayerTank()->getDirection() == target_dir) {
        const Position next_pos = state.getBoard().wrapPosition(state.getPlayerTank()->getPosition() + target_dir);

        if (state.getBoard().isMine(next_pos)) {
            Logger::getInstance().log(
                "Player " + std::to_string(state.getPlayerId()) + ": Mine ahead – aborting move and resetting path");
            current_path.clear();
            tried_path_without_success = true;
            return NONE;
        }

        if (state.getBoard().isWall(next_pos)) {
            if (state.canShoot()) {
                Logger::getInstance().log(
                    "Player " + std::to_string(state.getPlayerId()) + ": Wall ahead – shooting it");
                tried_path_without_success = false;
                return SHOOT;
            }

            return NONE;
        }

        current_path.erase(current_path.begin());
        Logger::getInstance().log(
            "Player " + std::to_string(state.getPlayerId()) + ": Moving forward to (" + std::to_string(next_pos.x) + ","
            + std::to_string(next_pos.y) + ")");
        tried_path_without_success = false;
        return MOVE_FORWARD;
    }
    // if he isn't able to do anything, just rotate and maybe it will help in the next steps
    Logger::getInstance().log(
        "Player " + std::to_string(state.getPlayerId()) + ": Rotating from " + std::to_string(
            state.getPlayerTank()->getDirection()) + " to " +
        std::to_string(target_dir));
    tried_path_without_success = false;
    return state.rotateTowards(target_dir);
}

