#ifndef PLAYER1_H
#define PLAYER1_H
#include "MyActualPlayer.h"

class Player1 final : public MyActualPlayer {

public:
    Player1(int player_index,
            size_t x, size_t y,
            size_t max_steps, size_t num_shells): MyActualPlayer(player_index, x, y, max_steps, num_shells){
    }

    //todo: add deconstructor, block move and so on
};

#endif //PLAYER1_H
