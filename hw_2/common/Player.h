//
// Created by Ishay Yemini on 26/05/2025.
//

#ifndef PLAYER_H
#define PLAYER_H

#include "SatelliteView.h"
#include "TankAlgorithm.h"

class Player {
public:
    Player(int player_index,
           size_t x, size_t y,
           size_t max_steps, size_t num_shells) {
    }

    virtual ~Player() {
    }

    virtual void updateTankWithBattleInfo
    (TankAlgorithm &tank, SatelliteView &satellite_view) = 0;
};


#endif //PLAYER_H
