#include "Board.h"

#include <iostream>

#include "Collision.h"
#include "Mine.h"
#include "Tank.h"
#include "Wall.h"
#include "Shell.h"

GameObject *Board::placeObjectReal(std::unique_ptr<GameObject> element, const Position real_pos) {
    const auto [x, y] = wrapPositionReal(real_pos);

    // Check for collision
    if (isOccupiedReal(real_pos)) {
        if (const auto collision = dynamic_cast<Collision *>(getObjectAtReal(real_pos))) {
            collision->addElement(std::move(element));
        } else {
            board[y][x] = std::make_unique<Collision>(std::move(board[y][x]), std::move(element));
            collisions_pos[board[y][x]->getId()] = Position(x, y);
        }
    } else {
        board[y][x] = std::move(element);
    }

    GameObject *game_object = board[y][x].get();

    if (const auto tank = dynamic_cast<Tank *>(game_object)) {
        tanks_pos[tank->getId()] = Position(x, y);
        if (tank->getPlayerId() == 1) {
            player_1_tank = tank->getId();
        } else {
            player_2_tank = tank->getId();
        }
    }

    if (const auto shell = dynamic_cast<Shell *>(game_object)) {
        shells_pos[shell->getId()] = Position(x, y);
    }

    if (x % 2 == 0 && y % 2 == 0) {
        game_object->setPosition(Position(x / 2, y / 2));
    } else {
        moving_pos[game_object->getId()] = Position(x, y);
    }

    return game_object;
}

Position Board::wrapPositionReal(const Position real_pos) const {
    const int mod_x = (real_pos.x % width + width) % width;
    const int mod_y = (real_pos.y % height + height) % height;
    return Position(mod_x, mod_y);
}

GameObject *Board::getObjectAtReal(const Position real_pos) const {
    const auto [x, y] = wrapPositionReal(real_pos);
    return board[y][x].get();
}

bool Board::isOccupiedReal(const Position real_pos) const {
    return getObjectAtReal(real_pos) != nullptr;
}

void Board::removeObjectReal(const Position real_pos) {
    const auto [x, y] = wrapPositionReal(real_pos);
    if (board[y][x] != nullptr) removeIndices(board[y][x].get());
    board[y][x] = nullptr;
}

GameObject *Board::replaceObjectReal(const Position from_real, const Position to_real) {
    const auto [f_x, f_y] = wrapPositionReal(from_real);
    if (!isOccupiedReal(from_real)) return nullptr;

    std::unique_ptr<GameObject> element = nullptr;

    // Handle moving collisions -> If not ok, move entire collision. Else, move just the shell.
    if (const auto collision = dynamic_cast<Collision *>(getObjectAtReal(from_real))) {
        if (collision->checkOkCollision()) {
            element = collision->getShell();
            std::unique_ptr<Mine> mine = collision->getMine();
            removeIndices(collision);
            board[f_y][f_x] = std::move(mine);
        }
    }
    if (element == nullptr) element = std::move(board[f_y][f_x]);

    removeIndices(element.get());
    return placeObjectReal(std::move(element), to_real);
}

GameObject *Board::moveObjectReal(const Position from_real, const Direction::DirectionType dir) {
    return replaceObjectReal(from_real, from_real + dir);
}

void Board::removeIndices(GameObject *game_object) {
    if (game_object == nullptr) return;

    if (const auto tank = dynamic_cast<Tank *>(game_object)) {
        tanks_pos.erase(tank->getId());
        if (tank->getPlayerId() == 1) {
            player_1_tank = -1;
        } else {
            player_2_tank = -1;
        }
    }

    if (const auto shell = dynamic_cast<Shell *>(game_object)) {
        shells_pos.erase(shell->getId());
    }

    if (const auto collision = dynamic_cast<Collision *>(game_object)) {
        for (const auto it = collision->getElements().begin(); it != collision->getElements().end();) {
            removeIndices(it->get());
        }
        collisions_pos.erase(collision->getId());
    }

    moving_pos.erase(game_object->getId());
}

Board::Board() {
}

Board::Board(const int width, const int height) : width(width * 2),
                                                  height(height * 2),
                                                  board(std::vector<std::vector<std::unique_ptr<GameObject> > >(
                                                      height * 2)) {
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            board[i].push_back(nullptr);
        }
    }
}

bool Board::isOccupied(const Position pos) const {
    return isOccupiedReal(pos * 2);
}

void Board::removeObject(const Position pos) {
    removeObjectReal(pos * 2);
}

Position Board::wrapPosition(const Position pos) const {
    return wrapPositionReal(pos * 2) / 2;
}

GameObject *Board::getObjectAt(const Position pos) const {
    return getObjectAtReal(pos * 2);
}

GameObject *Board::replaceObject(const Position from, const Position to) {
    return replaceObjectReal(from * 2, to * 2);
}

// We have to call finishMove() afterwards!
GameObject *Board::moveObject(const Position from, const Direction::DirectionType dir) {
    return moveObjectReal(from * 2, dir);
}

// This takes into account there being only one tank per player
Tank *Board::getPlayerTank(const int player_id) const {
    int tank_id;
    if (player_id == 1) {
        tank_id = player_1_tank;
    } else {
        tank_id = player_2_tank;
    }
    if (!tanks_pos.contains(tank_id)) return nullptr;
    GameObject *b = getObjectAtReal(tanks_pos.at(tank_id));
    if (const auto t = dynamic_cast<Tank *>(b)) return t;
    return nullptr;
}

void Board::displayBoard() const {
    for (int i = 0; i < height; i += 2) {
        for (int j = 0; j < width; j += 2) {
            if (board[i][j] != nullptr) {
                std::cout << *board[i][j];
            } else {
                std::cout << "[     ]";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Board::checkCollisions() {
    for (const auto [id, pos]: collisions_pos) {
        if (const auto collision = dynamic_cast<Collision *>(getObjectAtReal(pos))) {
            if (collision->checkOkCollision()) continue;
        }
        removeObjectReal(pos);
    }
}

void Board::finishMove() {
    checkCollisions(); // After this, we only have ok collisions
    for (const auto [id, pos]: moving_pos) {
        if (const auto obj = getObjectAtReal(pos)) {
            moveObjectReal(pos, obj->getDirection());
        } else {
            moving_pos.erase(id);
        }
    }
    checkCollisions();
}
