#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>
#include "BattleInfo.h"
#include "Logger.h"

class MyBattleInfo final : public BattleInfo {
    std::vector<std::vector<char> > board{};
    size_t max_steps;
    size_t num_shells;

public:
    explicit MyBattleInfo(const std::vector<std::vector<char> > &board, const int player_id, size_t max_steps,
                          const size_t num_shells): board(board), max_steps(max_steps), num_shells(num_shells) {
        Logger::getInstance().log("Player_id: " + std::to_string(player_id));
    }

    const std::vector<std::vector<char> > &getBoard() const { return board; }

    size_t getMaxSteps() const { return max_steps; }

    size_t getNumShells() const { return num_shells; }
};

#endif //GAMESTATE_H
