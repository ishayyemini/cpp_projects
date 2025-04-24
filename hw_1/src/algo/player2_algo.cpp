//
// Created by Ishay Yemini on 23/04/2025.
//

#include "algo/player2_algo.h"

int Player2Algo::calcDistance(const std::pair<int, int> &pos1, const std::pair<int, int> &pos2) {
    return std::max(std::abs(pos2.first - pos1.first),
                    std::abs(pos2.second - pos1.second));
}

std::pair<int, int>
Player2Algo::calcNextPos(const std::pair<int, int> &pos, const Direction::DirectionType &dir) const {
    auto [dx, dy] = Direction::getOffset(dir);
    int height = board.getHeight();
    int width = board.getWidth();
    return {((pos.first + dx) % height + height) % height, ((pos.second + dy) % width + width) % width};
}


Action Player2Algo::getNextAction() {
    const auto &player2_tank = board.getPlayerTank(2, 0);
    const auto &player1_tank = board.getPlayerTank(1, 1);

    if (!player2_tank || !player1_tank) {
        return NONE;
    }

    const auto player2_pos = player2_tank->getPosition();
    const auto player1_pos = player1_tank->getPosition();

    const int curr_dist = calcDistance(player1_pos, player2_pos);
    if (curr_dist <= 2) {
        const std::pair forward_pos = calcNextPos(player2_pos, player2_tank->getCannonDirection());
        if (board.getBoardElement(forward_pos) == nullptr && calcDistance(player1_pos, forward_pos) > curr_dist) {
            return FORWARD;
        }

        for (const std::pair<int, Action> &turn: std::initializer_list<std::pair<int, Action> >{
                 {-90, ROTATE_90_LEFT}, {-45, ROTATE_45_LEFT}, {45, ROTATE_45_RIGHT}, {90, ROTATE_90_RIGHT}
             }) {
            const auto next_direction = static_cast<Direction::DirectionType>(
                player2_tank->getCannonDirection() + turn.first);
            const std::pair<int, int> next_pos = calcNextPos(player2_pos, next_direction);
            if (board.getBoardElement(next_pos) == nullptr && calcDistance(player1_pos, next_pos) > curr_dist) {
                return turn.second;
            }
        }

        if (forward_pos == player1_pos) {
            return FORWARD;
        }
    }

    // for (const auto &shell: board.getShells()) {
    //     const auto shell_pos = shell->getPosition();
    //     const auto shell_dir = shell->getDirection();
    //
    //     auto next_pos = calcNextPos(shell_pos, shell_dir);
    //     auto next_next_pos = calcNextPos(next_pos, shell_dir);
    //
    //     if (calcNextPos())
    //
    //     // Simulate shell trajectory
    //     while (board.isValidPosition(current_pos)) {
    //         if (current_pos == player2_pos) {
    //             // Try to escape
    //             for (const auto &[dx, dy]: Direction
    //
    //
    //             :
    //             ) {
    //                 const std::pair<int, int> new_pos = {player2_pos.first + dx, player2_pos.second + dy};
    //                 if (gameManager->getBoard().getBoardElement(new_pos) == nullptr) {
    //                     return Action::Move(new_pos);
    //                 }
    //             }
    //
    //             // If escape is not possible, do nothing
    //             return Action::NONE;
    //         }
    //         current_pos.first += shell_dir.first;
    //         current_pos.second += shell_dir.second;
    //     }
    // }

    return NONE;
}
