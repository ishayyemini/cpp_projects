#include "game/game_manager.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <utility>

#include "utils/file_utils.h"

GameManager::GameManager(const std::string &input_file_path) {
    file_utils::loadBoard(board, input_file_path);
}

std::pair<int, int>
GameManager::calcNextPos(const std::pair<int, int> &pos, const Direction::DirectionType &dir) const {
    auto [dx, dy] = Direction::getOffset(dir);
    const int height = board.getHeight();
    const int width = board.getWidth();
    return {((pos.first + dx) % height + height) % height, ((pos.second + dy) % width + width) % width};
}

std::pair<int, int>
GameManager::calcNextPos(const std::pair<int, int> &pos, const int dir_i) const {
    const auto dir = Direction::getDirection(dir_i);
    return calcNextPos(pos, dir);
}

Winner GameManager::startGame() {
    using namespace std::chrono_literals;

    Player1Algo p1_algo(board);
    Player2Algo p2_algo(board);

    board.displayBoard();
    std::this_thread::sleep_for(1000ms);

    int i = 0;
    while (true) {
        if (empty_countdown == 0) {
            return TIE;
        }

        if (i % 2 == 0) tanksTurn(p1_algo, p2_algo);
        shellsTurn();
        board.displayBoard();

        if (const Winner winner = checkDeaths(); winner != NO_WINNER) {
            return winner;
        }

        std::this_thread::sleep_for(500ms);
        i++;
    }

    return winner;
}

void GameManager::tankAction(Tank &tank, const Action action) {
    const int back_counter = tank.getBackwardsCounter();

    /* 3 -> "regular". Here, any action will perform normally, except for back, which will just dec the counter.
     * 2 -> "waiting". Don't move. If FORWARD, reset counter. Else, dec counter.
     * 1 -> "almost moved". If FORWARD, reset counter. Else, move back and dec counter.
     * 0 -> "moved". If back, move back. Else, perform regular action and reset counter.
     */

    if (back_counter == 2) {
        if (action == FORWARD) {
            tank.setBackwardsCounter(3);
        } else {
            tank.setBackwardsCounter(back_counter - 1);
        }
    }

    if (back_counter == 1) {
        if (action == FORWARD) {
            tank.setBackwardsCounter(3);
        } else {
            moveBackward(tank);
            tank.setBackwardsCounter(back_counter - 1);
        }
    }

    switch (action) {
        case FORWARD:
            moveForward(tank);
            if (back_counter == 0) tank.setBackwardsCounter(3);
            break;
        case BACKWARD:
            if (back_counter == 0) {
                moveBackward(tank);
            } else {
                tank.setBackwardsCounter(back_counter - 1);
            }
            break;
        case ROTATE_45_LEFT:
            rotate(tank, -45);
            break;
        case ROTATE_45_RIGHT:
            rotate(tank, 45);
            break;
        case ROTATE_90_LEFT:
            rotate(tank, -90);
            break;
        case ROTATE_90_RIGHT:
            rotate(tank, 90);
            break;
        case SHOOT:
            shoot(tank);
            break;
        default: ;
    }

    tank.decreaseShootingCooldown();
}

void GameManager::tanksTurn(Player1Algo &algo1, Player2Algo &algo2) {
    Tank *t1 = board.getPlayerTank(1);
    if (t1 == nullptr) return;
    const Action a1 = algo1.getNextAction();

    Tank *t2 = board.getPlayerTank(2);
    if (t2 == nullptr) return;
    const Action a2 = algo2.getNextAction();

    std::cout << a1 << " " << a2 << std::endl;

    tankAction(*t1, a1);
    tankAction(*t2, a2);

    // Check if both tanks used their shells
    if (empty_countdown == -1 && board.getPlayerTank(0)->getRemainingShell() == 0 &&
        board.getPlayerTank(1)->getRemainingShell() == 0) {
        empty_countdown = 40;
    }
    if (empty_countdown != 1) {
        empty_countdown--;
    }
}

void GameManager::shellsTurn() {
    const Shell *shell = board.getShell(0);
    for (int i = 0; shell != nullptr; i++) {
        const std::pair new_position = calcNextPos(shell->getPosition(), shell->getDirection());
        if (board.getBoardElement(new_position) != nullptr) {
            // TODO bomb!
        }
        board.moveShell(i, new_position);
        shell = board.getShell(i + 1);
    }
}

Winner GameManager::checkDeaths() const {
    // Check if tanks are both destroyed
    const bool firstDead = board.getPlayerTank(1) == nullptr || board.getPlayerTank(1)->isDestroyed();
    const bool secondDead = board.getPlayerTank(2) == nullptr || board.getPlayerTank(2)->isDestroyed();
    if (firstDead && secondDead) {
        return TIE;
    }
    if (firstDead) {
        return PLAYER_2;
    }
    if (secondDead) {
        return PLAYER_1;
    }
    return NO_WINNER;
}

bool GameManager::moveForward(Tank &tank) {
    const std::pair new_position = calcNextPos(tank.getPosition(), tank.getDirection());
    return board.moveTank(tank.getPosition(), new_position);
}

bool GameManager::moveBackward(Tank &tank) {
    const std::pair new_position = calcNextPos(tank.getPosition(), tank.getDirection() + 180);
    return board.moveTank(tank.getPosition(), new_position);
}

bool GameManager::rotate(Tank &tank, const int turn) {
    const int new_direction = tank.getDirection() + turn;
    tank.setDirection(Direction::getDirection(new_direction));
    return true;
}

bool GameManager::shoot(Tank &tank) {
    if (tank.getShootingCooldown() == 0) {
        tank.shoot();
        board.addShell(std::make_unique<Shell>(tank.getPosition(), tank.getDirection()));
        return true;
    }
    return false;
}
