#include "GameManager.h"

#include <iostream>
#include <thread>
#include <chrono>

#include "Logger.h"
#include "Mine.h"
#include "Shell.h"
#include "ActionRequest.h"

using namespace std::chrono_literals;

inline std::map<ActionRequest, std::string> action_strings = {
    {ActionRequest::DoNothing, "None"},
    {ActionRequest::MoveForward, "Move Forward"},
    {ActionRequest::MoveBackward, "Move Backward"},
    {ActionRequest::RotateLeft45, "Rotate Left Eighth"},
    {ActionRequest::RotateRight45, "Rotate Right Eighth"},
    {ActionRequest::RotateLeft90, "Rotate Left Quarter"},
    {ActionRequest::RotateRight90, "Rotate Right Quarter"},
    {ActionRequest::Shoot, "Shoot"},
};


bool GameManager::tankAction(Tank &tank, const ActionRequest action) {
    bool result = false;
    const int back_counter = tank.getBackwardsCounter();
    tank.decreaseShootingCooldown();

    if (action == ActionRequest::DoNothing) result = true;

    /* 3 -> "regular". Here, any action will perform normally, except for back, which will just dec the counter.
     * 2 -> "waiting". Don't move. If FORWARD, reset counter. Else, dec counter.
     * 1 -> "almost moved". If FORWARD, reset counter. Else, move back and dec counter.
     * 0 -> "moved". If back, move back. Else, perform regular action and reset counter.
     */

    if (back_counter == 2) {
        if (action == ActionRequest::MoveForward) {
            tank.setBackwardsCounter(3);
        } else {
            tank.setBackwardsCounter(back_counter - 1);
        }
    }

    if (back_counter == 1) {
        if (action == ActionRequest::MoveForward) {
            tank.setBackwardsCounter(3);
        } else {
            result = moveBackward(tank);
            tank.setBackwardsCounter(back_counter - 1);
        }
    }

    switch (action) {
        case ActionRequest::MoveForward:
            result = moveForward(tank);
            if (back_counter == 0) tank.setBackwardsCounter(3);
            break;
        case ActionRequest::MoveBackward:
            if (back_counter == 0) {
                result = moveBackward(tank);
            } else {
                tank.setBackwardsCounter(back_counter - 1);
            }
            break;
        case ActionRequest::RotateLeft45:
            result = rotate(tank, -45);
            break;
        case ActionRequest::RotateRight45:
            result = rotate(tank, 45);
            break;
        case ActionRequest::RotateLeft90:
            result = rotate(tank, -90);
            break;
        case ActionRequest::RotateRight90:
            result = rotate(tank, 90);
            break;
        case ActionRequest::Shoot:
            result = shoot(tank);
            break;
        default: ;
    }

    return result;
}

void GameManager::checkDeaths() {
    // TODO Check if tanks are both destroyed
    // const bool firstDead = board.getPlayerTank(1) == nullptr || board.getPlayerTank(1)->isDestroyed();
    // const bool secondDead = board.getPlayerTank(2) == nullptr || board.getPlayerTank(2)->isDestroyed();
    //
    // if (firstDead && secondDead) {
    //     winner = TIE;
    //     game_over = true;
    //     return;
    // }
    // if (firstDead) {
    //     winner = PLAYER_2;
    //     game_over = true;
    //     return;
    // }
    // if (secondDead) {
    //     winner = PLAYER_1;
    //     game_over = true;
    //     return;
    // }

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
    std::vector<ActionRequest> actions;
    for (size_t i = 0; i < tanks.size(); ++i) {
        actions.emplace_back(tanks[i]->getAction());
        step_history.push_back(action_strings[actions.back()]);
    }

    size_t i = 0;
    for (const auto tank: board.getTanks()) {
        if (actions.size() < i) {
            tankAction(*tank, actions[i]);
        }
    }

    // Logger::getInstance().log("Player 1 Action: " + action_strings[a1] + (!g1 ? " [Bad Step]" : ""));
    // Logger::getInstance().log("Player 2 Action: " + action_strings[a2] + (!g2 ? " [Bad Step]" : ""));


    // TODO Check if both players used their shells
    // if (empty_countdown == -1 && t1->getAmmunition() == 0 && t2->getAmmunition() == 0) {
    //     empty_countdown = 40;
    // }

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
