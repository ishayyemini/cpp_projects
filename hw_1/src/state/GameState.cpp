#include "GameState.h"

#include <limits>
#include <set>

#include "Direction.h"
#include "SimpleAlgorithm.h"

GameState::GameState(Board &board, const int player_id): board(board), player_id(player_id) {
}

Board &GameState::getBoard() const { return board; }

Tank *GameState::getPlayerTank() const { return board.getPlayerTank(player_id); }

bool GameState::canShoot() const {
    if (auto t = getPlayerTank()) return t->getAmmunition() > 0 && t->getCooldown() == 0;
    return false;
}

Tank *GameState::getEnemyTank() const { return board.getPlayerTank(player_id == 1 ? 2 : 1); }

bool GameState::isSafePosition(const Position position, const bool immediate_safe) const {
    if (board.isOccupied(position)) return false;

    // If only checking immediate safety, we're done
    if (immediate_safe) return true;

    return !isShellApproaching(position);
}

std::vector<Direction::DirectionType> GameState::getSafeDirections(const Position position) const {
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

std::vector<Position> GameState::getNearbyEmptyPositions(const Position position) const {
    std::vector<Position> empty_positions;

    for (int i = 0; i < Direction::getDirectionSize(); i++) {
        auto direction = Direction::getDirectionFromIndex(i);
        Position next_position = position + direction;
        if (!board.isOccupied(next_position)) {
            empty_positions.push_back(next_position);
        }
    }

    return empty_positions;
}

// std::vector<Position> GameState::getNearbyEmptyPositions(Position position, int steps_num) const {
//     std::vector<Position> result;
//     std::set<Position> visited;
//
//     struct State {
//         Position pos;
//         int steps;
//     };
//
//     std::queue<State> queue;
//     queue.push({position, 0});
//     visited.insert(position);
//
//     while (!queue.empty()) {
//         auto current = queue.front();
//         queue.pop();
//
//         // If we've used all steps, don't explore further
//         if (current.steps >= steps_num) continue;
//
//         // Try all directions
//         for (int i = 0; i < Direction::getDirectionSize(); i++) {
//             auto dir = Direction::getDirectionFromIndex(i);
//             Position next_pos = board.wrapPosition(current.pos + Direction::getDirectionDelta(dir));
//
//             // Check if position is valid and empty
//             if (visited.find(next_pos) == visited.end() && isEmptyPosition(next_pos)) {
//                 result.push_back(next_pos);
//                 visited.insert(next_pos);
//                 queue.push({next_pos, current.steps + 1});
//             }
//         }
//     }
//
//     return result;
// }

// Action GameState::getActionToPosition(Position target_position) const {
//     Position current = getPlayerTank()->getPosition();
//     Direction::DirectionType current_dir = getPlayerTank()->getDirection();
//
//     // If already at the target
//     if (current == target_position) {
//         return NONE;
//     }
//
//     // Calculate relative direction to target
//     int dx = target_position.x - current.x;
//     int dy = target_position.y - current.y;
//
//     // Handle board wrapping
//     if (std::abs(dx) > board.getHeight() / 2) {
//         dx = dx > 0 ? dx - board.getHeight() : dx + board.getHeight();
//     }
//     if (std::abs(dy) > board.getWidth() / 2) {
//         dy = dy > 0 ? dy - board.getWidth() : dy + board.getWidth();
//     }
//
//     // Determine target direction
//     Direction::DirectionType target_dir;
//     if (dx < 0 && dy == 0) target_dir = Direction::UP;
//     else if (dx > 0 && dy == 0) target_dir = Direction::DOWN;
//     else if (dx == 0 && dy < 0) target_dir = Direction::LEFT;
//     else if (dx == 0 && dy > 0) target_dir = Direction::RIGHT;
//     else if (dx < 0 && dy < 0) target_dir = Direction::UP_LEFT;
//     else if (dx < 0 && dy > 0) target_dir = Direction::UP_RIGHT;
//     else if (dx > 0 && dy < 0) target_dir = Direction::DOWN_LEFT;
//     else target_dir = Direction::DOWN_RIGHT;
//
//     // If already facing the target direction, move forward
//     if (current_dir == target_dir) {
//         return MOVE_FORWARD;
//     }
//
//     // Need to rotate - find shortest path
//     int current_index = current_dir / 45;
//     int target_index = target_dir / 45;
//     int diff = (target_index - current_index + 8) % 8;
//
//     if (diff == 1) return ROTATE_RIGHT_EIGHTH;
//     if (diff == 7) return ROTATE_LEFT_EIGHTH;
//     if (diff == 2) return ROTATE_RIGHT_QUARTER;
//     if (diff == 6) return ROTATE_LEFT_QUARTER;
//
//     // For larger rotations, take biggest step in correct direction
//     return (diff <= 4) ? ROTATE_RIGHT_QUARTER : ROTATE_LEFT_QUARTER;
// }

// bool GameState::isShellApproaching(const int threat_threshold) const {
//     return !getApproachingShellsPosition(threat_threshold).empty();
// }

// bool GameState::isObjectInLine(Position object_position, int distance) const {
//     return areObjectsInLine(getPlayerTank()->getPosition(), getPlayerTank()->getDirection(), object_position,
//                             distance);
// }

// bool GameState::canRotateToFaceEnemy() const {
//     Tank *player = getPlayerTank();
//     Tank *enemy = getEnemyTank();
//     if (!player || !enemy) return false;
//
//     Position player_pos = player->getPosition();
//     Position enemy_pos = enemy->getPosition();
//     const std::vector possible_directions = {
//         Direction::rotateDirection(player->getDirection(), true, false),
//         Direction::rotateDirection(player->getDirection(), false, false),
//         Direction::rotateDirection(player->getDirection(), true, true),
//         Direction::rotateDirection(player->getDirection(), false, true)
//     };
//
//     for (const auto &new_dir: possible_directions) {
//         if (areObjectsInLine(player_pos, new_dir, enemy_pos, std::max(board.getWidth(), board.getHeight())) &&
//             isLineOfSightClear(player_pos, enemy_pos, new_dir)) {
//             return true;
//         }
//     }
//
//     return false;
// }

// bool GameState::isEmptyPosition(Position position) const {
//     return board.getObjectAt(position) == nullptr;
// }

// std::vector<Position> GameState::getApproachingShellsPosition(int threat_threshold, bool get_closest) const {
//     Position player_position = getPlayerTank()->getPosition();
//     std::vector<Position> approaching_shells;
//     std::vector<Position> closest_shells;
//     int closest_distance = std::numeric_limits<int>::max();
//
//     for (const auto &shell_pos: board.getShells()) {
//         int distance = getObjectsDistance(player_position, shell_pos.second);
//         int max_distance = (threat_threshold == -1) ? distance : threat_threshold;
//         Shell *shell = dynamic_cast<Shell *>(board.getObjectAt(shell_pos.second));
//         if (shell == nullptr) continue;
//
//         Direction::DirectionType shell_direction = shell->getDirection();
//         if (areObjectsInLine(shell_pos.second, shell_direction, player_position, max_distance)) {
//             approaching_shells.push_back(shell_pos.second);
//             if (distance < closest_distance) {
//                 closest_distance = distance;
//                 closest_shells.push_back(shell_pos.second);
//             }
//         }
//     }
//
//     if (get_closest) {
//         return closest_shells;
//     }
//     return approaching_shells;
// }

bool GameState::isInLineOfSight(const Position target) const {
    if (!getPlayerTank()) return false;
    return isInLineOfSight(getPlayerTank()->getPosition(), getPlayerTank()->getDirection(), target);
}

bool GameState::isInLineOfSight(const Position from, const Direction::DirectionType dir, const Position target) const {
    Position check = from;
    for (int i = 1; i <= std::max(board.getWidth(), board.getHeight()); i++) {
        check = getBoard().wrapPosition(check + dir);
        if (target == check) return true;
        if (board.isWall(check)) return false;
        // Only if shell is heading towards us
        if (board.isShell(check) && board.getObjectAt(check)->getDirection() == -dir) return false;
    }
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

bool GameState::canShootEnemy() const {
    if (!getPlayerTank()) return false;
    return canShootEnemy(getPlayerTank()->getPosition(), getPlayerTank()->getDirection());
}

bool GameState::canShootEnemy(const Position from) const {
    if (!getPlayerTank()) return false;
    return canShootEnemy(from, getPlayerTank()->getDirection());
}

bool GameState::canShootEnemy(const Direction::DirectionType dir) const {
    if (!getPlayerTank()) return false;
    return canShootEnemy(getPlayerTank()->getPosition(), dir);
}

bool GameState::canShootEnemy(const Position from, const Direction::DirectionType dir) const {
    if (!canShoot()) return false;
    if (!getEnemyTank()) return false;
    return isInLineOfSight(from, dir, getEnemyTank()->getPosition());
}

Action GameState::rotateTowards(const Direction::DirectionType to) const {
    if (!getPlayerTank()) return NONE;
    return rotateTowards(getPlayerTank()->getDirection(), to);
}

Action GameState::rotateTowards(const Position to) const {
    if (!getPlayerTank()) return NONE;

    const Direction::DirectionType curr_dir = getPlayerTank()->getDirection();
    for (Direction::DirectionType dir: {curr_dir, curr_dir + 45, curr_dir - 45, curr_dir + 90, curr_dir - 90}) {
        if (isInLineOfSight(getPlayerTank()->getPosition(), dir, to)) return rotateTowards(dir);
    }

    return ROTATE_RIGHT_QUARTER;
}

Action GameState::rotateTowards(const Direction::DirectionType from, const Direction::DirectionType to) const {
    if (from == to) return MOVE_FORWARD;

    // Find the shortest rotation path
    if (int diff = (from - to + 360) % 360; diff > 180) {
        diff = 360 - diff;
        // Clockwise is shorter
        if (diff == 45) return ROTATE_RIGHT_EIGHTH;
        if (diff == 90) return ROTATE_RIGHT_QUARTER;
        if (diff == 135) return ROTATE_RIGHT_QUARTER;
        if (diff == 180) return ROTATE_RIGHT_QUARTER;
    } else {
        // Counter-clockwise is shorter or equal
        if (diff == 45) return ROTATE_LEFT_EIGHTH;
        if (diff == 90) return ROTATE_LEFT_QUARTER;
        if (diff == 135) return ROTATE_LEFT_QUARTER;
        if (diff == 180) return ROTATE_LEFT_QUARTER;
    }

    return NONE;
}

bool GameState::isShellApproaching() const {
    if (!getPlayerTank()) return false;
    return isShellApproaching(getPlayerTank()->getPosition());
}

bool GameState::isShellApproaching(const Position position) const {
    for (auto [id, shell]: board.getShells()) {
        if (isInLineOfSight(shell->getPosition(), shell->getDirection(), position)) {
            return true;
        }
    }

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



