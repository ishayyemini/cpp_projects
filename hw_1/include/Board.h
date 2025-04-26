#ifndef BOARD_H
#define BOARD_H

#include <map>
#include <memory>
#include <Shell.h>
#include <vector>

#include "GameObject.h"
#include "Tank.h"

enum ObjectType {
    TANK_1,
    TANK_2,
    MINE,
    WALL,
};

class Board {
    int width = 1;
    int height = 1;
    std::vector<std::vector<std::unique_ptr<GameObject> > > board;
    std::map<int, Position> tanks_pos;
    int player_1_tank;
    int player_2_tank;
    std::map<int, Position> shells_pos;
    std::map<int, Position> collisions_pos;

public:
    Board();

    Board(int width, int height);

    [[nodiscard]] int getHeight() const { return height; }

    [[nodiscard]] int getWidth() const { return width; }

    [[nodiscard]] bool isOccupied(Position pos) const;

    template<typename T>
    T *placeObject(std::unique_ptr<T> element);

    void removeObject(Position pos);

    GameObject *moveObject(Position from, Position to);

    Position wrapPosition(Position pos) const;

    GameObject *getObjectAt(Position pos) const;

    std::map<int, Position> getShellsPos() const{ return shells_pos; }

    // [[nodiscard]] Tank *getPlayerTank(int player_id, int tank_id) const;
    //
    [[nodiscard]] Tank *getPlayerTank(int player_id) const;

    //
    // [[nodiscard]] GameObject *getBoardElement(int row, int col) const noexcept;
    //
    // [[nodiscard]] GameObject *getBoardElement(const std::pair<int, int> &pos) const noexcept;
    //
    // bool pushSymbol(int row, int col, char symbol);
    //
    // bool moveTank(const std::pair<int, int> &old_pos, const std::pair<int, int> &new_pos);
    //
    // bool moveShell(int shell_index, const std::pair<int, int> &new_pos);
    //
    // void displayBoard() const;
    //
    // void addShell(std::unique_ptr<Shell> shell);
    //
    [[nodiscard]] Shell *getShell(int i) const;
    //
    // [[nodiscard]] Shell *getShell(const std::pair<int, int> &pos) const;
    //
    // void removeElement(const std::pair<int, int> &pos);
    //
    // void removeShell(int shell_index);
    //
    // int getShellsCount() const { return shells.size(); }
};

// template<>
// const Tank *Board::placeObject<Tank>(Position pos, std::unique_ptr<Tank> element);


#endif //BOARD_H
