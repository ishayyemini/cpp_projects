//
// Created by Ishay Yemini on 23/04/2025.
//

#include "algo/player2_algo.h"

int Player2Algo::calcDistance(const std::pair<int, int> &pos1, const std::pair<int, int> &pos2) {
    return std::max(std::abs(pos2.first - pos1.first),
                    std::abs(pos2.second - pos1.second));
}

Action Player2Algo::getNextAction() {
    const auto &player2_tank = board.getPlayerTank(2);
    const auto &player1_tank = board.getPlayerTank(1);

    if (!player2_tank || !player1_tank) {
        return NONE;
    }

    const auto player2_pos = player2_tank->getPosition();
    const auto player1_pos = player1_tank->getPosition();
    const auto player2_dir = player2_tank->getDirection();
    const int curr_dist = calcDistance(player1_pos, player2_pos);

    // Check for clear shot to opponent
    if (player2_tank->getShootingCooldown() == 0 && player2_tank->getRemainingShell() > 0) {
        // Check if already aimed at opponent
        std::pair<int, int> checkPos = player2_pos;
        bool clearShot = false;
        // Trace line of sight to check for clear shot
        for (int i = 0; i < 10; i++) {
            checkPos = calcNextPos(checkPos, player2_dir);
            if (checkPos == player1_pos) {
                clearShot = true;
                break;
            }
            if (board.getBoardElement(checkPos) != nullptr) {
                break; // Hit obstacle
            }
        }
        if (clearShot) {
            return SHOOT;
        }
        // Try to rotate for a shot if player1 is nearby
        if (curr_dist <= 5) {
            for (const auto &rotation: {
                     std::make_pair(ROTATE_45_LEFT, -45), std::make_pair(ROTATE_45_RIGHT, 45),
                     std::make_pair(ROTATE_90_LEFT, -90),
                     std::make_pair(ROTATE_90_RIGHT, 90)
                 }) {
                Direction::DirectionType newDir = Direction::getDirection(player2_dir + rotation.second);
                checkPos = player2_pos;
                clearShot = false;
                for (int i = 0; i < 10; i++) {
                    checkPos = calcNextPos(checkPos, newDir);
                    if (checkPos == player1_pos) {
                        clearShot = true;
                        break;
                    }
                    if (board.getBoardElement(checkPos) != nullptr) {
                        break;
                    }
                }
                if (clearShot) {
                    return rotation.first;
                }
            }
        }
    }
    // Avoid shells - check nearby positions for shells
    for (int i = 0; i < 8; i++) {
        Direction::DirectionType checkDir = Direction::getDirection(i * 45);
        std::pair<int, int> nearPos = calcNextPos(player2_pos, checkDir);
        auto elem = board.getBoardElement(nearPos);
        if (elem && elem->getSymbol() == '*') {
            // Shell nearby
            // Move away from shell
            Direction::DirectionType escapeDir = Direction::getDirection((i + 4) % 8 * 45); // Opposite direction
            std::pair<int, int> escapePos = calcNextPos(player2_pos, escapeDir);
            if (board.getBoardElement(escapePos) == nullptr) {
                if (player2_dir == escapeDir) {
                    return FORWARD;
                }
                // Determine rotation to escape direction
                int angle_diff = ((escapeDir - player2_dir) + 360) % 360;
                if (angle_diff == 45 || angle_diff == 315) return ROTATE_45_RIGHT;
                if (angle_diff == 90 || angle_diff == 270) return ROTATE_90_RIGHT;
                if (angle_diff == 135 || angle_diff == 225) return ROTATE_45_LEFT;
                if (angle_diff == 180) return ROTATE_90_LEFT;
            }
        }
    }
    // Run away from player1 if too close
    if (curr_dist <= 5) {
        // Find direction that maximizes distance
        Action bestAction = NONE;
        int maxDist = curr_dist;
        // Try forward
        std::pair<int, int> forwardPos = calcNextPos(player2_pos, player2_dir);
        if (board.getBoardElement(forwardPos) == nullptr) {
            int dist = calcDistance(player1_pos, forwardPos);
            if (dist > maxDist) {
                maxDist = dist;
                bestAction = FORWARD;
            }
        }
        // Try rotations
        for (const auto &rotation: {
                 std::make_pair(ROTATE_45_LEFT, -45), std::make_pair(ROTATE_45_RIGHT, 45),
                 std::make_pair(ROTATE_90_LEFT, -90),
                 std::make_pair(ROTATE_90_RIGHT, 90)
             }) {
            Direction::DirectionType newDir = Direction::getDirection(player2_dir + rotation.second);
            std::pair<int, int> newPos = calcNextPos(player2_pos, newDir);
            if (board.getBoardElement(newPos) == nullptr) {
                int dist = calcDistance(player1_pos, newPos);
                if (dist > maxDist) {
                    maxDist = dist;
                    bestAction = rotation.first;
                }
            }
        }
        if (bestAction != NONE) {
            return bestAction;
        }
    }

    // If escape impossible and opponent is very close, crash into it
    if (curr_dist <= 2) {
        // Calculate direction to player1
        int dx = player1_pos.first - player2_pos.first;
        int dy = player1_pos.second - player2_pos.second;
        Direction::DirectionType dirToPlayer1;
        if (abs(dx) > abs(dy)) {
            dirToPlayer1 = dx > 0 ? Direction::RIGHT : Direction::LEFT;
        } else {
            dirToPlayer1 = dy > 0 ? Direction::DOWN : Direction::UP;
        }
        // If already facing player1, move forward
        if (dirToPlayer1 == player2_dir) {
            return FORWARD;
        }
        // Rotate toward player1
        int angle_diff = ((dirToPlayer1 - player2_dir) + 360) % 360;
        if (angle_diff <= 45 || angle_diff >= 315) return FORWARD;
        if (angle_diff < 90) return ROTATE_45_RIGHT;
        if (angle_diff == 90) return ROTATE_90_RIGHT;
        if (angle_diff < 180) return ROTATE_90_RIGHT;
        if (angle_diff == 180) return ROTATE_90_RIGHT; // Either direction works
        if (angle_diff < 270) return ROTATE_90_LEFT;
        if (angle_diff == 270) return ROTATE_90_LEFT;
        return ROTATE_45_LEFT;
    }

    // Default behavior: move forward
    std::pair<int, int> forwardPos = calcNextPos(player2_pos, player2_dir);
    if (board.getBoardElement(forwardPos) == nullptr) {
        return FORWARD;
    }

    // Or rotate if can't move forward
    return ROTATE_45_RIGHT;
}
