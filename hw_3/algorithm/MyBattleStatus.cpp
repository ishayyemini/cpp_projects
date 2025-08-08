#include "MyBattleStatus.h"

#include "Direction.h"

namespace Algorithm_322868852_340849710 {
    MyBattleStatus::MyBattleStatus(int player_id, int tank_index) : tank_direction(
                                                                        player_id == 1
                                                                            ? Direction::LEFT
                                                                            : Direction::RIGHT),
                                                                    player_id(player_id),
                                                                    tank_index(tank_index) {
        updateTanksPosition();
    }

    Position MyBattleStatus::wrapPosition(Position p) const {
        const size_t mod_x = p.x % board_x;
        const size_t mod_y = p.y % board_y;
        return {mod_x, mod_y};
    }

    void MyBattleStatus::updateBoard(const std::vector<std::vector<char> > &updated_board) {
        board = updated_board;
        board_x = board.size();
        board_y = board[0].size();
        updateTanksPosition();
    }

    Position MyBattleStatus::getTankPosition() const {
        return tank_position;
    }

    bool MyBattleStatus::hasTankAmmo() const {
        return num_shells > 0;
    }

    bool MyBattleStatus::canTankShoot() const {
        return hasTankAmmo() && cool_down == 0;
    }

    char MyBattleStatus::getBoardItem(int x, int y) const {
        return board[x][y];
    }

    char MyBattleStatus::getBoardItem(Position p) const {
        return getBoardItem(p.x, p.y);
    }

    size_t MyBattleStatus::getDistance(const Position &a, const Position &b) const {
        int raw_dx = std::abs(static_cast<int>(a.x) - static_cast<int>(b.x));
        int raw_dy = std::abs(static_cast<int>(a.y) - static_cast<int>(b.y));

        size_t dx = std::min(static_cast<size_t>(raw_dx), board_x - static_cast<size_t>(raw_dx));
        size_t dy = std::min(static_cast<size_t>(raw_dy), board_y - static_cast<size_t>(raw_dy));

        return std::max(dx, dy);
    }

    bool MyBattleStatus::canTankShootEnemy(Direction::DirectionType dir, bool include_shells) const {
        if (enemy_positions.empty() || !canTankShoot()) {
            return false;
        }

        for (Position enemy: enemy_positions) {
            if (isTargetOnSight(dir, enemy)) {
                return true;
            }
        }
        if (include_shells) {
            for (Position shell: shells_position) {
                if (isTargetOnSight(dir, shell)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool MyBattleStatus::canTankShootEnemy(Position enemy) const {
        if (!canTankShoot()) {
            return false;
        }
        return isTargetOnSight(tank_direction, enemy);
    }

    bool MyBattleStatus::canTankShootEnemy(bool include_shells) const {
        return canTankShootEnemy(tank_direction, include_shells);
    }

    bool MyBattleStatus::isTargetOnSight(Direction::DirectionType dir, Position target) const {
        Position p = tank_position;
        for (size_t i = 1; i <= std::max(board_x, board_y); i++) {
            p = wrapPosition(p + dir);
            if (target == p) return true;
            if (board[p.x][p.y] == boardItemToChar(BoardItem::WALL) || board[p.x][p.y] == getAllyName()) {
                return false;
            }
        }
        return false;
    }


    bool MyBattleStatus::isSafePosition(Position p, const bool immediate_safe) const {
        if (board[p.x][p.y] != boardItemToChar(BoardItem::EMPTY)) {
            return false;
        }
        // If only checking immediate safety, we're done
        if (immediate_safe) {
            return true;
        }
        return !(isShellClose(p) || isEnemyClose(p));
    }

    std::vector<Direction::DirectionType> MyBattleStatus::getSafeDirections(const Position position) const {
        std::vector<Direction::DirectionType> safe_directions;

        for (int i = 0; i < Direction::getDirectionSize(); i++) {
            auto direction = Direction::getDirectionFromIndex(i);
            Position next_position = wrapPosition(position + direction);
            if (isSafePosition(next_position)) {
                safe_directions.push_back(direction);
            }
        }

        return safe_directions;
    }

    ActionRequest MyBattleStatus::rotateTowards(Direction::DirectionType to_direction) const {
        if (tank_direction == to_direction) return ActionRequest::DoNothing;

        // Find the shortest rotation path
        if (int diff = (tank_direction - to_direction + 360) % 360; diff > 180) {
            diff = 360 - diff;
            // Clockwise is shorter
            if (diff == 45) return ActionRequest::RotateRight45;
            if (diff == 90) return ActionRequest::RotateRight90;
            if (diff == 135) return ActionRequest::RotateRight90;
            if (diff == 180) return ActionRequest::RotateRight90;
        } else {
            // Counter-clockwise is shorter or equal
            if (diff == 45) return ActionRequest::RotateLeft45;
            if (diff == 90) return ActionRequest::RotateLeft90;
            if (diff == 135) return ActionRequest::RotateLeft90;
            if (diff == 180) return ActionRequest::RotateLeft90;
        }

        return ActionRequest::DoNothing;
    }


    void MyBattleStatus::updateTanksPosition() {
        std::vector<Position> enemy;
        std::vector<Position> ally;
        std::vector<Position> shells;
        for (size_t i{0}; i < board.size(); i++) {
            for (size_t j{0}; j < board.front().size(); j++) {
                Position p = {i, j};

                if (board[i][j] == getAllyName()) {
                    ally.push_back(p);
                } else if (board[i][j] == boardItemToChar(BoardItem::CURRENT_TANK)) {
                    tank_position = p;
                } else if (board[i][j] == getEnemyName()) {
                    enemy.push_back(p);
                } else if (board[i][j] == boardItemToChar(BoardItem::SHELL)) {
                    shells.push_back(p);
                }
            }
        }
        ally_positions = ally;
        enemy_positions = enemy;
        shells_position = shells;
    }

    char MyBattleStatus::getEnemyName() const {
        (void) tank_index;
        return (player_id == 1) ? boardItemToChar(BoardItem::TANK_PLAYER_2) : boardItemToChar(BoardItem::TANK_PLAYER_1);
    }

    char MyBattleStatus::getAllyName() const {
        return (player_id == 1) ? boardItemToChar(BoardItem::TANK_PLAYER_1) : boardItemToChar(BoardItem::TANK_PLAYER_2);
    }

    std::vector<Position> MyBattleStatus::getEnemyPositions() const {
        return enemy_positions;
    }

    size_t MyBattleStatus::getEnemyTankCounts() const {
        return enemy_positions.size();
    }

    void MyBattleStatus::updateTankDirectionBaseAction(const ActionRequest action) {
        tank_direction = Direction::getDirectionAfterAction(tank_direction, action);
    }

    void MyBattleStatus::updateBattleStatusBaseAction(ActionRequest action) {
        if (action == ActionRequest::Shoot) {
            cool_down = 4;
            num_shells--;
            return;
        }
        cool_down = (cool_down == 0) ? 0 : cool_down - 1;
        updateTankDirectionBaseAction(action);
    }

    bool MyBattleStatus::isShellClose(Position position, size_t thresh) const {
        for (auto shell_pos: shells_position) {
            if (getDistance(position, shell_pos) < thresh) {
                return true;
            }
        }
        return false;
    }

    bool MyBattleStatus::isEnemyClose(Position position) const {
        if (enemy_positions.empty()) {
            return false;
        }
        for (auto enemy_position: enemy_positions) {
            if (getDistance(position, enemy_position) <= 2) {
                return true;
            }
        }
        return false;
    }

    //this time we cannot know the shells' direction.
    //we say shell is threatening a tank if is close to him
    bool MyBattleStatus::isShellClose(size_t thresh) const {
        return isShellClose(tank_position, thresh);
    }

    bool MyBattleStatus::isEnemyClose() const {
        return isEnemyClose(tank_position);
    }
}
