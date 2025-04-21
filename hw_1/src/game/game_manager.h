//
// Created by rachel on 4/2/2025.
//

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include "board/direction.h"
#include "board/position.h"
#include "board_elements/tank.h"

enum Winner {
    TIE,
    PLAYER_1,
    PLAYER_2,
    NONE
};

class GameManager {
    int game_step = 0;
    bool game_over = false;
    Winner winner = NONE;

    bool moveForward(Tank tank, Position position);
    bool moveBackward();
    void rotate(Direction direction); //should call tank.rotate


    public:
    GameManager();
    Winner start_game();







};

#endif //GAME_MANAGER_H
