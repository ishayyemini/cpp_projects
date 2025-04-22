#include "game/game_manager.h"

#include <iostream>
#include <utility>

#include "utils/file_utils.h"

GameManager::GameManager(const std::string &input_file_path) {
    file_utils::loadBoard(board, input_file_path);
}

Winner GameManager::start_game() {
    moveForward(2, 0);
    board.displayBoard();

    moveForward(2, 0);
    board.displayBoard();
    return winner;
}

bool GameManager::moveForward(const int player_id, const int tank_id) {
    Tank *tank = board.getPlayerTank(player_id, tank_id);
    if (tank == nullptr) {
        std::cerr << "Can't get tank " << tank_id << " of player " << player_id << std::endl;
        return false;
    }

    if (tank->getBackwardsCounter() != 2 || tank->getBackwardsCounter() != 0) {
        tank->setBackwardsCounter(2);
        return false;
    }
    if (tank->getBackwardsCounter() == 0) {
        tank->setBackwardsCounter(2);
    }

    const Direction::DirectionType direction = tank->getCannonDirection();
    auto [x_offset, y_offset] = Direction::getOffset(direction);
    const std::pair new_position = {tank->getPosition().first + x_offset, tank->getPosition().second + y_offset};
    if (board.getBoardElement(new_position) != nullptr) {
        return false;
    }
    return board.moveBoardElement(tank->getPosition(), new_position);
}

bool GameManager::moveBackward(const int player_id, const int tank_id) {
    Tank *tank = board.getPlayerTank(player_id, tank_id);
    if (tank == nullptr) {
        std::cerr << "Can't get tank " << tank_id << " of player " << player_id << std::endl;
        return false;
    }

    if (tank->getBackwardsCounter() != 0) {
        tank->setBackwardsCounter(tank->getBackwardsCounter() - 1);
        return false;
    }

    const Direction::DirectionType direction = tank->getCannonDirection();
    auto [x_offset, y_offset] = Direction::getOffset(direction);
    const std::pair new_position = {tank->getPosition().first - x_offset, tank->getPosition().second - y_offset};
    if (board.getBoardElement(new_position) != nullptr) {
        return false;
    }
    return board.moveBoardElement(tank->getPosition(), new_position);
}
