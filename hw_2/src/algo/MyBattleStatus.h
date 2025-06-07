#ifndef BATTLEUTILS_H
#define BATTLEUTILS_H
#include <vector>
#include "Direction.h"

//todo: add rule of 5
class MyBattleStatus {
public:
    size_t board_x{0};
    size_t board_y{0};
    size_t max_steps{0};
    size_t num_shells{0};
    size_t turn_number{0};
    size_t cool_down{0};
    size_t last_requested_info_turn{0};

    Direction::DirectionType tank_direction;
    Position tank_position = {-1, -1};

    enum class BoardItem {
        WALL,
        MINE,
        SHELL,
        TANK_PLAYER_1,
        TANK_PLAYER_2,
        CURRENT_TANK,
        EMPTY,
        OUTSIDE
    };

    explicit MyBattleStatus(int player_id, int tank_index);

    void updateBoard(const std::vector<std::vector<char> > &updated_board);

    Position wrapPosition(Position p) const;

    void updateBattleStatusBaseAction(ActionRequest action);

    char getBoardItem(int x, int y) const;

    char getBoardItem(Position p) const;

    size_t getDistance(const Position &pos1, const Position &pos2) const;

    bool isShellClose(Position position, size_t thresh = 6) const;

    bool isShellClose(size_t thresh = 6) const;

    bool isEnemyClose(Position position) const;

    bool isEnemyClose() const;

    size_t getEnemyTankCounts() const;

    std::vector<Position> getEnemyPositions() const;

    bool hasTankAmmo() const;

    bool canTankShoot() const;

    bool canTankShootEnemy(bool include_shells = false) const;

    bool canTankShootEnemy(Position enemy) const;

    bool canTankShootEnemy(Direction::DirectionType dir, bool include_shells = false) const;

    bool isSafePosition(Position p, bool immediate_safe = false) const;

    std::vector<Direction::DirectionType> getSafeDirections(Position position) const;

    ActionRequest rotateTowards(Direction::DirectionType to_direction) const;

    static char boardItemToChar(const BoardItem b) {
        switch (b) {
            case BoardItem::WALL: return '#';
            case BoardItem::SHELL: return '*';
            case BoardItem::MINE: return '@';
            case BoardItem::TANK_PLAYER_1: return '1';
            case BoardItem::TANK_PLAYER_2: return '2';
            case BoardItem::CURRENT_TANK: return '%';
            case BoardItem::EMPTY: return ' ';
            case BoardItem::OUTSIDE: return '&';
            default: return '?';
        }
    }

private:
    // board_info based on last satellite update
    std::vector<std::vector<char> > board = {};
    std::vector<Position> enemy_positions = {};
    std::vector<Position> ally_positions = {};
    std::vector<Position> shells_position = {};

    int player_id{0};
    int tank_index{0};

    char getEnemyName() const;

    char getAllyName() const;

    void updateTanksPosition();

    Position getTankPosition() const;

    void updateTankDirectionBaseAction(ActionRequest action);

    bool isTargetOnSight(Direction::DirectionType dir, Position enemy) const;
};


#endif //BATTLEUTILS_H
