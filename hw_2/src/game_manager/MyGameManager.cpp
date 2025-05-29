#include "MyGameManager.h"

#include <iostream>
#include <thread>
#include <chrono>

#include "Logger.h"
#include "Mine.h"
#include "Shell.h"
#include "ActionRequest.h"
#include "InputParser.h"
#include "MySatelliteView.h"

using namespace std::chrono_literals;

void MyGameManager::readBoard(const std::string &file_name) {
    auto input_parser = InputParser();

    board = input_parser.parseInputFile(file_name);

    if (board == nullptr) {
        board = make_unique<Board>();
        std::cerr << "Can't parse file " << file_name << std::endl;
    }

    for (int i = 1; i <= 2; i++)
        players.emplace_back(playerFactory.create(i, board->getWidth(), board->getHeight(), board->getMaxSteps(),
                                                  board->getNumShells()));

    for (auto [player_i, tank_i]: input_parser.getTanks()) {
        tanks.emplace_back(tankAlgorithmFactory.create(player_i, tank_i));
    }
}

void MyGameManager::run() {
    while (!isGameOver()) {
        processStep();
        if (visual) std::this_thread::sleep_for(200ms);
    }

    Logger::getInstance().log(getGameResult());
    if (visual) {
        std::cout << getGameResult() << std::endl;
    }
}


bool MyGameManager::tankAction(Tank &tank, const ActionRequest action) {
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
        case ActionRequest::GetBattleInfo:
            result = getBattleInfo(tank.getTankAlgoIndex(), tank.getPlayerIndex());
            break;
        default: ;
    }

    return result;
}

bool MyGameManager::checkNoTanks(const int player_index) const {
    if (!board) return true;
    for (const auto tank: board->getPlayerTanks(player_index)) {
        if (!tank->isDestroyed()) return false;
    }
    return true;
}

void MyGameManager::checkDeaths() {
    const bool firstDead = checkNoTanks(1);
    const bool secondDead = checkNoTanks(2);

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

bool MyGameManager::moveForward(Tank &tank) {
    if (const auto obj = board->getObjectAt(tank.getPosition() + tank.getDirection())) {
        if (obj->getSymbol() == '#') return false;
    }
    return board->moveObject(tank.getPosition(), tank.getDirection());
}

bool MyGameManager::moveBackward(Tank &tank) {
    if (const auto obj = board->getObjectAt(tank.getPosition() + tank.getDirection())) {
        if (obj->getSymbol() == '#') return false;
    }
    return board->moveObject(tank.getPosition(), -tank.getDirection());
}

bool MyGameManager::rotate(Tank &tank, const int turn) {
    const int new_direction = tank.getDirection() + turn;
    tank.setDirection(Direction::getDirection(new_direction));
    return true;
}

bool MyGameManager::shoot(Tank &tank) {
    if (tank.getAmmunition() == 0) {
        return false;
    }
    if (tank.getCooldown() != 0) {
        return false;
    }

    tank.decrementAmmunition();
    tank.setCooldown(3);
    board->placeObject(std::make_unique<Shell>(tank.getPosition() + tank.getDirection(), tank.getDirection(),
                                               tank.getId()));
    return true;
}

bool MyGameManager::getBattleInfo(const size_t tank_algo_i, const size_t player_i) {
    auto satellite_view = MySatelliteView(board->getWidth(), board->getHeight());
    board->fillSatelliteView(satellite_view);
    players[player_i - 1]->updateTankWithBattleInfo(*tanks[tank_algo_i], satellite_view);
    return true;
}

bool MyGameManager::allEmptyAmmo() const {
    if (!board) return true;
    for (const auto tank: board->getTanks()) {
        if (tank->getAmmunition() == 0) return false;
    }
    return true;
}

void MyGameManager::tanksTurn() {
    auto actions = std::vector<std::tuple<ActionRequest, bool, bool> >(
        tanks.size(), {ActionRequest::DoNothing, false, false}
    );

    for (const auto tank: board->getTanks()) {
        const int i = tank->getTankAlgoIndex();
        ActionRequest action = tanks[i]->getAction();
        bool ignored = !tankAction(*tank, std::get<0>(actions[i]));
        bool dead = tank->isDestroyed();
        actions[i] = {action, ignored, dead};
    }

    Logger::getInstance().logActions(actions);

    if (empty_countdown == -1 && allEmptyAmmo()) {
        // TODO update with max steps
        empty_countdown = 40;
    }

    if (empty_countdown != -1) {
        empty_countdown--;
    }
}

void MyGameManager::shellsTurn() const {
    for (auto [id, shell]: board->getShells()) {
        board->moveObject(shell->getPosition(), shell->getDirection());
    }
}

void MyGameManager::processStep() {
    if (game_over) return;
    game_step++;

    shellsTurn();
    board->finishMove();

    if (visual) {
        std::cout << "Step " << game_step << std::endl;
    }

    shellsTurn();
    tanksTurn();
    board->finishMove();

    if (visual) {
        board->displayBoard();
    }

    checkDeaths();
}

std::string MyGameManager::getGameResult() const {
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
