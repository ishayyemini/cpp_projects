#include "player1_algo.h"

#include <iostream>
#include <unordered_set>

#include "game/action.h"
#include "algorithm.h"


Player1Algo::Player1Algo(GameBoard &board)
    : Algorithm(board), board_graph(board) {
}

Action Player1Algo::getNextAction() {
    const auto &player2_tank = board.getPlayerTank(2, 0);
    const auto &player1_tank = board.getPlayerTank(1, 1);

    if (!player2_tank || !player1_tank) {
        return NONE;
    }

    if (isTankThreaten(player1_tank->getPosition())) {
        const Action escape_action = escape();
        if (escape_action == NONE) {
            return suicide_mission(); //aka - try to get tie at least
        }
        return escape_action;
    }

    return chase();
}


Action Player1Algo::escape() {
    // TODO: Implement escape logic
    return NONE;
}

Action Player1Algo::chase() {
    const auto &player2_tank = board.getPlayerTank(2, 0);
    const auto &player1_tank = board.getPlayerTank(1, 1);

    const Direction::DirectionType dir = bfsToOpponent(player1_tank->getPosition(), player2_tank->getPosition());

    const auto current_dir = player1_tank->getDirection();
    if (dir == current_dir) {
        return FORWARD;
    }

    int needed_turn = current_dir - dir;
    if (needed_turn > 180) needed_turn -= 360;

    if (abs(needed_turn) <= 90) {
        switch (needed_turn) {
            case -90:
                return ROTATE_90_RIGHT;
            case -45:
                return ROTATE_45_RIGHT;
            case 45:
                return ROTATE_45_LEFT;
            case 90:
                return ROTATE_90_LEFT;
            default:
                return NONE;
        }
    }

    if (abs(current_dir + 90 - needed_turn) < abs(current_dir - 90 - needed_turn))
        return ROTATE_90_RIGHT;

    return ROTATE_90_LEFT;
}

Action Player1Algo::suicide_mission() {
    // TODO: Implement aggressive action when no better options are available
    return Action();
}

Direction::DirectionType Player1Algo::bfsToOpponent(const std::pair<int, int> &tankPos,
                                                    const std::pair<int, int> &targetPos) const {
    if (tankPos == targetPos) {
        return Direction::UP;
    }

    // Define the BFS queue and visited set
    std::queue<std::pair<std::pair<int, int>, int> > queue;
    std::vector<std::pair<int, int> > visited;

    // Initialize the BFS with the starting position
    queue.emplace(tankPos, -1);
    visited.push_back(tankPos);

    while (!queue.empty()) {
        auto [currentPos, initialDir] = queue.front();
        queue.pop();

        for (int i = 0; i < Direction::getDirectionSize(); ++i) {
            Direction::DirectionType dir = Direction::getDirection(i * 45);
            std::pair<int, int> neighbor = calcNextPos(currentPos, dir);

            if (std::ranges::find(visited, neighbor) != visited.end()
                || !board_graph.isValidCell(neighbor)) {
                continue;
            }

            // Mark as visited
            visited.push_back(neighbor);

            // If the target is found, return the initial direction
            if (neighbor == targetPos) {
                return initialDir == -1 ? dir : Direction::getDirection(initialDir * 45);
            }

            // Add to the queue with the initial direction
            queue.emplace(neighbor, initialDir == -1 ? dir / 45 : initialDir);
        }
    }

    return Direction::UP; // Default direction if no path is found
}
