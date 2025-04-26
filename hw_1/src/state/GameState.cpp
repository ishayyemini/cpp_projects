#include "GameState.h"

#include <limits>
#include <set>

#include "Wall.h"
#include "SimpleAlgorithm.h"

GameState::GameState(Board &board, const int player_id): board(board), player_id(player_id) {
}

Board &GameState::getBoard() const { return board; }

Tank *GameState::getPlayerTank() const { return board.getPlayerTank(player_id); }

Tank *GameState::getEnemyTank() const { return board.getPlayerTank(player_id == 1 ? 2 : 1); }

bool GameState::isSafePosition(Position position, bool immediate_safe) const {
    // Check if there's any object at the position
    if (board.getObjectAt(position) != nullptr) {
        return false; // Position has an object, not safe
    }
    // If only checking immediate safety, we're done
    if (immediate_safe) {
        return true;
    }

    // Check for approaching shells
    for (const auto &[id, shell_pos]: board.getShells()) {
        Shell *shell = dynamic_cast<Shell *>(board.getObjectAt(shell_pos));
        if (!shell) continue;

        // Check if shell will pass through this position
        if (areObjectsInLine(shell_pos, shell->getDirection(), position,
                             std::max(board.getWidth(), board.getHeight()))) {
            return false;
        }
    }

    return true;
}

std::vector<Direction::DirectionType> GameState::getSafeDirections(Position position) const {
    std::vector<Direction::DirectionType> safe_directions;

    for (int i = 0; i < Direction::getDirectionSize(); i++) {
        auto direction = Direction::getDirectionFromIndex(i);
        Position next_position = position + Direction::getDirectionDelta(direction);
        if (isSafePosition(next_position)) {
            safe_directions.push_back(direction);
        }
    }
    return safe_directions;
}


std::vector<Position> GameState::getNearbyEmptyPositions(Position position, int steps_num) const {
    std::vector<Position> result;
    std::set<Position> visited;

    struct State {
        Position pos;
        int steps;
    };

    std::queue<State> queue;
    queue.push({position, 0});
    visited.insert(position);

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        // If we've used all steps, don't explore further
        if (current.steps >= steps_num) continue;

        // Try all directions
        for (int i = 0; i < Direction::getDirectionSize(); i++) {
            auto dir = Direction::getDirectionFromIndex(i);
            Position next_pos = current.pos + Direction::getDirectionDelta(dir);

            // Handle wrapping
            next_pos.x = (next_pos.x + board.getHeight()) % board.getHeight();
            next_pos.y = (next_pos.y + board.getWidth()) % board.getWidth();

            // Check if position is valid and empty
            if (visited.find(next_pos) == visited.end() && isEmptyPosition(next_pos)) {
                result.push_back(next_pos);
                visited.insert(next_pos);
                queue.push({next_pos, current.steps + 1});
            }
        }
    }

    return result;
}

Action GameState::getActionToPosition(Position target_position) const {
    Position current = getPlayerTank()->getPosition();
    Direction::DirectionType current_dir = getPlayerTank()->getDirection();

    // If already at the target
    if (current == target_position) {
        return NONE;
    }

    // Calculate relative direction to target
    int dx = target_position.x - current.x;
    int dy = target_position.y - current.y;

    // Handle board wrapping
    if (std::abs(dx) > board.getHeight() / 2) {
        dx = dx > 0 ? dx - board.getHeight() : dx + board.getHeight();
    }
    if (std::abs(dy) > board.getWidth() / 2) {
        dy = dy > 0 ? dy - board.getWidth() : dy + board.getWidth();
    }

    // Determine target direction
    Direction::DirectionType target_dir;
    if (dx < 0 && dy == 0) target_dir = Direction::UP;
    else if (dx > 0 && dy == 0) target_dir = Direction::DOWN;
    else if (dx == 0 && dy < 0) target_dir = Direction::LEFT;
    else if (dx == 0 && dy > 0) target_dir = Direction::RIGHT;
    else if (dx < 0 && dy < 0) target_dir = Direction::UP_LEFT;
    else if (dx < 0 && dy > 0) target_dir = Direction::UP_RIGHT;
    else if (dx > 0 && dy < 0) target_dir = Direction::DOWN_LEFT;
    else target_dir = Direction::DOWN_RIGHT;

    // If already facing the target direction, move forward
    if (current_dir == target_dir) {
        return MOVE_FORWARD;
    }

    // Need to rotate - find shortest path
    int current_index = current_dir / 45;
    int target_index = target_dir / 45;
    int diff = (target_index - current_index + 8) % 8;

    if (diff == 1) return ROTATE_RIGHT_EIGHTH;
    if (diff == 7) return ROTATE_LEFT_EIGHTH;
    if (diff == 2) return ROTATE_RIGHT_QUARTER;
    if (diff == 6) return ROTATE_LEFT_QUARTER;

    // For larger rotations, take biggest step in correct direction
    return (diff <= 4) ? ROTATE_RIGHT_QUARTER : ROTATE_LEFT_QUARTER;
}

bool GameState::isShellApproaching(const int threat_threshold) const {
    return getApproachingShellsPosition(threat_threshold).size() > 0;
}

bool GameState::isObjectInLine(Position object_position, int distance) const {
    return areObjectsInLine(getPlayerTank()->getPosition(), getPlayerTank()->getDirection(), object_position,
                            distance);
}

bool GameState::canRotateToFaceEnemy() const {
    Tank *player = getPlayerTank();
    Tank *enemy = getEnemyTank();
    if (!player || !enemy) return false;
    Direction::DirectionType current_dir = player->getDirection();
    Position player_pos = player->getPosition();
    Position enemy_pos = enemy->getPosition();
    // Check all possible one-step rotations
    const std::vector possible_directions = {
        Direction::rotateDirection(current_dir, true, false), // 45° right
        Direction::rotateDirection(current_dir, false, false), // 45° left
        Direction::rotateDirection(current_dir, true, true), // 90° right
        Direction::rotateDirection(current_dir, false, true) // 90° left
    };
    for (const auto &new_dir: possible_directions) {
        // First check if enemy would be in line with the new direction
        if (!areObjectsInLine(player_pos, new_dir, enemy_pos, std::max(board.getWidth(), board.getHeight()))) {
            continue;
        }
        // Now check for obstacles in the path
        Position current = player_pos;
        auto dir_delta = Direction::getDirectionDelta(new_dir);
        bool clear_path = true;
        while (current != enemy_pos && clear_path) {
            current = current + dir_delta;
            // Handle board wrapping
            current.x = (current.x + board.getHeight()) % board.getHeight();
            current.y = (current.y + board.getWidth()) % board.getWidth();
            // Stop if we reached the enemy
            if (current == enemy_pos) {
                break;
            }
            // Check for obstacles
            auto *obj = board.getObjectAt(current);
            if (obj != nullptr && (dynamic_cast<Wall *>(obj) != nullptr || (
                                       dynamic_cast<Tank *>(obj) != nullptr && current != enemy_pos))) {
                clear_path = false;
            }
        }

        if (clear_path) {
            return true; // This rotation would give a clear line of sight
        }
    }

    return false; // No rotation gives a clear line of sight
}

bool GameState::isEmptyPosition(Position position) const {
    return board.getObjectAt(position) == nullptr;
}

std::vector<Position> GameState::getApproachingShellsPosition(int threat_threshold, bool get_closest) const {
    Position player_position = getPlayerTank()->getPosition();
    std::vector<Position> approaching_shells;
    std::vector<Position> closest_shells;
    int closest_distance = std::numeric_limits<int>::max();

    for (const auto &shell_pos: board.getShells()) {
        int distance = getObjectsDistance(player_position, shell_pos.second);
        int max_distance = (threat_threshold == -1) ? distance : threat_threshold;
        Shell *shell = dynamic_cast<Shell *>(board.getObjectAt(shell_pos.second));
        if (shell == nullptr) continue;

        Direction::DirectionType shell_direction = shell->getDirection();
        if (areObjectsInLine(shell_pos.second, shell_direction, player_position, max_distance)) {
            approaching_shells.push_back(shell_pos.second);
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_shells.push_back(shell_pos.second);
            }
        }
    }

    if (get_closest) {
        return closest_shells;
    }
    return approaching_shells;
}

bool GameState::isInLineOfSight(Position position) const {
    Position player_pos = getPlayerTank()->getPosition();
    Direction::DirectionType player_dir = getPlayerTank()->getDirection();

    // First check if the target is in the direction the tank is facing
    if (!areObjectsInLine(player_pos, player_dir, position,
                          std::max(board.getWidth(), board.getHeight()))) {
        return false;
    }

    // Now check if there are obstacles between tank and target
    Position current = player_pos;
    auto dir_delta = Direction::getDirectionDelta(player_dir);

    while (current != position) {
        current = current + dir_delta;

        // Handle board wrapping
        current.x = (current.x + board.getHeight()) % board.getHeight();
        current.y = (current.y + board.getWidth()) % board.getWidth();

        // Stop if we reached the target
        if (current == position) {
            break;
        }

        // Check for obstacles
        auto *obj = board.getObjectAt(current);
        if (obj != nullptr && dynamic_cast<Wall *>(obj) != nullptr) {
            return false; // Wall blocks line of sight
        }

        // Tank blocks line of sight too
        if (obj != nullptr && dynamic_cast<Tank *>(obj) != nullptr &&
            current != position) {
            return false;
        }
    }

    return true;
}

int GameState::getEnemyDistance() const {
    return getEnemyDistance(getPlayerTank()->getPosition());
}

int GameState::getEnemyDistance(Position position) const {
    return getObjectsDistance(position, getEnemyTank()->getPosition());
}

int GameState::getObjectsDistance(Position obj1, Position obj2) const {
    int dx = std::abs(obj2.x - obj1.x);
    int dy = std::abs(obj2.y - obj1.y);

    dx = std::min(dx, board.getHeight() - dx); // consider wrap-around vertically
    dy = std::min(dy, board.getWidth() - dy); // consider wrap-around horizontally

    return std::max(dx, dy);
}


bool GameState::isFacingWall() const {
    return isWallInDirection(getPlayerTank()->getPosition(), getPlayerTank()->getDirection());
}


bool GameState::isWallInDirection(Position position, Direction::DirectionType direction) const {
    Position next_position = position + Direction::getDirectionDelta(direction);
    auto element = board.getObjectAt(next_position);
    return dynamic_cast<Wall *>(element) != nullptr;
}

Action GameState::rotateTowardsWall() const {
    auto current_player_direction = getPlayerTank()->getDirection();
    std::vector<std::pair<Action, Direction::DirectionType> > rotations = {
        {ROTATE_LEFT_EIGHTH, Direction::rotateDirection(current_player_direction, false, false)},
        {ROTATE_RIGHT_EIGHTH, Direction::rotateDirection(current_player_direction, true, false)},
        {ROTATE_LEFT_QUARTER, Direction::rotateDirection(current_player_direction, false, true)},
        {ROTATE_RIGHT_QUARTER, Direction::rotateDirection(current_player_direction, true, true)}
    };

    for (const auto &rotation: rotations) {
        auto cannon_new_direction = rotation.second;
        if (isWallInDirection(getPlayerTank()->getPosition(), cannon_new_direction)) {
            return rotation.first; // Rotate toward wall
        }
    }
    return NONE; // No wall nearby
}

bool GameState::areObjectsInLine(Position obj1, Direction::DirectionType obj1_direction, Position obj2,
                                 int max_distance) const {
    auto direction_delta = Direction::getDirectionDelta(obj1_direction);
    for (auto i = 0; i <= max_distance; i++) {
        Position inline_position = obj1 + direction_delta * i;

        // Handle board wrapping
        inline_position.x = (inline_position.x + board.getHeight()) % board.getHeight();
        inline_position.y = (inline_position.y + board.getWidth()) % board.getWidth();

        if (inline_position == obj2) {
            return true;
        }
    }
    return false;
}

Position GameState::calcNextPosition(Position position, Direction::DirectionType direction) const {
    auto direction_delta = Direction::getDirectionDelta(direction);
    return position + direction_delta;
}

bool GameState::canShootWall() const {
    const auto &player_tank = board.getPlayerTank(1);
    if (!player_tank) {
        return false;
    }
    const auto tank_pos = player_tank->getPosition();
    const auto tank_dir = player_tank->getDirection();
    Position checkPos = tank_pos;
    for (int i = 1; i <= 5; i++) {
        // Check up to 5 squares ahead
        if (isWallInDirection(checkPos, tank_dir)) {
            return true;
        }
        checkPos = checkPos + tank_dir;
    }

    return false;
}




