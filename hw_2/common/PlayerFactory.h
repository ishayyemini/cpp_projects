#ifndef PLAYERFACTORY_H
#define PLAYERFACTORY_H

#include <memory>

#include "Player.h"

using namespace std;

class PlayerFactory {
public:
    virtual ~PlayerFactory() {}

    virtual unique_ptr<Player> create(int player_index, size_t x, size_t y,
                                      size_t max_steps, size_t num_shells) const = 0;
};

#endif //PLAYERFACTORY_H
