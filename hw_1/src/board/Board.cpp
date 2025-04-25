#include "Board.h"

#include "Collision.h"
#include "Tank.h"
#include "Wall.h"
#include "Shell.h"

template<typename T>
T *Board::placeObject(std::unique_ptr<T> element) {
    static_assert(std::is_same_v<T, Tank> || std::is_same_v<T, Wall>);
    return nullptr;
}

template<>
Tank *Board::placeObject<Tank>(std::unique_ptr<Tank> element) {
    const auto [x, y] = wrapPosition(element->getPosition());

    // Check for collision
    if (isOccupied(Position(x, y))) {
        if (const auto collision = dynamic_cast<Collision *>(getObjectAt(Position(x, y)))) {
            collision->addElement(std::move(element));
        } else {
            board[y][x] = std::make_unique<Collision>(board[y][x], std::move(element));
            collisions_pos[board[y][x]->getId()] = board[y][x]->getPosition();
        }
    } else {
        tanks_pos[element->getId()] = element->getPosition();
        if (element->getPlayerId() == 1) {
            player_1_tank = element->getId();
        } else {
            player_2_tank = element->getId();
        }
        board[y][x] = std::move(element);
    }
    GameObject *game_object = board[y][x].get();

    return dynamic_cast<Tank *>(game_object);
}

template<>
Shell *Board::placeObject<Shell>(std::unique_ptr<Shell> element) {
    const auto [x, y] = wrapPosition(element->getPosition());

    // Check for collision
    if (isOccupied(Position(x, y))) {
        if (const auto collision = dynamic_cast<Collision *>(getObjectAt(Position(x, y)))) {
            collision->addElement(std::move(element));
        } else {
            board[y][x] = std::make_unique<Collision>(board[y][x], std::move(element));
            collisions_pos[board[y][x]->getId()] = board[y][x]->getPosition();
        }
    } else {
        shells_pos[element->getId()] = element->getPosition();
        board[y][x] = std::move(element);
    }
    GameObject *game_object = board[y][x].get();

    return dynamic_cast<Shell *>(game_object);
}

Board::Board() {
}

Board::Board(const int width, const int height) : width(width),
                                                  height(height),
                                                  board(std::vector<std::vector<std::unique_ptr<GameObject> > >(
                                                      height, std::vector<std::unique_ptr<GameObject> >(width))) {
}

bool Board::isOccupied(const Position pos) const {
    return getObjectAt(pos) != nullptr;
}

void Board::removeObject(const Position pos) {
    const auto [x, y] = wrapPosition(pos);
    board[y][x] = nullptr;
}

Position Board::wrapPosition(const Position pos) const {
    return Position((pos.x % width + width) % width, (pos.y % height + height) % height);
}

GameObject *Board::getObjectAt(const Position pos) const {
    const auto [x, y] = wrapPosition(pos);
    return board[y][x].get();
}

GameObject *Board::moveObject(const Position from, Position to) {
    const auto [f_x, f_y] = wrapPosition(from);
    const auto [t_x, t_y] = wrapPosition(to);
    if (!isOccupied(from)) return nullptr;
    auto element = std::move(board[f_y][f_x]);
    element->setPosition(Position(t_x, t_y));
    return placeObject(std::move(element));
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
    GameObject *b = getObjectAt(tanks_pos.at(tank_id));
    if (const auto t = dynamic_cast<Tank *>(b)) return t;
    return nullptr;
}
