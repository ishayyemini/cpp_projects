#include "PathfindingAlgorithm.h"

#include <format>
#include <queue>
#include <set>
#include "Logger.h"
#include "Direction.h"


// a boolean to indicate if we are stuck in the current BFS path and need to recompute the path
bool tried_path_without_success = false;
std::vector<Direction::DirectionType> current_path; // the current path we got from the BFS computation

void PathfindingAlgorithm::initLatestEnemyPosition() {
    last_enemy_positions = std::vector<Position>(battle_status.getEnemyTankCounts(), {-1, -1});
}

void PathfindingAlgorithm::updateLatestEnemyPosition() {
    last_enemy_positions = battle_status.getEnemyPositions();
}


void PathfindingAlgorithm::handleTankThreatened(ActionRequest *request, std::string *request_title) {
    //or shell is approaching or enemy tank is close
    was_threatened = true;
    if (battle_status.canTankShootEnemy(true)) {
        *request = ActionRequest::Shoot;
        *request_title = "Shoot threat";
        return;
    }
    *request = moveIfThreatened();
    *request_title = "Escape threat";
}

void PathfindingAlgorithm::tryShootEnemy(ActionRequest *request, std::string *request_title) {
    if (battle_status.canTankShootEnemy()) {
        tried_path_without_success = false;
        *request = ActionRequest::Shoot;
        *request_title = "Shoot Enemy";
        return;
    }
    rotateToEnemy(request, request_title);
}

bool PathfindingAlgorithm::rotateToEnemy(ActionRequest *request, std::string *request_title) const {
    for (auto dir_index = 0; dir_index < 8; ++dir_index) {
        Direction::DirectionType dir = Direction::getDirectionFromIndex(dir_index);
        if (battle_status.tank_direction == dir) continue;
        if (battle_status.canTankShootEnemy(dir)) {
            current_path.clear();
            tried_path_without_success = false;
            *request = battle_status.rotateTowards(dir);
            *request_title = "Rotating " + std::to_string(dir_index) + " toward enemy";
            return true;
        }
    }
    return false;
}

bool PathfindingAlgorithm::hasEnemyMoved() const {
    auto enemy_positions = battle_status.getEnemyPositions();
    if (enemy_positions.size() != last_enemy_positions.size()) {
        return true; // if the number of enemies changed, we definitely need to recompute the path
    }
    //todo: is there a better way to check? we don't really know the enemy's tank index so this is the best we can do i think
    return enemy_positions == last_enemy_positions;
}

void PathfindingAlgorithm::updatePathIfNeeded() {
    const bool enemy_moved = hasEnemyMoved();
    if (current_path.empty() || enemy_moved || tried_path_without_success) {
        const std::string reason = current_path.empty()
                                       ? "empty"
                                       : (enemy_moved ? "enemy moved" : "triedPathWithoutSuccess");
        std::string msg = "Computing BFS (reason: " + reason + ")";
        current_path = computeBFS();
        tried_path_without_success = current_path.empty();
        updateLatestEnemyPosition();
    }
}

void PathfindingAlgorithm::handleEmptyPath(ActionRequest *request, std::string *request_title) const {
    if (battle_status.canTankShoot()) {
        *request = ActionRequest::Shoot;
        tried_path_without_success = !battle_status.canTankShootEnemy();
        *request_title = (tried_path_without_success)
                             ? "Shooting randomly due to stuck state"
                             : "No path but can shoot directly";
        return;
    }
    *request_title = "Stuck, rotating randomly";
    *request = ActionRequest::RotateLeft45;
    tried_path_without_success = true;
}


void PathfindingAlgorithm::followPathOrRotate(ActionRequest *request, std::string *request_title) {
    Direction::DirectionType target_dir = current_path.front();
    if (battle_status.tank_direction == target_dir) {
        const Position next_pos = battle_status.wrapPosition(battle_status.tank_position + target_dir);

        if (battle_status.getBoardItem(next_pos) == '@') {
            *request = ActionRequest::DoNothing;
            *request_title = "Mine ahead – aborting move and resetting path";
            current_path.clear();
            tried_path_without_success = true;
            return;
        }

        if (battle_status.getBoardItem(next_pos) == '#') {
            if (battle_status.canTankShoot()) {
                *request = ActionRequest::Shoot;
                *request_title = "Wall ahead – shooting it";
                tried_path_without_success = false;
                return;
            }
            *request_title = "Wall ahead – Can't shoot. Do nothing";
            *request = ActionRequest::DoNothing;
            return;
        }

        current_path.erase(current_path.begin());
        tried_path_without_success = false;
        *request_title = "Moving forward to (" + std::to_string(next_pos.x) + "," + std::to_string(next_pos.y) + ")";
        *request = ActionRequest::MoveForward;
        return;
    }

    // if he isn't able to do anything, just rotate and maybe it will help in the next steps
    tried_path_without_success = false;
    *request_title = std::format("Rotating from direction {} to {}", battle_status.tank_direction, target_dir);
    *request = battle_status.rotateTowards(target_dir);
}


std::vector<Direction::DirectionType> PathfindingAlgorithm::computeBFS() {
    struct Node {
        Position pos;
        std::vector<Direction::DirectionType> path;

        Node(const Position &pos, const std::vector<Direction::DirectionType> &path): pos(pos), path(path) {
        }
    };

    std::string msg = std::format("Performing BFS. Start Position = ({}, {})",
                                  battle_status.tank_position.x,
                                  battle_status.tank_position.y);
    printLogs(msg);

    std::queue<Node> q;
    std::set<Position> visited;
    q.push(Node(battle_status.tank_position, {}));

    std::vector<Direction::DirectionType> best_path;
    size_t shortest_length = std::numeric_limits<size_t>::max();

    while (!q.empty()) {
        auto [position, path] = q.front();
        q.pop();
        if (visited.contains(position)) continue;
        visited.insert(position);
        // checking if the next step in the queue can lead to shooting the enemy directly
        if (battle_status.canTankShootEnemy(position)) {
            if (path.size() < shortest_length) {
                best_path = path;
                shortest_length = path.size();
            }
            continue;
        }

        for (const auto dir: battle_status.getSafeDirections(position)) {
            const Position next = battle_status.wrapPosition(position + dir);
            if (visited.contains(next)) continue;
            std::vector<Direction::DirectionType> new_path = path;
            new_path.push_back(dir);
            q.push(Node(next, new_path));
        }
    }
    return best_path;
}

void PathfindingAlgorithm::calculateAction(ActionRequest *request, std::string *request_title) {
    if (battle_status.turn_number == 0 || was_threatened) {
        was_threatened = false;
        *request = ActionRequest::GetBattleInfo;
        *request_title = "Requesting Battle Info (first turn or previously threatened)";
        return;
    }
    if (isTankThreatened) {
        handleTankThreatened(request, request_title);
        return;
    }
    if (!battle_status.hasTankAmmo() || (battle_status.last_requested_info_turn < battle_status.turn_number)) {
        battle_status.last_requested_info_turn = battle_status.turn_number + 1;
        *request = ActionRequest::GetBattleInfo;
        *request_title = "Requesting updated Battle Info";
        return;
    }
    tryShootEnemy(request, request_title);
    if (*request == ActionRequest::DoNothing) {
        updatePathIfNeeded();
        (current_path.empty())
            ? handleEmptyPath(request, request_title)
            : followPathOrRotate(request, request_title);
    }
}
