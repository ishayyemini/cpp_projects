//
// Created by Ishay Yemini on 25/04/2025.
//

#ifndef BOARD_H
#define BOARD_H

#include <map>
#include <unordered_set>
#include <memory>
#include <vector>

#include "GameObject.h"
#include "Mine.h"
#include "Shell.h"
#include "Tank.h"
#include "Wall.h"

enum ObjectType {
    TANK_1,
    TANK_2,
    MINE,
    WALL,
};

class Board {
    int width = 2;
    int height = 2;
    std::vector<std::vector<std::unique_ptr<GameObject> > > board;
    std::map<int, Position> tanks_pos;
    int player_1_tank = -1;
    int player_2_tank = -1;
    std::map<int, Position> shells_pos;
    std::map<int, Position> collisions_pos;
    std::map<int, Position> moving_pos;
    std::vector<std::unique_ptr<GameObject> > destroyed;

    GameObject *placeObjectReal(std::unique_ptr<GameObject> element, Position real_pos);

    bool isOccupiedReal(Position real_pos) const;

    void removeObjectReal(Position real_pos);

    GameObject *replaceObjectReal(Position from_real, Position to_real);

    Position wrapPositionReal(Position real_pos) const;

    GameObject *getObjectAtReal(Position real_pos) const;

    GameObject *moveObjectReal(Position from_real, Direction::DirectionType dir);

    void removeIndices(GameObject *game_object);

    void checkCollisions();

public:
    Board();

    Board(int width, int height);

    [[nodiscard]] int getHeight() const { return height / 2; }

    [[nodiscard]] int getWidth() const { return width / 2; }

    [[nodiscard]] bool isOccupied(Position pos) const;

    template<typename T>
    T *placeObject(std::unique_ptr<T> element);

    void removeObject(Position pos);

    GameObject *replaceObject(Position from, Position to);

    GameObject *moveObject(Position from, Direction::DirectionType dir);

    Position wrapPosition(Position pos) const;

    GameObject *getObjectAt(Position pos) const;

    // [[nodiscard]] Tank *getPlayerTank(int player_id, int tank_id) const;
    //
    [[nodiscard]] Tank *getPlayerTank(int player_id) const;

    void displayBoard() const;

    void finishMove();

    std::map<int, Position> getShells() const;

    // void addShell(std::unique_ptr<Shell> shell);
    //
    //
    // [[nodiscard]] Shell *getShell(const std::pair<int, int> &pos) const;
    //
    // void removeElement(const std::pair<int, int> &pos);
    //
    // void removeShell(int shell_index);
    //
    // int getShellsCount() const { return shells.size(); }
};

template<typename T>
T *Board::placeObject(std::unique_ptr<T> element) {
    static_assert(
        std::is_same_v<GameObject, Tank> ||
        std::is_same_v<GameObject, Wall> ||
        std::is_same_v<GameObject, Shell> ||
        std::is_same_v<GameObject, Mine> ||
        std::is_same_v<GameObject, GameObject>
    );
    if (element == nullptr) return nullptr;
    Position pos = element->getPosition();
    return dynamic_cast<T *>(placeObjectReal(std::move(element), pos * 2));
}

#endif //BOARD_H
