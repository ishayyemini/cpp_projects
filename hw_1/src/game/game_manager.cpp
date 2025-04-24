#include "game/game_manager.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <utility>

#include "algo/algorithm.h"
#include "algo/player1_algo.h"
#include "algo/player2_algo.h"
#include "utils/file_utils.h"

GameManager::GameManager(const std::string &input_file_path) {
    file_utils::loadBoard(board, input_file_path);
}

Winner GameManager::start_game() {
    using namespace std::chrono_literals;

    Player2Algo p2_algo(board);

    while (true) {
        if (empty_countdown == 0) {
            return TIE;
        }

        Action action = p2_algo.getNextAction();
        std::cout << action << std::endl;

        switch (action) {
            case FORWARD:
                moveForward(2, 0);
                break;
            case BACKWARD:
                moveBackward(2, 0);
                break;
            case ROTATE_45_LEFT:
                rotate(2, 0, -45);
                break;
            case ROTATE_45_RIGHT:
                rotate(2, 0, 45);
                break;
            case ROTATE_90_LEFT:
                rotate(2, 0, -90);
                break;
            case ROTATE_90_RIGHT:
                rotate(2, 0, 90);
                break;
            case SHOOT:
                shoot(2, 0);
                break;
            default: ;
        }

        moveForward(1, 1);

        std::this_thread::sleep_for(1000ms);

        board.displayBoard();

        // Check if both tanks used their shells
        if (empty_countdown == -1 && board.getPlayerTank(0)->getRemainingShell() == 0 &&
            board.getPlayerTank(1)->getRemainingShell() == 0) {
            empty_countdown = 40;
        }
        if (empty_countdown != 1) {
            empty_countdown--;
        }
    }

    return winner;
}

bool GameManager::moveForward(const int player_id, const int tank_id) {
    Tank *tank = board.getPlayerTank(player_id, tank_id);
    if (tank == nullptr) {
        std::cerr << "Can't get tank " << tank_id << " of player " << player_id << std::endl;
        return false;
    }

    if (tank->getBackwardsCounter() != 2 && tank->getBackwardsCounter() != 0) {
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

bool GameManager::rotate(const int player_id, const int tank_id, const int turn) {
    Tank *tank = board.getPlayerTank(player_id, tank_id);
    if (tank == nullptr) {
        std::cerr << "Can't get tank " << tank_id << " of player " << player_id << std::endl;
        return false;
    }
    if (turn != 45 && turn != 90 && turn != -45 && turn != -90) {
        std::cerr << "Got bad direction: " << turn << std::endl;
        return false;
    }
    if (tank->getBackwardsCounter() != 2) {
        tank->setBackwardsCounter(tank->getBackwardsCounter() - 1);
        return false;
    }
    if (tank->getBackwardsCounter() == 0) {
        tank->setBackwardsCounter(2);
    }

    int new_direction = tank->getCannonDirection() + turn;
    tank->setCannonDirection(static_cast<Direction::DirectionType>(new_direction));
    return true;
}

bool GameManager::shoot(int player_id, int tank_id) {
    Tank *tank = board.getPlayerTank(player_id, tank_id);
    if (tank == nullptr) {
        std::cerr << "Can't get tank " << tank_id << " of player " << player_id << std::endl;
        return false;
    }

    if (tank->getShootingCooldown() == 0) {
        tank->shoot();
        auto [fst, snd] = Direction::getOffset(tank->getCannonDirection());
        const std::pair new_position = {tank->getPosition().first + fst, tank->getPosition().second + snd};
        board.addShell(std::make_unique<Shell>(new_position, tank->getCannonDirection()));
        return true;
    }

    return false;
}
