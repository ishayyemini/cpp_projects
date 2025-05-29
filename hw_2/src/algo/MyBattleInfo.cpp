#include "MyBattleInfo.h"

#include <set>

#include "Direction.h"
#include "SimpleAlgorithm.h"

bool MyBattleInfo::canShoot() const {
    // TODO correct
    // if (auto t = getPlayerTank()) return t->getAmmunition() > 0 && t->getCooldown() == 0;
    return false;
}

bool MyBattleInfo::isSafePosition(const Position position, const bool immediate_safe) const {
    return false;
    // TODO correct
    // if (board.isOccupied(position)) return false;

    // If only checking immediate safety, we're done
    if (immediate_safe) return true;

    return !isShellApproaching(position);
}

std::vector<Direction::DirectionType> MyBattleInfo::getSafeDirections(const Position position) const {
    std::vector<Direction::DirectionType> safe_directions;

    for (int i = 0; i < Direction::getDirectionSize(); i++) {
        auto direction = Direction::getDirectionFromIndex(i);
        Position next_position = position + direction;
        if (isSafePosition(next_position)) {
            safe_directions.push_back(direction);
        }
    }

    return safe_directions;
}

std::vector<Position> MyBattleInfo::getNearbyEmptyPositions(const Position) const {
    // TODO correct
    std::vector<Position> empty_positions;
    //
    // for (int i = 0; i < Direction::getDirectionSize(); i++) {
    //     auto direction = Direction::getDirectionFromIndex(i);
    //     Position next_position = position + direction;
    //     if (!board.isOccupied(next_position)) {
    //         empty_positions.push_back(next_position);
    //     }
    // }
    //
    return empty_positions;
}

bool MyBattleInfo::isInLineOfSight(const Position) const {
    return false;
    // TODO implement
    // if (!getPlayerTank()) return false;
    // return isInLineOfSight(getPlayerTank()->getPosition(), getPlayerTank()->getDirection(), target);
}

bool MyBattleInfo::isInLineOfSight(const Position, const Direction::DirectionType,
                                   const Position) const {
    // TODO implement
    // Position check = from;
    // for (int i = 1; i <= std::max(board.getWidth(), board.getHeight()); i++) {
    //     check = getBoard().wrapPosition(check + dir);
    //     if (target == check) return true;
    //     if (board.isWall(check)) return false;
    //     // Only if shell is heading towards us
    //     if (board.isShell(check) && board.getObjectAt(check)->getDirection() == -dir) return false;
    // }
    return false;
}

// int GameState::getEnemyDistance() const {
//     return getEnemyDistance(getPlayerTank()->getPosition());
// }

// int GameState::getEnemyDistance(Position position) const {
//     return getObjectsDistance(position, getEnemyTank()->getPosition());
// }

// int GameState::getObjectsDistance(Position obj1, Position obj2) const {
//     int dx = std::abs(obj2.x - obj1.x);
//     int dy = std::abs(obj2.y - obj1.y);
//
//     dx = std::min(dx, board.getHeight() - dx); // consider wrap-around vertically
//     dy = std::min(dy, board.getWidth() - dy); // consider wrap-around horizontally
//
//     return std::max(dx, dy);
// }


// bool GameState::isFacingWall() const {
//     return isWallInDirection(getPlayerTank()->getPosition(), getPlayerTank()->getDirection());
// }


// bool GameState::isWallInDirection(Position position, Direction::DirectionType direction) const {
//     Position next_position = position + Direction::getDirectionDelta(direction);
//     auto element = board.getObjectAt(next_position);
//     return dynamic_cast<Wall *>(element) != nullptr;
// }

// Action GameState::rotateTowardsWall() const {
//     auto current_player_direction = getPlayerTank()->getDirection();
//     std::vector<std::pair<Action, Direction::DirectionType> > rotations = {
//         {ROTATE_LEFT_EIGHTH, Direction::rotateDirection(current_player_direction, false, false)},
//         {ROTATE_RIGHT_EIGHTH, Direction::rotateDirection(current_player_direction, true, false)},
//         {ROTATE_LEFT_QUARTER, Direction::rotateDirection(current_player_direction, false, true)},
//         {ROTATE_RIGHT_QUARTER, Direction::rotateDirection(current_player_direction, true, true)}
//     };
//
//     for (const auto &rotation: rotations) {
//         auto cannon_new_direction = rotation.second;
//         if (isWallInDirection(getPlayerTank()->getPosition(), cannon_new_direction)) {
//             return rotation.first; // Rotate toward wall
//         }
//     }
//     return NONE; // No wall nearby
// }

// bool GameState::areObjectsInLine(Position obj1, Direction::DirectionType obj1_direction, Position obj2,
//                                  int max_distance) const {
//     auto direction_delta = Direction::getDirectionDelta(obj1_direction);
//     for (auto i = 0; i <= max_distance; i++) {
//         Position inline_position = board.wrapPosition(obj1 + direction_delta * i);
//
//         if (inline_position == obj2) {
//             return true;
//         }
//     }
//     return false;
// }

// Position GameState::calcNextPosition(Position position, Direction::DirectionType direction) const {
//     auto direction_delta = Direction::getDirectionDelta(direction);
//     return position + direction_delta;
// }

// bool GameState::canShootWall() const {
//     const auto &player_tank = getPlayerTank();
//     if (!player_tank) {
//         return false;
//     }
//     const auto tank_pos = player_tank->getPosition();
//     const auto tank_dir = player_tank->getDirection();
//     Position checkPos = tank_pos;
//     for (int i = 1; i <= 5; i++) {
//         // Check up to 5 squares ahead
//         if (isWallInDirection(checkPos, tank_dir)) {
//             return true;
//         }
//         checkPos = checkPos + tank_dir;
//     }
//
//     return false;
// }

bool MyBattleInfo::canShootEnemy() const {
    if (!getPlayerTank()) return false;
    // return canShootEnemy(getPlayerTank()->getPosition(), getPlayerTank()->getDirection());
    // TODO
    return false;
}

bool MyBattleInfo::canShootEnemy(const Position) const {
    if (!getPlayerTank()) return false;
    // return canShootEnemy(from, getPlayerTank()->getDirection());
    // TODO
    return false;
}

bool MyBattleInfo::canShootEnemy(const Direction::DirectionType) const {
    if (!getPlayerTank()) return false;
    // return canShootEnemy(getPlayerTank()->getPosition(), dir);
    // TODO
    return false;
}

bool MyBattleInfo::canShootEnemy(const Position, const Direction::DirectionType) const {
    if (!canShoot()) return false;
    // if (!getEnemyTank()) return false;
    // return isInLineOfSight(from, dir, getEnemyTank()->getPosition());
    // TODO
    return false;
}

ActionRequest MyBattleInfo::rotateTowards(const Direction::DirectionType) const {
    if (!getPlayerTank()) return ActionRequest::DoNothing;
    // return rotateTowards(getPlayerTank()->getDirection(), to);
    // TODO
    return ActionRequest::DoNothing;
}

ActionRequest MyBattleInfo::rotateTowards(const Position) const {
    if (!getPlayerTank()) return ActionRequest::DoNothing;

    // const Direction::DirectionType curr_dir = getPlayerTank()->getDirection();
    // for (Direction::DirectionType dir: {curr_dir, curr_dir + 45, curr_dir - 45, curr_dir + 90, curr_dir - 90}) {
    //     if (isInLineOfSight(getPlayerTank()->getPosition(), dir, to)) return rotateTowards(dir);
    // }
    // TODO

    return ActionRequest::RotateRight90;
}

ActionRequest MyBattleInfo::rotateTowards(const Direction::DirectionType from,
                                          const Direction::DirectionType to) const {
    if (from == to) return ActionRequest::DoNothing;

    // Find the shortest rotation path
    if (int diff = (from - to + 360) % 360; diff > 180) {
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

bool MyBattleInfo::isShellApproaching() const {
    if (!getPlayerTank()) return false;
    // TODO
    // return isShellApproaching(getPlayerTank()->getPosition());
    return false;
}

bool MyBattleInfo::isShellApproaching(const Position) const {
    // for (auto [id, shell]: board.getShells()) {
    //     if (isInLineOfSight(shell->getPosition(), shell->getDirection(), position)) {
    //         return true;
    //     }
    // }
    // TODO we need to think about this, how to predict shell's movement?

    return false;
}

bool MyBattleInfo::isEnemyNearby() const {
    // TODO
    // if (!getPlayerTank() || !getEnemyTank()) return false;
    // const Position player_pos = getPlayerTank()->getPosition();
    // const Position enemy_pos = getEnemyTank()->getPosition();
    // for (int i = 0; i < 8; ++i) {
    //     const Direction::DirectionType dir = Direction::getDirectionFromIndex(i);
    //     if (enemy_pos == player_pos + dir) {
    //         return true;
    //     }
    //     if (enemy_pos == player_pos + dir + dir) {
    //         return true;
    //     }
    // }
    return false;
}

// bool GameState::isLineOfSightClear(const Position start, const Position end,
//                                    const Direction::DirectionType direction) const {
//     Position current = start;
//     auto dir_delta = Direction::getDirectionDelta(direction);
//
//     while (current != end) {
//         current = board.wrapPosition(current + dir_delta);
//
//         if (current == end) {
//             break;
//         }
//
//         auto *obj = board.getObjectAt(current);
//         if (obj != nullptr && (dynamic_cast<Wall *>(obj) != nullptr ||
//                                (dynamic_cast<Tank *>(obj) != nullptr && current != end))) {
//             return false;
//         }
//     }
//
//     return true;
// }



