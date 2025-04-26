#include "GameState.h"

#include <limits>
#include <Wall.h>
#include <algo/SimpleAlgorithm.h>

GameState::GameState(Board &board, const int player_id): board(board), player_id(player_id) {}

Board& GameState::getBoard() const { return board; }

Tank *GameState::getPlayerTank() const { return board.getPlayerTank(player_id); }

Tank *GameState::getEnemyTank() const { return board.getPlayerTank(player_id == 1 ? 2 : 1); }

bool GameState::isSafePosition(Position position, bool immediate_safe) const {
    //todo: implement this
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
    //todo: implement this
    return {};
}

Action GameState::getActionToPosition(Position target_position) const {
    //todo: implement this
    return NONE;
}

bool GameState::isShellApproaching(int threat_threshold) const {
    return getApproachingShellsPosition(threat_threshold).size() > 0;
}

bool GameState::isObjectInLine(Position object_position, int distance) const {
    return areObjectsInLine(getPlayerTank()->getPosition(), getPlayerTank()->getDirection(), object_position,
                            distance);
}

bool GameState::isEmptyPosition(Position position) const {
    return board.getObjectAt(position) == nullptr;
}

std::vector<Position> GameState::getApproachingShellsPosition(int threat_threshold, bool get_closest) const {
    Position player_position = getPlayerTank()->getPosition();
    std::vector<Position> approaching_shells;
    std::vector<Position> closest_shells;
    int closest_distance = std::numeric_limits<int>::max();

    for (const auto &shell_pos: board.getShellsPos()) {
        int distance = getObjectsDistance(player_position, shell_pos.second);
        int max_distance = (threat_threshold == -1) ? distance : threat_threshold;
        Direction::DirectionType shell_direction = board.getShell(shell_pos.first)->getDirection();
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


bool GameState::doesPlayerTankFacingWall() const {
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
        Position inline_position = obj1 + direction_delta.scalar_multiplication(i);
        if (inline_position == obj2) {
            return true;
        }
    }
    return false;
}






