#include "SimpleAlgorithm.h"

#include "Logger.h"

Action SimpleAlgorithm::decideAction(const GameState &state) {
    if (!state.getPlayerTank() || state.getPlayerTank()->isDestroyed() ||
        !state.getEnemyTank() || state.getEnemyTank()->isDestroyed()) {
        Logger::getInstance().log("Player " + std::to_string(state.getPlayerId()) + ": Doing nothing since no tank(s)");
        return NONE;
    }

    if (state.isShellApproaching()) {
        Logger::getInstance().log("Player " + std::to_string(state.getPlayerId()) + ": Threatened by shells.");
        return moveIfThreatened(state);
    }

    if (state.isEnemyNearby()) {
        Logger::getInstance().log("Player " + std::to_string(state.getPlayerId()) + ": Threatened by enemy.");
        return moveIfThreatened(state);
    }

    if (state.canShootEnemy()) {
        Logger::getInstance().log(
            "Player " + std::to_string(state.getPlayerId()) + ": Enemy in direct line of fire. Shooting now.");
        return SHOOT;
    }

    if (state.getEnemyTank()->getAmmunition() == 0) {
        Direction::DirectionType to = Direction::getDirectionTo(state.getPlayerTank()->getPosition(),
                                                                state.getEnemyTank()->getPosition());
        Direction::DirectionType from = state.getPlayerTank()->getDirection();
        if (from == to && state.canShoot()) {
            Logger::getInstance().log(
                "Player " + std::to_string(state.getPlayerId()) +
                ": Trying to shoot towards the enemy because the enemy tank wasted its entire ammo.");
            return SHOOT;
        }
        if (state.canShoot())
            Logger::getInstance().log(
                "Player " + std::to_string(state.getPlayerId()) +
                ": Trying to rotate towards the enemy in order to shoot him because the enemy tank wasted its entire ammo.");
        return state.rotateTowards(to);
    }

    Logger::getInstance().log("Player " + std::to_string(state.getPlayerId()) + ": Doing nothing.");
    return NONE;
}
