#include "Algorithm_322868852.h"

#include <queue>
#include <set>

#include "Logger.h"
#include "Direction.h"
#include "MyBattleInfo.h"
#include "TankAlgorithmRegistration.h"

using namespace Algorithm_322868852_340849710;

REGISTER_TANK_ALGORITHM(Algorithm_322868852)

// a boolean to indicate if we are stuck in the current BFS path and need to recompute the path
bool tried_path_without_success = false;
std::vector<Direction::DirectionType> current_path; // the current path we got from the BFS computation

void Algorithm_322868852::initLatestEnemyPosition() {
    last_enemy_positions = std::vector<Position>(battle_status.getEnemyTankCounts(), {-1, -1});
}

void Algorithm_322868852::updateLatestEnemyPosition() {
    last_enemy_positions = battle_status.getEnemyPositions();
}


void Algorithm_322868852::handleTankThreatened(ActionRequest *request, std::string *request_title) {
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

void Algorithm_322868852::tryShootEnemy(ActionRequest *request, std::string *request_title) {
    if (battle_status.canTankShootEnemy()) {
        tried_path_without_success = false;
        *request = ActionRequest::Shoot;
        *request_title = "Shoot Enemy";
        return;
    }
    rotateToEnemy(request, request_title);
}

bool Algorithm_322868852::rotateToEnemy(ActionRequest *request, std::string *request_title) const {
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

bool Algorithm_322868852::hasEnemyMoved() const {
    auto enemy_positions = battle_status.getEnemyPositions();
    if (enemy_positions.size() != last_enemy_positions.size()) {
        return true; // if the number of enemies changed, we definitely need to recompute the path
    }
    return enemy_positions == last_enemy_positions;
}

void Algorithm_322868852::updatePathIfNeeded() {
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

void Algorithm_322868852::handleEmptyPath(ActionRequest *request, std::string *request_title) const {
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


void Algorithm_322868852::followPathOrRotate(ActionRequest *request, std::string *request_title) {
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
        *request_title = "Moving forward to (" + std::to_string(next_pos.x) + "," + std::to_string(next_pos.y) +
                         ")";
        *request = ActionRequest::MoveForward;
        return;
    }

    // if he isn't able to do anything, just rotate and maybe it will help in the next steps
    tried_path_without_success = false;
    *request_title = "Rotating from direction " + Direction::directionToString(battle_status.tank_direction) +
                     " to " +
                     Direction::directionToString(target_dir);
    *request = battle_status.rotateTowards(target_dir);
}


std::vector<Direction::DirectionType> Algorithm_322868852::computeBFS() {
    struct Node {
        Position pos;
        std::vector<Direction::DirectionType> path;

        Node(const Position &pos, const std::vector<Direction::DirectionType> &path) : pos(pos), path(path) {
        }
    };
    std::string msg = "Performing BFS. Start Position = " + battle_status.tank_position.toString();
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

void Algorithm_322868852::calculateAction(ActionRequest *request, std::string *request_title) {
    if (battle_status.turn_number == 0 || was_threatened) {
        was_threatened = false;
        *request = ActionRequest::GetBattleInfo;
        *request_title = "Requesting Battle Info (first turn or previously threatened)";
        return;
    }
    if (isTankThreatened()) {
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

void Algorithm_322868852::updateBattleInfo(BattleInfo &info) {
    if (const MyBattleInfo *my_battle_info = dynamic_cast<MyBattleInfo *>(&info)) {
        battle_status.updateBoard(my_battle_info->getBoard());
        battle_status.num_shells = my_battle_info->getNumShells();
        battle_status.max_steps = my_battle_info->getMaxSteps();
    }
}

void Algorithm_322868852::printLogs(const std::string &msg) const {
    Logger::getInstance().log("Player " + std::to_string(player_id) + " - Tank Index " +
                              std::to_string(tank_index) + " " + msg);
}

ActionRequest Algorithm_322868852::getAction() {
    ActionRequest request = ActionRequest::DoNothing;
    std::string request_title = "Doing nothing";
    calculateAction(&request, &request_title);
    battle_status.turn_number++;
    printLogs(request_title);
    battle_status.updateBattleStatusBaseAction(request);
    return request;
}

bool Algorithm_322868852::isTankThreatened() const {
    if (battle_status.isShellClose()) {
        printLogs("Threatened by shells");
        return true;
    }
    if (battle_status.isEnemyClose()) {
        printLogs("Threatened by enemy");
        return true;
    }
    return false;
}

ActionRequest Algorithm_322868852::moveIfThreatened() const {
    // we'll try first moving forward in the current direction
    Position forward_pos = battle_status.wrapPosition(battle_status.tank_position + battle_status.tank_direction);

    if (battle_status.isSafePosition(forward_pos)) {
        return ActionRequest::MoveForward;
    }

    // If we can't move forward in the current direction, we'll find a safe cell around us and rotate towards it
    for (int i = 0; i < Direction::getDirectionSize(); ++i) {
        Direction::DirectionType possible_dir = Direction::getDirectionFromIndex(i);
        Position possible_pos = battle_status.wrapPosition(battle_status.tank_position + possible_dir);
        if (battle_status.isSafePosition(possible_pos)) {
            return battle_status.rotateTowards(possible_dir);
        }
    }

    if (battle_status.isSafePosition(forward_pos, true)) {
        return ActionRequest::MoveForward;
    }

    return ActionRequest::DoNothing;
}

