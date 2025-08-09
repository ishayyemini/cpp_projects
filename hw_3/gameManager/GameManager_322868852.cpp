#include "GameManager_322868852.h"

#include <iostream>
#include <thread>
#include <chrono>

#include "Logger.h"
#include "Shell.h"
#include "ActionRequest.h"
#include "MySatelliteView.h"
#include "GameManagerRegistration.h"
#include "GameObjectFactory.h"

using namespace std::chrono_literals;

using namespace GameManager_322868852_340849710;

REGISTER_GAME_MANAGER(GameManager_322868852)

void GameManager_322868852::initBoard(size_t map_width, size_t map_height, const SatelliteView &map,
                                      const std::string &map_name, size_t max_steps, size_t num_shells) {
    board = std::make_unique<Board>(map_name, max_steps, num_shells, map_width, map_height);
    GameObjectFactory::reset();

    for (size_t y = 0; y < map_height; ++y) {
        for (size_t x = 0; x < map_width; ++x) {
            auto obj = GameObjectFactory::create(map.getObjectAt(x, y), Position(x, y), num_shells);
            if (const auto t = dynamic_cast<Tank *>(obj.get())) {
                tanks.emplace_back(ta_factories[t->getPlayerIndex() - 1](t->getPlayerIndex(), t->getTankIndex()));
                tank_status.emplace_back(false, ActionRequest::DoNothing, true, false);
            }
            board->placeObject(std::move(obj));
        }
    }
}

GameResult GameManager_322868852::run(size_t map_width, size_t map_height,
                                      const SatelliteView &map, // <= a snapshot, NOT updated
                                      std::string map_name,
                                      size_t max_steps, size_t num_shells,
                                      Player &player1, std::string name1, Player &player2, std::string name2,
                                      TankAlgorithmFactory player1_tank_algo_factory,
                                      TankAlgorithmFactory player2_tank_algo_factory) {
    players.push_back(&player1);
    player_names.push_back(name1);
    ta_factories.push_back(player1_tank_algo_factory);
    players.push_back(&player2);
    player_names.push_back(name2);
    ta_factories.push_back(player2_tank_algo_factory);

    initBoard(map_width, map_height, map, map_name, max_steps, num_shells);

    GameResult result;

    checkDeaths(); //check if one of the player doesn't have any tanks
    while (!isGameOver()) {
        processStep();
        if (visual) std::this_thread::sleep_for(200ms);
    }

    Logger::getInstance().logResult(getGameResult(result));
    if (visual) {
        std::cout << getGameResult(result) << std::endl;
    }

    return result;
}

void GameManager_322868852::updateCounters(Tank &tank, const ActionRequest action) {
    tank.decreaseShootingCooldown();
    const int back_counter = tank.getBackwardsCounter();

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
            tank.setBackwardsCounter(back_counter - 1);
        }
    }
}

bool GameManager_322868852::tankAction(Tank &tank, const ActionRequest action) {
    bool result = false;
    const int back_counter = tank.getBackwardsCounter();
    updateCounters(tank, action);
    if (action == ActionRequest::DoNothing) return true;
    if (back_counter == 1 && action != ActionRequest::MoveForward) return moveBackward(tank);

    switch (action) {
        case ActionRequest::MoveForward:
            result = moveForward(tank);
            if (back_counter == 0) tank.setBackwardsCounter(3);
            break;
        case ActionRequest::MoveBackward:
            if (back_counter == 0) result = moveBackward(tank);
            else tank.setBackwardsCounter(back_counter - 1);
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
            result = getBattleInfo(tank, tank.getPlayerIndex());
            break;
        default: ;
    }

    return result;
}

void GameManager_322868852::checkDeaths() {
    const bool firstDead = board->getPlayerAliveTanks(1).empty();
    const bool secondDead = board->getPlayerAliveTanks(2).empty();

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
        return;
    }

    if (game_step == board->getMaxSteps()) {
        winner = TIE_STEPS;
        game_over = true;
    }
}

bool GameManager_322868852::moveForward(Tank &tank) {
    if (const auto obj = board->getObjectAt(tank.getPosition() + tank.getDirection())) {
        if (obj->getSymbol() == '#') return false;
    }
    return board->moveObject(tank.getPosition(), tank.getDirection());
}

bool GameManager_322868852::moveBackward(Tank &tank) {
    if (const auto obj = board->getObjectAt(tank.getPosition() + tank.getDirection())) {
        if (obj->getSymbol() == '#') return false;
    }
    return board->moveObject(tank.getPosition(), -tank.getDirection());
}

bool GameManager_322868852::rotate(Tank &tank, const int turn) {
    const int new_direction = tank.getDirection() + turn;
    tank.setDirection(Direction::getDirection(new_direction));
    return true;
}

bool GameManager_322868852::shoot(Tank &tank) {
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

bool GameManager_322868852::getBattleInfo(const Tank &tank, const size_t player_i) {
    const int tank_algo_i = tank.getTankAlgoIndex();
    auto [x,y] = tank.getPosition();
    MySatelliteView satellite_view = this->satellite_view;
    satellite_view.setObjectAt(x, y, '%');
    players[player_i - 1]->updateTankWithBattleInfo(*tanks[tank_algo_i], satellite_view);
    return true;
}

void GameManager_322868852::updateSatelliteView() {
    satellite_view.setDimensions(board->getWidth(), board->getHeight());
    board->fillSatelliteView(satellite_view);
}

bool GameManager_322868852::allEmptyAmmo() const {
    if (!board) return true;
    for (const auto tank: board->getAliveTanks()) {
        if (tank->getAmmunition() != 0) return false;
    }
    return true;
}

void GameManager_322868852::tanksTurn() {
    for (const auto tank: board->getAliveTanks()) {
        const int i = tank->getTankAlgoIndex();
        const ActionRequest action = tanks[i]->getAction();
        const bool res = tankAction(*tank, action);
        tank_status[i] = {false, action, res, false};
    }

    if (empty_countdown == -1 && allEmptyAmmo()) {
        empty_countdown = max_steps_empty_ammo;
    }

    if (empty_countdown != -1) {
        empty_countdown--;
    }
}

void GameManager_322868852::shellsTurn() const {
    for (auto [id, shell]: board->getShells()) {
        board->moveObject(shell->getPosition(), shell->getDirection());
    }
}

void GameManager_322868852::processStep() {
    if (game_over) return;
    game_step++;

    updateSatelliteView();

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
    logStep();

    // Export game state for visualization after each step
    if (export_game_state) {
        exportGameState();
    }
}

std::string GameManager_322868852::getGameResult(GameResult &result) const {
    std::string p1_tanks = std::to_string(board->getPlayerAliveTanks(1).size());
    std::string p2_tanks = std::to_string(board->getPlayerAliveTanks(2).size());
    std::string max_steps = std::to_string(board->getMaxSteps());
    std::string empty_ammo = std::to_string(max_steps_empty_ammo);

    result.remaining_tanks = {board->getPlayerAliveTanks(1).size(), board->getPlayerAliveTanks(2).size()};
    auto sv = std::make_unique<MySatelliteView>(board->getWidth(), board->getHeight());
    board->fillSatelliteView(*sv);
    result.gameState = std::move(sv);
    result.rounds = game_step;

    switch (winner) {
        case TIE:
            result.winner = 0;
            result.reason = GameResult::Reason::ALL_TANKS_DEAD;
            return "Tie, both players have zero tanks";
        case TIE_STEPS:
            result.winner = 0;
            result.reason = GameResult::Reason::MAX_STEPS;
            return "Tie, reached max steps = " + max_steps + ", player 1 has " + p1_tanks + " tanks, player 2 has "
                   +
                   p2_tanks + " tanks";
        case TIE_AMMO:
            result.winner = 0;
            result.reason = GameResult::Reason::ZERO_SHELLS;
            return "Tie, both players have zero shells for " + empty_ammo + " steps";
        case PLAYER_1:
            result.winner = 1;
            return "Player 1 won with " + p1_tanks + " tanks still alive";
        case PLAYER_2:
            result.winner = 2;
            return "Player 2 won with " + p2_tanks + " tanks still alive";
        default:
            return "Bad result";
    }
}

void GameManager_322868852::logStep() {
    for (const auto tank: board->getTanks()) {
        if (tank->isDestroyed()) {
            std::get<3>(tank_status[tank->getTankAlgoIndex()]) = true;
        }
    }

    Logger::getInstance().logActions(tank_status);

    // Update deaths
    for (size_t i = 0; i < tank_status.size(); i++) {
        if (std::get<3>(tank_status[i])) {
            std::get<0>(tank_status[i]) = true;
        }
    }
}

void GameManager_322868852::exportGameState() {
    if (!game_state_file.is_open()) return;
    game_state_file << "STEP " << game_step << std::endl;
    game_state_file << board->getWidth() << " " << board->getHeight() << std::endl;

    for (int y = 0; y < board->getHeight(); y++) {
        for (int x = 0; x < board->getWidth(); x++) {
            Position pos{x, y};
            auto obj = board->getObjectAt(pos);
            if (obj) {
                char symbol = obj->getSymbol();
                if (symbol == '1' || symbol == '2') {
                    auto tank = dynamic_cast<Tank *>(obj);
                    if (tank) {
                        int player_id = tank->getPlayerIndex();
                        int dir = tank->getDirection();
                        int ammo = tank->getAmmunition();
                        game_state_file << "T," << x << "," << y << "," << player_id << "," << dir << "," << ammo <<
                                std::endl;
                    }
                } else if (symbol == '*' || symbol == 'X') {
                    int dir = obj->getDirection();
                    game_state_file << "*," << x << "," << y << "," << dir << std::endl;
                } else if (symbol == '@') {
                    game_state_file << "@," << x << "," << y << std::endl;
                } else if (symbol == '#') {
                    game_state_file << "#," << x << "," << y << std::endl;
                }
            }
        }
    }

    game_state_file << "END_STEP" << std::endl;
    game_state_file.flush();

    if (game_over) {
        GameResult result;
        game_state_file << "GAME_OVER" << std::endl;
        game_state_file << getGameResult(result) << std::endl;
        game_state_file.close();
    }
}

