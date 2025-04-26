#include "Algorithm.h"


Action Algorithm::moveIfThreatened(const GameState &state) const {
    if (!state.getPlayerTank()) return NONE;

    Position player_pos = state.getPlayerTank()->getPosition();
    Direction::DirectionType player_dir = state.getPlayerTank()->getDirection();

    // we'll try first moving forward in the current direction
    Position forward_pos = state.getBoard().wrapPosition(player_pos + player_dir);

    if (state.isSafePosition(forward_pos)) {
        return MOVE_FORWARD;
    }

    // If we can't move forward in the current direction, we'll find a safe cell around us and rotate towards it
    for (int i = 0; i < 8; ++i) {
        Direction::DirectionType try_dir = Direction::getDirectionFromIndex(i);
        Position try_pos = state.getBoard().wrapPosition(player_pos + try_dir);
        if (state.isSafePosition(try_pos)) {
            return state.rotateTowards(try_dir);
        }
    }

    return NONE;
}

