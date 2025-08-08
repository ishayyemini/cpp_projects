#include "Board.h"

#include <iostream>
#include <utility>

#include "Collision.h"
#include "Mine.h"
#include "Tank.h"
#include "Wall.h"
#include "Shell.h"

GameObject *Board::placeObjectReal(std::unique_ptr<GameObject> element, const Position real_pos) {
    const auto [x, y] = wrapPositionReal(real_pos);

    if (const auto tank = dynamic_cast<Tank *>(element.get())) {
        tanks_pos[{tank->getPlayerIndex(), tank->getTankIndex()}] = Position(x, y);
    }

    if (const auto shell = dynamic_cast<Shell *>(element.get())) {
        shells_pos[shell->getId()] = Position(x, y);
    }

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

    if (x % 2 == 0 && y % 2 == 0) {
        game_object->setPosition(Position(x / 2, y / 2));
    } else {
        moving_pos[game_object->getId()] = Position(x, y);
    }

    return game_object;
}

Position Board::wrapPositionReal(const Position real_pos) const {
    const size_t mod_x = (real_pos.x % width + width) % width;
    const size_t mod_y = (real_pos.y % height + height) % height;
    return {mod_x, mod_y};
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
    destroyed.push_back(std::move(board[y][x]));
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
        tanks_pos.erase({tank->getPlayerIndex(), tank->getTankIndex()});
    }

    if (const auto shell = dynamic_cast<Shell *>(game_object)) {
        shells_pos.erase(shell->getId());
    }

    if (const auto collision = dynamic_cast<Collision *>(game_object)) {
        while (auto it = collision->popElement()) {
            removeIndices(it.get());
            destroyed.push_back(std::move(it));
        }
        collisions_pos.erase(collision->getId());
    }

    moving_pos.erase(game_object->getId());
}

Board::Board() : max_steps(0), num_shells(0) {
}

Board::Board(std::string desc, const size_t max_steps, const size_t num_shells, size_t width,
             size_t height) : desc(std::move(desc)), max_steps(max_steps), num_shells((num_shells)), width(width * 2),
                              height(height * 2),
                              board(std::vector<std::vector<std::unique_ptr<GameObject> > >(
                                  height * 2)) {
    for (size_t i = 0; i < this->height; i++) {
        for (size_t j = 0; j < this->width; j++) {
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

std::vector<Tank *> Board::getAliveTanks() const {
    std::vector<Tank *> tanks;
    for (auto pos: tanks_pos) {
        GameObject *b = getObjectAtReal(pos.second);
        if (auto t = dynamic_cast<Tank *>(b)) {
            if (!t->isDestroyed()) tanks.push_back(t);
        }
    }
    return tanks;
}

std::vector<Tank *> Board::getTanks() const {
    std::vector<Tank *> tanks;

    for (auto pos: tanks_pos) {
        GameObject *b = getObjectAtReal(pos.second);
        if (auto t = dynamic_cast<Tank *>(b)) {
            tanks.push_back(t);
        }
    }

    for (auto &obj: destroyed) {
        if (auto t = dynamic_cast<Tank *>(obj.get())) {
            tanks.push_back(t);
        }
    }

    return tanks;
}

GameObject *Board::replaceObject(const Position from, const Position to) {
    return replaceObjectReal(from * 2, to * 2);
}

// We have to call finishMove() afterwards!
GameObject *Board::moveObject(const Position from, const Direction::DirectionType dir) {
    return moveObjectReal(from * 2, dir);
}

std::vector<Tank *> Board::getPlayerAliveTanks(int player_index) const {
    std::vector<Tank *> tanks;
    for (auto tank: getAliveTanks()) {
        if (tank->getPlayerIndex() == player_index) tanks.push_back(tank);
    }
    return tanks;
}

std::vector<Tank *> Board::getPlayerTanks(const int player_index) const {
    std::vector<Tank *> tanks;
    for (auto tank: getTanks()) {
        if (tank->getPlayerIndex() == player_index) tanks.push_back(tank);
    }
    return tanks;
}

Tank *Board::getPlayerTank(int player_index, int tank_index) const {
    for (const auto tank: getPlayerTanks(player_index)) {
        if (tank->getTankIndex() == tank_index) return tank;
    }
    return nullptr;
}

void Board::displayBoard() const {
    for (size_t i = 0; i < height; i += 2) {
        for (size_t j = 0; j < width; j += 2) {
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
    for (auto tmp_pos = collisions_pos; const auto [id, pos]: tmp_pos) {
        if (const auto collision = dynamic_cast<Collision *>(getObjectAtReal(pos))) {
            if (collision->checkOkCollision()) continue;

            if (std::unique_ptr<Wall> wall = collision->getWeakenedWall()) {
                removeObjectReal(pos);
                placeObjectReal(std::move(wall), pos);
                continue;
            }
        }

        removeObjectReal(pos);
    }
}

void Board::finishMove() {
    checkCollisions(); // After this, we only have ok collisions

    for (const auto tmp_pos = moving_pos; const auto [id, pos]: tmp_pos) {
        if (const auto obj = getObjectAtReal(pos)) {
            moveObjectReal(pos, obj->getDirection());
        } else {
            moving_pos.erase(id);
        }
    }

    checkCollisions();
}

std::map<int, Shell *> Board::getShells() const {
    std::map<int, Shell *> shells;
    for (const auto [id, pos]: shells_pos) {
        if (const auto shell = dynamic_cast<Shell *>(getObjectAtReal(pos))) {
            shells[id] = shell;
        }
        if (const auto collision = dynamic_cast<Collision *>(getObjectAtReal(pos))) {
            if (collision->checkOkCollision()) {
                shells[id] = collision->getShellPtr();
            }
        }
    }
    return shells;
}

void Board::fillSatelliteView(MySatelliteView &satellite_view) const {
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            if (isCollision({i, j}))
                satellite_view.setObjectAt(i, j, '*'); // We can only have mine-shell collisions here
            else if (isOccupied({i, j}))
                satellite_view.setObjectAt(i, j, getObjectAt({i, j})->getSymbol());
            else
                satellite_view.setObjectAt(i, j, ' ');
        }
    }
}
