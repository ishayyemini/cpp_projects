//
// Created by Ishay Yemini on 27/05/2025.
//

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "PlayerFactory.h"
#include "TankAlgorithmFactory.h"

class GameManager {
    PlayerFactory *playerFactory;
    TankAlgorithmFactory *tankAlgorithmFactory;

public:
    GameManager(const PlayerFactory &player_factory,
                const TankAlgorithmFactory &tank_algorithm_factory): playerFactory(&player_factory),
                                                                     tankAlgorithmFactory(&tank_algorithm_factory) {
    }

    void readBoard(const std::string &file_name);

    void run();
};

#endif //GAMEMANAGER_H
