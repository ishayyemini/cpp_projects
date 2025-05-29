#include "SimpleAlgorithm.h"

#include "Logger.h"

ActionRequest SimpleAlgorithm::getAction() {
    // TODO: maybe not just get battle info?
    if (turn_number % 2 == 0) {
        turn_number++;
        return ActionRequest::GetBattleInfo;
    }

    turn_number++;

    return ActionRequest::MoveForward;

    // TODO correct
    // if (!state->getPlayerTank() || state->getPlayerTank()->isDestroyed() ||
    //     !state->getEnemyTank() || state->getEnemyTank()->isDestroyed()) {
    //     Logger::getInstance().
    //             log("Player " + std::to_string(state->getPlayerId()) + ": Doing nothing since no tank(s)");
    //     return ActionRequest::DoNothing;
    // }

    if (state->isShellApproaching()) {
        Logger::getInstance().log("Player " + std::to_string(state->getPlayerId()) + ": Threatened by shells.");
        return moveIfThreatened();
    }

    if (state->isEnemyNearby()) {
        Logger::getInstance().log("Player " + std::to_string(state->getPlayerId()) + ": Threatened by enemy.");
        return moveIfThreatened();
    }

    if (state->canShootEnemy()) {
        Logger::getInstance().log(
            "Player " + std::to_string(state->getPlayerId()) + ": Enemy in direct line of fire. Shooting now.");
        return ActionRequest::Shoot;
    }

    // TODO can we know if they don't have ammo?
    // if (state->getEnemyTank()->getAmmunition() == 0) {
    //     Direction::DirectionType to = Direction::getDirectionTo(state->getPlayerTank()->getPosition(),
    //                                                             state->getEnemyTank()->getPosition());
    //     Direction::DirectionType from = state->getPlayerTank()->getDirection();
    //     if (from == to && state->canShoot()) {
    //         Logger::getInstance().log(
    //             "Player " + std::to_string(state->getPlayerId()) +
    //             ": Trying to shoot towards the enemy because the enemy tank wasted its entire ammo.");
    //         return ActionRequest::Shoot;
    //     }
    //     if (state->canShoot())
    //         Logger::getInstance().log(
    //             "Player " + std::to_string(state->getPlayerId()) +
    //             ": Trying to rotate towards the enemy in order to shoot him because the enemy tank wasted its entire ammo.");
    //     return state->rotateTowards(to);
    // }

    Logger::getInstance().log("Player " + std::to_string(state->getPlayerId()) + ": Doing nothing.");
    return ActionRequest::DoNothing;
}
