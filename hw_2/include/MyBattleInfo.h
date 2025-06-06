#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>
#include "BattleInfo.h"

class MyBattleInfo final : public BattleInfo {
public:
    std::vector<std::vector<char> > board{};
    size_t max_steps;
    size_t num_shells;

    explicit MyBattleInfo(const std::vector<std::vector<char> > &board, const size_t max_steps, const size_t num_shells): board(board),
        max_steps(max_steps),
        num_shells(num_shells) {
    }
};

#endif //GAMESTATE_H
