#include "SimpleAlgorithm.h"

Action SimpleAlgorithm::decideAction(const GameState &state) {
    if (state.getPlayerTank() == nullptr || state.getPlayerTank()->isDestroyed()) return NONE;

    int enemy_tank_distance = state.getEnemyDistance();

    // step 1: check if the tank is at risk
    if (enemy_tank_distance <= 3 || state.getApproachingShellsPosition().size() > 0) {
        return handleImmediateThreat(state, enemy_tank_distance);
    }

    // Shoot on the other tank if can
    if (state.isInLineOfSight(state.getEnemyTank()->getPosition())) {
        return SHOOT;
    }

    // If rotation can face enemy, rotate
    if (state.canRotateToFaceEnemy()) {
        Action action = state.getActionToPosition(state.getEnemyTank()->getPosition());
        if (action != NONE && action != MOVE_FORWARD && action != MOVE_BACKWARD && action != SHOOT) {
            return action;
        }
    }

    return handleFutureThreat(state);
}

Action SimpleAlgorithm::handleFutureThreat(const GameState &state) const {
    //distance > 3:
    //always check if we have an escape way - meaning at least two different directions we
    //can move to (2 because we don't know in which direction the enemy 2 will come
    //if so, don't do anything, else if we have less, but we have a wall next to us, shoot him
    auto closeSafeDirections = state.getSafeDirections(state.getPlayerTank()->getPosition()).size();
    if (closeSafeDirections >= 2 || state.getPlayerTank()->getAmmunition() == 0) {
        return NONE;
    }

    //if we are pointing to wall, shoot
    if (state.doesPlayerTankFacingWall()) {
        return SHOOT;
    }
    //else rotate towards it
    return state.rotateTowardsWall();
}





