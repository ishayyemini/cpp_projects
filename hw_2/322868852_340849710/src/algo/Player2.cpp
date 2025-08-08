#include "Player2.h"

#include "MyBattleInfo.h"

void Player2::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) {
    auto battle_info = MyBattleInfo(createPartialBoard(satellite_view), player_index, max_steps, num_shells);
    tank.updateBattleInfo(battle_info);
}

std::vector<std::vector<char> > Player2::createPartialBoard(const SatelliteView &satellite_view) const {
    std::vector board(x, std::vector(y, ' '));
    std::pair<int, int> tank;

    for (size_t i{0}; i < x; i++) {
        for (size_t j{0}; j < y; j++) {
            if (satellite_view.getObjectAt(i, j) == '%') tank = {i, j};
        }
    }

    if (!tank.first || !tank.second) return board;

    for (int i = std::max(0, tank.first - 6); i < std::min(static_cast<int>(x), tank.first + 6); i++) {
        for (int j = std::max(0, tank.second - 6); j < std::min(static_cast<int>(y), tank.second + 6); j++) {
            board[i][j] = satellite_view.getObjectAt(i, j);
        }
    }

    return board;
}
