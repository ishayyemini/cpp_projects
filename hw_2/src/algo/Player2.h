#ifndef PLAYER2_H
#define PLAYER2_H
#include <vector>

#include "Player.h"

class Player2 final : public Player {
    int player_index;
    size_t x;
    size_t y;
    size_t max_steps;
    size_t num_shells;

    std::vector<std::vector<char> > createPartialBoard(const SatelliteView &satellite_view) const;

public:
    Player2(int player_index,
            size_t x, size_t y,
            size_t max_steps, size_t num_shells): Player(player_index, x, y, max_steps, num_shells),
                                                  player_index(player_index), x(x), y(y),
                                                  max_steps(max_steps),
                                                  num_shells(num_shells) {
    }

    void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) override;
};

#endif //PLAYER2_H
