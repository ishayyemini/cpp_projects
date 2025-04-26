#include "SimpleAlgorithm.h"

Action SimpleAlgorithm::decideAction(const GameState &state) const {
    int enemy_tank_distance = state.getEnemyDistance();

    // step 1: check if the tank is at risk
    if (enemy_tank_distance <= 3 || state.getApproachingShellsPosition().size() > 0) {
        return handleImmediateThreat(state, enemy_tank_distance);
    }
    return handleFutureThreat(state);
}

Action SimpleAlgorithm::handleFutureThreat(const GameState &state) {
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

Action SimpleAlgorithm::handleImmediateThreat(const GameState &state, int tankDistance) const {
    //taking all the shells not only the closest because the player will die anyway no matter if is distance 1 or 2
    auto approachingShells = state.getApproachingShellsPosition(2);
    if (!approachingShells.empty()) {
        for (const auto &shell_position: approachingShells) {
            if (state.isObjectInLine(shell_position, 2) && state.getPlayerTank()->getAmmunition() > 0) {
                return SHOOT;
            }
        }
        return escape(state, false); // TODO: change this to escape from a specific direction
    }

    bool canChangeDirection = (tankDistance == 3) || !state.isShellApproaching(4);

    if (tankDistance <= 2 && state.isObjectInLine(state.getEnemyTank()->getPosition(), tankDistance)) {
        // If we have ammo, shoot - maybe the enemy has no ammo and we can win
        // Otherwise, we don't have time to escape, so we should try to collide with the other tank
        if (state.getPlayerTank()->getAmmunition() > 0) {
            return SHOOT;
        }
        return MOVE_FORWARD;
    }
    return escape(state, canChangeDirection);
}


Action SimpleAlgorithm::escape(const GameState &state, bool can_change_direction, Action default_action) const {
    // if we cannot change direction, we can only check if moving forward is possible and immediate safe
    auto forward_position = state.getPlayerTank()->getPosition() + Direction::getDirectionDelta(
                                state.getPlayerTank()->getDirection());
    if (state.isEmptyPosition(forward_position) && state.isSafePosition(forward_position, !can_change_direction)) {
        return MOVE_FORWARD;
    }
    if (!can_change_direction) {
        return default_action;
    }
    // let's check which safe direction we can go to. if there aren't any, return default action
    auto empty_positions = state.getNearbyEmptyPositions();
    for (auto position: empty_positions) {
        if (state.isSafePosition(position, false)) {
            return state.getActionToPosition(position);
        }
    }
    return default_action;
}
