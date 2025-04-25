#include "player1_algo.h"

#include <iostream>
#include <unordered_set>

#include "game/action.h"
#include "algorithm.h"


Player1Algo::Player1Algo(GameBoard &board)
    : Algorithm(board), board_graph(board) {
}

Action Player1Algo::getNextAction() {
    const auto &player2_tank = board.getPlayerTank(2);
    const auto &player1_tank = board.getPlayerTank(1);

    if (!player2_tank || !player1_tank) {
        return NONE;
    }

    // First priority: Shoot if we have a clear shot at the enemy
    if (player1_tank->getShootingCooldown() == 0 && player1_tank->getRemainingShell() > 0) {
        if (hasLineOfSightToEnemy()) {
            return SHOOT;
        }

        // Second priority: Shoot walls that are in our way
        if (canShootWall()) {
            return SHOOT;
        }
    }

    // Third priority: Chase the enemy aggressively
    return chase();
}

bool Player1Algo::isTankThreaten(const std::pair<int, int> &pos) const {
    const auto &player1_tank = board.getPlayerTank(1);
    const auto &player2_tank = board.getPlayerTank(2);
    if (!player1_tank || !player2_tank) {
        return false;
    }
    // Check if enemy has line of sight to us
    const auto enemy_pos = player2_tank->getPosition();
    const auto enemy_dir = player2_tank->getDirection();
    // Check for enemy line of sight
    std::pair<int, int> checkPos = enemy_pos;
    for (int i = 0; i < 10; i++) {
        checkPos = calcNextPos(checkPos, enemy_dir);
        if (checkPos == pos) {
            return true; // Enemy can see us directly
        }
        if (!board_graph.isValidCell(checkPos) || board.getBoardElement(checkPos) != nullptr) {
            break; // Line of sight blocked
        }
    }
    // Check for nearby shells (dangerous!)
    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++) {
            if (dx == 0 && dy == 0) continue;
            std::pair<int, int> nearPos = {pos.first + dy, pos.second + dx};
            if (!board_graph.isValidCell(nearPos)) continue;
            Shell *shell = board.getShell(nearPos);
            if (shell) {
                return true; // Shell nearby is threatening
            }
        }
    }
    // Check if enemy is too close (within 3 squares)
    int distance = abs(pos.first - enemy_pos.first) + abs(pos.second - enemy_pos.second);
    if (distance <= 3) {
        return true;
    }

    return false;
}


Action Player1Algo::escape() {
    const auto &player1_tank = board.getPlayerTank(1);
    const auto &player2_tank = board.getPlayerTank(2);
    if (!player1_tank || !player2_tank) {
        return NONE;
    }
    const auto tank_pos = player1_tank->getPosition();
    const auto enemy_pos = player2_tank->getPosition();
    const auto tank_dir = player1_tank->getDirection();
    // First priority: If we can shoot the enemy or a wall in our way, do it
    if (player1_tank->getShootingCooldown() == 0 && player1_tank->getRemainingShell() > 0) {
        if (hasLineOfSightToEnemy()) {
            return SHOOT;
        }
        if (canShootWall()) {
            return SHOOT;
        }
    }
    // Find direction that maximizes distance from enemy
    Direction::DirectionType best_dir = tank_dir;
    int max_distance = abs(tank_pos.first - enemy_pos.first) + abs(tank_pos.second - enemy_pos.second);
    Action best_action = NONE;
    // Check forward movement
    std::pair<int, int> next_pos = calcNextPos(tank_pos, tank_dir);
    if (board_graph.isValidCell(next_pos) && board.getBoardElement(next_pos) == nullptr && board.getShell(next_pos) ==
        nullptr) {
        int new_distance = abs(next_pos.first - enemy_pos.first) + abs(next_pos.second - enemy_pos.second);
        if (new_distance > max_distance) {
            max_distance = new_distance;
            best_action = FORWARD;
        }
    }
    // Check rotations and resulting forward movements
    const std::vector<std::pair<Action, int> > rotations = {
        {ROTATE_45_LEFT, 45},
        {ROTATE_45_RIGHT, -45},
        {ROTATE_90_LEFT, 90},
        {ROTATE_90_RIGHT, -90}
    };
    for (const auto &[action, angle]: rotations) {
        Direction::DirectionType new_dir = Direction::getDirection(tank_dir + angle);
        std::pair<int, int> rot_next_pos = calcNextPos(tank_pos, new_dir);
        if (board_graph.isValidCell(rot_next_pos) && board.getBoardElement(rot_next_pos) == nullptr && board.
            getShell(rot_next_pos) == nullptr) {
            int new_distance = abs(rot_next_pos.first - enemy_pos.first) + abs(rot_next_pos.second - enemy_pos.second);
            if (new_distance > max_distance) {
                max_distance = new_distance;
                best_action = action;
            }
        }
    }
    return best_action;
}

Action Player1Algo::suicide_mission() {
    const auto &player1_tank = board.getPlayerTank(1);
    const auto &player2_tank = board.getPlayerTank(2);
    if (!player1_tank || !player2_tank) {
        return NONE;
    }
    // If we have a clear shot, take it
    if (player1_tank->getShootingCooldown() == 0 && player1_tank->getRemainingShell() > 0) {
        if (hasLineOfSightToEnemy()) {
            return SHOOT;
        }
    }
    // Otherwise, try to ram the enemy
    const auto tank_pos = player1_tank->getPosition();
    const auto enemy_pos = player2_tank->getPosition();
    const auto tank_dir = player1_tank->getDirection();
    // Calculate direction to enemy
    int dx = enemy_pos.second - tank_pos.second;
    int dy = enemy_pos.first - tank_pos.first;
    Direction::DirectionType dir_to_enemy;
    if (abs(dx) > abs(dy)) {
        dir_to_enemy = dx > 0 ? Direction::RIGHT : Direction::LEFT;
    } else {
        dir_to_enemy = dy > 0 ? Direction::DOWN : Direction::UP;
    }
    // If already facing enemy, charge!
    if (dir_to_enemy == tank_dir) {
        return FORWARD;
    }
    // Otherwise rotate toward enemy
    int angle_diff = ((dir_to_enemy - tank_dir) + 360) % 360;
    if (angle_diff <= 45 || angle_diff >= 315) {
        return FORWARD;
    } else if (angle_diff <= 90) {
        return ROTATE_45_RIGHT;
    } else if (angle_diff <= 135) {
        return ROTATE_90_RIGHT;
    } else if (angle_diff <= 225) {
        return ROTATE_90_LEFT;
    } else if (angle_diff <= 270) {
        return ROTATE_90_LEFT;
    } else {
        return ROTATE_45_LEFT;
    }
}

Direction::DirectionType Player1Algo::bfsToOpponent(const std::pair<int, int> &tankPos,
                                                    const std::pair<int, int> &targetPos) const {
    if (tankPos == targetPos) {
        return Direction::UP; // Already at target
    }

    const auto &player1_tank = board.getPlayerTank(1);
    Direction::DirectionType currentTankDirection = player1_tank->getDirection();

    // State for search algorithm
    struct State {
        std::pair<int, int> pos;
        Direction::DirectionType dir;
        int cost;
        Direction::DirectionType initialDir;

        bool operator>(const State &other) const {
            return cost > other.cost;
        }
    };
    // Priority queue for Dijkstra's algorithm
    std::priority_queue<State, std::vector<State>, std::greater<> > pq;
    // Track visited states with their costs
    std::map<std::pair<std::pair<int, int>, Direction::DirectionType>, int> visited;
    // Calculate rotation cost between two directions
    auto getRotationCost = [](Direction::DirectionType from, Direction::DirectionType to) {
        if (from == to) return 0;
        int diff = abs(from - to);
        if (diff > 180) diff = 360 - diff;
        // Based on rotation requirements
        return diff > 0 ? 2 : 0; // Any rotation takes 2 game turns
    };
    // Try all possible initial directions
    for (int i = 0; i < Direction::getDirectionSize(); ++i) {
        Direction::DirectionType initialDir = Direction::getDirection(i * 45);
        int rotationCost = getRotationCost(currentTankDirection, initialDir);
        std::pair<int, int> nextPos = calcNextPos(tankPos, initialDir);
        if (!board_graph.isValidCell(nextPos)) {
            continue;
        }
        pq.push({nextPos, initialDir, rotationCost + 1, initialDir});
        visited[{nextPos, initialDir}] = rotationCost + 1;
    }
    while (!pq.empty()) {
        State current = pq.top();
        pq.pop();
        if (visited[{current.pos, current.dir}] < current.cost) {
            continue;
        }
        if (current.pos == targetPos) {
            return current.initialDir;
        }
        for (int i = 0; i < Direction::getDirectionSize(); ++i) {
            Direction::DirectionType newDir = Direction::getDirection(i * 45);
            const int rotationCost = getRotationCost(current.dir, newDir);
            std::pair<int, int> nextPos = calcNextPos(current.pos, newDir);
            if (!board_graph.isValidCell(nextPos)) {
                continue;
            }
            int nextCost = current.cost + rotationCost + 1;
            auto nextState = std::make_pair(nextPos, newDir);
            if (visited.find(nextState) != visited.end() && visited[nextState] <= nextCost) {
                continue;
            }

            visited[nextState] = nextCost;
            pq.push({nextPos, newDir, nextCost, current.initialDir});
        }
    }

    return Direction::UP; // Default if no path found
}

bool Player1Algo::hasLineOfSightToEnemy() const {
    const auto &player1_tank = board.getPlayerTank(1);
    const auto &player2_tank = board.getPlayerTank(2);

    if (!player1_tank || !player2_tank) {
        return false;
    }

    const auto tank_pos = player1_tank->getPosition();
    const auto enemy_pos = player2_tank->getPosition();
    const auto tank_dir = player1_tank->getDirection();

    std::pair<int, int> checkPos = tank_pos;

    for (int i = 0; i < 20; i++) {
        // Check up to 20 squares ahead
        checkPos = calcNextPos(checkPos, tank_dir);
        if (checkPos == enemy_pos) {
            return true; // Found enemy tank
        }
        if (!board_graph.isValidCell(checkPos) || board.getBoardElement(checkPos) != nullptr) {
            return false; // Hit obstacle or wall
        }
    }
    return false;
}

bool Player1Algo::canShootWall() const {
    const auto &player1_tank = board.getPlayerTank(1);
    if (!player1_tank) {
        return false;
    }
    const auto tank_pos = player1_tank->getPosition();
    const auto tank_dir = player1_tank->getDirection();
    std::pair<int, int> checkPos = tank_pos;
    for (int i = 1; i <= 5; i++) {
        // Check up to 5 squares ahead
        checkPos = calcNextPos(checkPos, tank_dir);
        if (!board_graph.isValidCell(checkPos)) {
            return false;
        }
        auto element = board.getBoardElement(checkPos);
        if (element != nullptr) {
            return element->getSymbol() == '#'; // Found a wall to shoot
        }
    }

    return false;
}

Action Player1Algo::chase() {
    const auto &player2_tank = board.getPlayerTank(2);
    const auto &player1_tank = board.getPlayerTank(1);

    // Only recalculate path if enemy moved or we don't have a cached direction
    Direction::DirectionType dir;
    if (last_enemy_pos != player2_tank->getPosition() || cached_direction == Direction::UP) {
        dir = bfsToOpponent(player1_tank->getPosition(), player2_tank->getPosition());
        cached_direction = dir;
        last_enemy_pos = player2_tank->getPosition();
    } else {
        dir = cached_direction;
    }

    const auto current_dir = player1_tank->getDirection();
    if (dir == current_dir) {
        return FORWARD;
    }

    int needed_turn = current_dir - dir;
    if (needed_turn > 180) needed_turn -= 360;
    if (needed_turn < -180) needed_turn += 360;

    if (abs(needed_turn) <= 90) {
        switch (needed_turn) {
            case -90: return ROTATE_90_RIGHT;
            case -45: return ROTATE_45_RIGHT;
            case 45: return ROTATE_45_LEFT;
            case 90: return ROTATE_90_LEFT;
            default: return NONE;
        }
    }

    if (abs(current_dir + 90 - needed_turn) < abs(current_dir - 90 - needed_turn))
        return ROTATE_90_RIGHT;

    return ROTATE_90_LEFT;
}
