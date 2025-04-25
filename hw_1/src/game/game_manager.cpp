#include "GameManager.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <utility>

#include "Mine.h"
#include "Wall.h"
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
    if (tank.getCooldown() == 0) {
        tank.shoot();
        board.addShell(std::make_unique<Shell>(tank.getPosition(), tank.getDirection()));
        return true;
    }
    return false;
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

        if (i % 2 == 0) {
            tanksTurn(p1_algo, p2_algo);
            processMovements();
        }
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

void GameManager::tanksTurn() {
    Tank *t1 = board.getPlayerTank(1);
    Tank *t2 = board.getPlayerTank(2);
    Action a1 = NONE;
    Action a2 = NONE;

    GameState game_state_1 = GameState(board, );

    if (t1 != nullptr) {
        a1 = algo1.decideAction();
    }

    if (t2 != nullptr) {
        a2 = algo2.decideAction();
    }

    std::cout << a1 << " " << a2 << std::endl;

    // Queue actions instead of immediately executing them
    if (t1 != nullptr) {
        t1->queueAction(a1);
        t1->decreaseShootingCooldown();
    }

    if (t2 != nullptr) {
        t2->queueAction(a2);
        t2->decreaseShootingCooldown();
    }

    // Check if both tanks used their shells
    if (empty_countdown == -1 &&
        t1 && t2 &&
        t1->getRemainingShell() == 0 &&
        t2->getRemainingShell() == 0) {
        empty_countdown = 40;
    }

    if (empty_countdown != -1) {
        empty_countdown--;
    }
}

void GameManager::processMovements() {
    std::vector<MovingObject> movements;

    // Process tank 1 movements
    if (Tank *t1 = board.getPlayerTank(1)) {
        std::pair<int, int> new_pos = t1->getPosition();
        bool willShoot = false;

        // Apply queued action
        Action action = t1->getQueuedAction();
        switch (action) {
            case FORWARD:
                new_pos = calcNextPos(t1->getPosition(), t1->getDirection());
                break;
            case BACKWARD:
                new_pos = calcNextPos(t1->getPosition(), t1->getDirection() + 180);
                break;
            case SHOOT:
                willShoot = (t1->getCooldown() == 0 && t1->getRemainingShell() > 0);
                break;
            case ROTATE_45_LEFT:
            case ROTATE_45_RIGHT:
            case ROTATE_90_LEFT:
            case ROTATE_90_RIGHT:
                // Handle rotations separately as they don't cause position changes
                rotate(*t1, action == ROTATE_45_LEFT
                                ? -45
                                : action == ROTATE_45_RIGHT
                                      ? 45
                                      : action == ROTATE_90_LEFT
                                            ? -90
                                            : 90);
                break;
            default:
                break;
        }

        if (new_pos != t1->getPosition()) {
            movements.push_back({MovingObject::TANK, 1, t1->getPosition(), new_pos});
        }

        if (willShoot) {
            t1->shoot();
            std::pair<int, int> shellPos = t1->getPosition();
            std::pair<int, int> nextPos = calcNextPos(shellPos, t1->getDirection());
            board.addShell(std::make_unique<Shell>(shellPos, t1->getDirection()));
            movements.push_back({MovingObject::SHELL, board.getShellsCount() - 1, shellPos, nextPos});
        }
    }

    // Process tank 2 movements (similar to tank 1)
    if (Tank *t2 = board.getPlayerTank(2)) {
        std::pair<int, int> new_pos = t2->getPosition();
        bool willShoot = false;

        // Apply queued action
        Action action = t2->getQueuedAction();
        switch (action) {
            case FORWARD:
                new_pos = calcNextPos(t2->getPosition(), t2->getDirection());
                break;
            case BACKWARD:
                new_pos = calcNextPos(t2->getPosition(), t2->getDirection() + 180);
                break;
            case SHOOT:
                willShoot = (t2->getCooldown() == 0 && t2->getRemainingShell() > 0);
                break;
            case ROTATE_45_LEFT:
            case ROTATE_45_RIGHT:
            case ROTATE_90_LEFT:
            case ROTATE_90_RIGHT:
                // Handle rotations separately as they don't cause position changes
                rotate(*t2, action == ROTATE_45_LEFT
                                ? -45
                                : action == ROTATE_45_RIGHT
                                      ? 45
                                      : action == ROTATE_90_LEFT
                                            ? -90
                                            : 90);
                break;
            default:
                break;
        }

        if (new_pos != t2->getPosition()) {
            movements.push_back({MovingObject::TANK, 2, t2->getPosition(), new_pos});
        }

        if (willShoot) {
            t2->shoot();
            std::pair<int, int> shellPos = t2->getPosition();
            std::pair<int, int> nextPos = calcNextPos(shellPos, t2->getDirection());
            board.addShell(std::make_unique<Shell>(shellPos, t2->getDirection()));
            movements.push_back({MovingObject::SHELL, board.getShellsCount() - 1, shellPos, nextPos});
        }
    }

    // Process all movements
    detectAndHandleCollisions(movements);
    applyMovements(movements);
}

void GameManager::detectAndHandleCollisions(std::vector<MovingObject> &movements) {
    std::map<std::pair<int, int>, std::vector<MovingObject *> > destination_map;
    std::map<std::pair<int, int>, std::vector<MovingObject *> > path_crossings;

    // Build collision maps
    for (auto &obj: movements) {
        destination_map[obj.to].push_back(&obj);

        // For shells, also track the path it takes
        if (obj.type == MovingObject::SHELL) {
            // Mark intermediate points (for path crossing detection)
            std::pair<int, int> midpoint = {
                (obj.from.first + obj.to.first) / 2,
                (obj.from.second + obj.to.second) / 2
            };
            path_crossings[midpoint].push_back(&obj);
        }
    }

    // Handle destination collisions
    for (auto &[pos, objs]: destination_map) {
        // Multiple objects at same destination
        if (objs.size() > 1) {
            // Mark all objects for removal
            for (auto *obj: objs) {
                obj->to = {-1, -1}; // Mark invalid
            }

            // Handle board elements at this position
            if (GameObject *elem = board.getBoardElement(pos)) {
                if (auto *wall = dynamic_cast<Wall *>(elem)) {
                    wall->takeDamage();
                    if (wall->getHitCount() == 0) {
                        board.removeElement(pos);
                    }
                } else {
                    board.removeElement(pos);
                }
            }
        } else if (objs.size() == 1) {
            // Single object - check collision with board elements
            MovingObject *obj = objs[0];
            if (GameObject *elem = board.getBoardElement(pos)) {
                if (auto *wall = dynamic_cast<Wall *>(elem)) {
                    if (obj->type == MovingObject::SHELL) {
                        wall->takeDamage();
                        if (wall->getHitCount() == 0) {
                            board.removeElement(pos);
                        }
                        obj->to = {-1, -1}; // Mark shell invalid
                    } else {
                        obj->to = obj->from; // Prevent tank movement
                    }
                } else if (dynamic_cast<Mine *>(elem)) {
                    board.removeElement(pos);
                    obj->to = {-1, -1}; // Mark object invalid
                } else {
                    board.removeElement(pos);
                    obj->to = {-1, -1}; // Mark object invalid
                }
            }
        }
    }

    // Handle path crossings (for shells)
    for (auto &[pos, objs]: path_crossings) {
        if (objs.size() > 1) {
            // Shells cross paths - destroy all
            for (auto *obj: objs) {
                obj->to = {-1, -1}; // Mark invalid
            }
        }
    }
}

void GameManager::applyMovements(const std::vector<MovingObject> &movements) {
    for (const auto &obj: movements) {
        if (obj.to.first == -1) {
            // This object was marked for removal
            if (obj.type == MovingObject::TANK) {
                Tank *tank = board.getPlayerTank(obj.id);
                if (tank) tank->setDestroyed();
            } else if (obj.type == MovingObject::SHELL) {
                board.removeShell(obj.id);
            }
        } else if (obj.to != obj.from) {
            // Apply movement
            if (obj.type == MovingObject::TANK) {
                board.moveTank(obj.from, obj.to);
            } else if (obj.type == MovingObject::SHELL) {
                board.moveShell(obj.id, obj.to);
            }
        }
    }
}

void GameManager::shellsTurn() {
    std::vector<MovingObject> shell_movements;

    // Collect all shell movements
    for (int i = 0; i < board.getShellsCount(); i++) {
        const Shell *shell = board.getShell(i);
        if (shell) {
            std::pair<int, int> nextPos = calcNextPos(shell->getPosition(), shell->getDirection());
            shell_movements.push_back({MovingObject::SHELL, i, shell->getPosition(), nextPos});
        }
    }

    // Process shell movements
    detectAndHandleCollisions(shell_movements);
    applyMovements(shell_movements);
}

void GameManager::processStep() {
    if (empty_countdown == 0) {
        winner = TIE;
    }

    tanksTurn();
    processMovements();
    shellsTurn();
    board.displayBoard();

    if (const Winner winner = checkDeaths(); winner != NO_WINNER) {
        return winner;
    }
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
