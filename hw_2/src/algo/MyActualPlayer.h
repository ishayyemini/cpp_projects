#ifndef TANK_GAME_MYACTUALPLAYER_H
#define TANK_GAME_MYACTUALPLAYER_H

#include <vector>
#include "Player.h"

class MyActualPlayer : public Player {
    int player_index;
    size_t x;
    size_t y;
    size_t max_steps;
    size_t num_shells;

    std::vector<std::vector<char> > createBoardFromSatellite(const SatelliteView &satellite_view) const;

    MyActualPlayer(const MyActualPlayer &) = delete;

    MyActualPlayer &operator=(const MyActualPlayer &) = delete;

    MyActualPlayer(MyActualPlayer &&) = delete;

    MyActualPlayer &operator=(MyActualPlayer &&) = delete;

protected:
    MyActualPlayer(int player_index,
                   size_t x, size_t y,
                   size_t max_steps, size_t num_shells): Player(player_index, x, y, max_steps, num_shells),
                                                         player_index(player_index), x(x), y(y), max_steps(max_steps),
                                                         num_shells(num_shells) {
    }

public:
    void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) override;

    ~MyActualPlayer() override = default;
};


#endif //TANK_GAME_MYACTUALPLAYER_H
