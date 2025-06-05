#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>
#include "BattleInfo.h"

class MyBattleInfo final : public BattleInfo {
public:
    std::vector<std::vector<char> > board{};
    int max_steps;;
    int num_shells;

    explicit MyBattleInfo(const std::vector<std::vector<char> > &board, const int max_steps, const int num_shells): board(board),
        max_steps(max_steps),
        num_shells(num_shells) {
    }
};

#endif //GAMESTATE_H
