#include "GameManager.h"

#include <iostream>
#include <thread>
#include <chrono>

#include "Mine.h"
#include "Shell.h"

using namespace std::chrono_literals;


void GameManager::tankAction(Tank &tank, const Action action) {
    const int back_counter = tank.getBackwardsCounter();

    /* 3 -> "regular". Here, any action will perform normally, except for back, which will just dec the counter.
     * 2 -> "waiting". Don't move. If FORWARD, reset counter. Else, dec counter.
     * 1 -> "almost moved". If FORWARD, reset counter. Else, move back and dec counter.
     * 0 -> "moved". If back, move back. Else, perform regular action and reset counter.
     */

    if (back_counter == 2) {
        if (action == MOVE_FORWARD) {
            tank.setBackwardsCounter(3);
        } else {
            tank.setBackwardsCounter(back_counter - 1);
        }
    }

    if (back_counter == 1) {
        if (action == MOVE_FORWARD) {
            tank.setBackwardsCounter(3);
        } else {
            moveBackward(tank);
            tank.setBackwardsCounter(back_counter - 1);
        }
    }

    switch (action) {
        case MOVE_FORWARD:
            moveForward(tank);
            if (back_counter == 0) tank.setBackwardsCounter(3);
            break;
        case MOVE_BACKWARD:
            if (back_counter == 0) {
                moveBackward(tank);
            } else {
                tank.setBackwardsCounter(back_counter - 1);
            }
            break;
        case ROTATE_LEFT_EIGHTH:
            rotate(tank, -45);
            break;
        case ROTATE_RIGHT_EIGHTH:
            rotate(tank, 45);
            break;
        case ROTATE_LEFT_QUARTER:
            rotate(tank, -90);
            break;
        case ROTATE_RIGHT_QUARTER:
            rotate(tank, 90);
            break;
        case SHOOT:
            shoot(tank);
            break;
        default: ;
    }

    tank.decreaseShootingCooldown();
}

void GameManager::checkDeaths() {
    // Check if tanks are both destroyed
    const bool firstDead = board.getPlayerTank(1) == nullptr || board.getPlayerTank(1)->isDestroyed();
    const bool secondDead = board.getPlayerTank(2) == nullptr || board.getPlayerTank(2)->isDestroyed();

    if (firstDead && secondDead) {
        winner = TIE;
        game_over = true;
    }
    if (firstDead) {
        winner = PLAYER_2;
        game_over = true;
    }
    if (secondDead) {
        winner = PLAYER_1;
        game_over = true;
    }
}

bool GameManager::moveForward(Tank &tank) {
    return board.moveObject(tank.getPosition(), tank.getDirection());
}

bool GameManager::moveBackward(Tank &tank) {
    return board.moveObject(tank.getPosition(), -tank.getDirection());
}

bool GameManager::rotate(Tank &tank, const int turn) {
    const int new_direction = tank.getDirection() + turn;
    tank.setDirection(Direction::getDirection(new_direction));
    return true;
}

bool GameManager::shoot(Tank &tank) {
    if (tank.getAmmunition() == 0) {
        return false;
    }
    if (tank.getCooldown() == 0) {
        return false;
    }

    tank.decrementAmmunition();
    board.placeObject(std::make_unique<Shell>(tank.getPosition() + tank.getDirection(), tank.getDirection(),
                                              tank.getPlayerId()));
    return true;
}

void GameManager::tanksTurn() {
    const auto game_state_1 = GameState(board, 1);
    const auto game_state_2 = GameState(board, 2);

    Tank *t1 = game_state_1.getPlayerTank();
    if (t1 == nullptr) return;
    const Action a1 = algo1.decideAction(game_state_1);

    Tank *t2 = game_state_2.getPlayerTank();
    if (t2 == nullptr) return;
    const Action a2 = algo2.decideAction(game_state_2);

    std::cout << a1 << " " << a2 << std::endl;

    tankAction(*t1, a1);
    tankAction(*t2, a2);

    if (empty_countdown != -1) {
        empty_countdown--;
    }

    // Check if both tanks used their shells
    if (empty_countdown == -1 && t1->getAmmunition() == 0 && t2->getAmmunition() == 0) {
        empty_countdown = 40;
    }
}

void GameManager::shellsTurn() {
    // std::vector<MovingObject> shell_movements;
    //
    // // Collect all shell movements
    // for (int i = 0; i < board.getShellsCount(); i++) {
    //     const Shell *shell = board.getShell(i);
    //     if (shell) {
    //         Position nextPos = calcNextPos(shell->getPosition(), shell->getDirection());
    //         shell_movements.push_back({MovingObject::SHELL, i, shell->getPosition(), nextPos});
    //     }
    // }
    //
    // // Process shell movements
    // detectAndHandleCollisions(shell_movements);
    // applyMovements(shell_movements);
}

void GameManager::processStep() {
    if (game_over) return;
    game_step++;

    if (empty_countdown == 0) {
        winner = TIE;
        game_over = true;
        return;
    }

    shellsTurn();
    board.finishMove();

    board.displayBoard();
    std::this_thread::sleep_for(500ms);

    shellsTurn();
    tanksTurn();
    board.finishMove();

    board.displayBoard();
    std::this_thread::sleep_for(500ms);

    checkDeaths();
}

std::string GameManager::getGameResult() const {
    switch (winner) {
        case TIE:
            return "Tie";
        case PLAYER_1:
            return "Player 1 wins";
        case PLAYER_2:
            return "Player 2 wins";
        default:
            return "Bad result";
    }
}
