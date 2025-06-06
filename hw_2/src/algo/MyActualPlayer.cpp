#include "MyActualPlayer.h"
#include "MyBattleInfo.h"

void MyActualPlayer::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) {
    auto battle_info = MyBattleInfo(createBoardFromSatellite(satellite_view), player_index, max_steps, num_shells);
    tank.updateBattleInfo(battle_info);
}

std::vector<std::vector<char>> MyActualPlayer::createBoardFromSatellite(const SatelliteView &satellite_view) const {
    std::vector board(x, std::vector<char>(y));
    for (size_t i{0}; i < x; i++) {
        for (size_t j{0}; j < y; j++) {
            board[i][j]= satellite_view.getObjectAt(i, j);
        }
    }
    return board;
}
