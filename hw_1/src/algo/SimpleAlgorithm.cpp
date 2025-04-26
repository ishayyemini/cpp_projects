#include "SimpleAlgorithm.h"

Action SimpleAlgorithm::decideAction(const GameState &state) {
    if (!state.getPlayerTank() || state.getPlayerTank()->isDestroyed() ||
        !state.getEnemyTank() || state.getEnemyTank()->isDestroyed()) {
        return NONE;
    }

    if (state.isShellApproaching(state.getPlayerTank()->getPosition())) {
        // Logger::debug("Player 2: Threatened By Shells");
        return moveIfThreatened(state);
    }

    if (state.canShootEnemy()) {
        // Logger::debug("Player 2: Enemy in direct line of fire. Shooting now.");
        return SHOOT;
    }

    if (state.getEnemyTank()->getAmmunition() == 0) {
        Direction::DirectionType to = Direction::getDirectionTo(state.getPlayerTank()->getPosition(),
                                                                state.getEnemyTank()->getPosition());
        Direction::DirectionType from = state.getPlayerTank()->getDirection();
        if (from == to && state.canShoot()) {
            // Logger::debug(
            //     "Player 2: Trying to shoot towards the enemy because the enemy tank wasted its entire ammo.");
            return SHOOT;
        }
        if (state.canShoot())
            // Logger::debug(
            //     "Player 2: Trying to rotate towards the enemy in order to shoot him because the enemy tank wasted its entire ammo.");
            return state.rotateTowards(to);
    }

    return NONE;
}
