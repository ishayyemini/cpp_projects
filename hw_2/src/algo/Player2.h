#ifndef PLAYER2_H
#define PLAYER2_H
#include "MyActualPlayer.h"

class Player2 final : public MyActualPlayer {

public:
    Player2(int player_index,
            size_t x, size_t y,
            size_t max_steps, size_t num_shells): MyActualPlayer(player_index, x, y, max_steps, num_shells){
    }
};

#endif //PLAYER2_H
