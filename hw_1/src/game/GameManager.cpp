#include "GameManager.h"

#include <iostream>
#include <thread>
#include <chrono>

#include "Logger.h"
#include "Mine.h"
#include "Shell.h"

using namespace std::chrono_literals;


bool GameManager::tankAction(Tank &tank, const Action action) {
    bool result = false;
    const int back_counter = tank.getBackwardsCounter();
    tank.decreaseShootingCooldown();

    if (action == NONE) result = true;

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
            result = moveBackward(tank);
            tank.setBackwardsCounter(back_counter - 1);
        }
    }

    switch (action) {
        case MOVE_FORWARD:
            result = moveForward(tank);
            if (back_counter == 0) tank.setBackwardsCounter(3);
            break;
        case MOVE_BACKWARD:
            if (back_counter == 0) {
                result = moveBackward(tank);
            } else {
                tank.setBackwardsCounter(back_counter - 1);
            }
            break;
        case ROTATE_LEFT_EIGHTH:
            result = rotate(tank, -45);
            break;
        case ROTATE_RIGHT_EIGHTH:
            result = rotate(tank, 45);
            break;
        case ROTATE_LEFT_QUARTER:
            result = rotate(tank, -90);
            break;
        case ROTATE_RIGHT_QUARTER:
            result = rotate(tank, 90);
            break;
        case SHOOT:
            result = shoot(tank);
            break;
        default: ;
    }

    return result;
}

void GameManager::checkDeaths() {
    // Check if tanks are both destroyed
    const bool firstDead = board.getPlayerTank(1) == nullptr || board.getPlayerTank(1)->isDestroyed();
    const bool secondDead = board.getPlayerTank(2) == nullptr || board.getPlayerTank(2)->isDestroyed();

    if (firstDead && secondDead) {
        winner = TIE;
        game_over = true;
        return;
    }
    if (firstDead) {
        winner = PLAYER_2;
        game_over = true;
        return;
    }
    if (secondDead) {
        winner = PLAYER_1;
        game_over = true;
        return;
    }

    if (empty_countdown == 0) {
        winner = TIE_AMMO;
        game_over = true;
    }
}

bool GameManager::moveForward(Tank &tank) {
    if (const auto obj = board.getObjectAt(tank.getPosition() + tank.getDirection())) {
        if (obj->getSymbol() == '#') return false;
    }
    return board.moveObject(tank.getPosition(), tank.getDirection());
}

bool GameManager::moveBackward(Tank &tank) {
    if (const auto obj = board.getObjectAt(tank.getPosition() + tank.getDirection())) {
        if (obj->getSymbol() == '#') return false;
    }
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
    if (tank.getCooldown() != 0) {
        return false;
    }

    tank.decrementAmmunition();
    tank.setCooldown(3);
    board.placeObject(std::make_unique<Shell>(tank.getPosition() + tank.getDirection(), tank.getDirection(),
                                              tank.getId()));
    return true;
}

void GameManager::tanksTurn() {
    const auto game_state_1 = GameState(board, 1);
    const auto game_state_2 = GameState(board, 2);

    Tank *t1 = game_state_1.getPlayerTank();
    if (t1 == nullptr) return;
    const Action a1 = algo1->decideAction(game_state_1);

    Tank *t2 = game_state_2.getPlayerTank();
    if (t2 == nullptr) return;
    const Action a2 = algo2->decideAction(game_state_2);

    step_history.push_back(action_strings[a1]);
    step_history.push_back(action_strings[a2]);

    const bool g1 = tankAction(*t1, a1);
    const bool g2 = tankAction(*t2, a2);

    Logger::getInstance().log("Player 1 Action: " + action_strings[a1] + (!g1 ? " [Bad Step]" : ""));
    Logger::getInstance().log("Player 2 Action: " + action_strings[a2] + (!g2 ? " [Bad Step]" : ""));


    // Check if both tanks used their shells
    if (empty_countdown == -1 && t1->getAmmunition() == 0 && t2->getAmmunition() == 0) {
        empty_countdown = 40;
    }

    if (empty_countdown != -1) {
        empty_countdown--;
    }
}

void GameManager::shellsTurn() const {
    for (auto [id, shell]: board.getShells()) {
        board.moveObject(shell->getPosition(), shell->getDirection());
    }
}

void GameManager::processStep() {
    if (game_over) return;
    game_step++;

    shellsTurn();
    board.finishMove();

    if (visual) {
        std::cout << "Step " << game_step << std::endl;
    }

    shellsTurn();
    tanksTurn();
    board.finishMove();

    if (visual) {
        board.displayBoard();
    }

    checkDeaths();
}

std::string GameManager::getGameResult() const {
    switch (winner) {
        case TIE:
            return "Tie - Both tanks are destroyed";
        case TIE_AMMO:
            return "Tie - No more ammunition";
        case PLAYER_1:
            return "Player 1 wins";
        case PLAYER_2:
            return "Player 2 wins";
        default:
            return "Bad result";
    }
}
