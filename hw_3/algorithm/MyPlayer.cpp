#include "MyPlayer.h"

#include "MyBattleInfo.h"

namespace Algorithm_322868852_340849710 {
    MyPlayer::MyPlayer(int player_index,
                       size_t x, size_t y,
                       size_t max_steps, size_t num_shells) : player_index(player_index), x(x), y(y),
                                                              max_steps(max_steps),
                                                              num_shells(num_shells) {
    }


    void MyPlayer::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) {
        auto battle_info = MyBattleInfo(createBoardFromSatellite(satellite_view), player_index, max_steps, num_shells);
        tank.updateBattleInfo(battle_info);
    }

    std::vector<std::vector<char> > MyPlayer::createBoardFromSatellite(const SatelliteView &satellite_view) const {
        std::vector board(x, std::vector<char>(y));
        for (size_t i{0}; i < x; i++) {
            for (size_t j{0}; j < y; j++) {
                board[i][j] = satellite_view.getObjectAt(i, j);
            }
        }
        return board;
    }
}
