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
    const auto &player2_tank = board.getPlayerTank(2);
    const auto &player1_tank = board.getPlayerTank(1);

    const Direction::DirectionType dir = bfsToOpponent(player1_tank->getPosition(), player2_tank->getPosition());
    std::cout << dir << std::endl;
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
