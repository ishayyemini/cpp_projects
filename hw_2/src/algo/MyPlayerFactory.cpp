//
// Created by Ishay Yemini on 27/05/2025.
//

#include "MyPlayerFactory.h"

#include "MyPlayer.h"

unique_ptr<Player> MyPlayerFactory::create(int player_index, size_t x, size_t y, size_t max_steps,
                                           size_t num_shells) const {
    return std::make_unique<MyPlayer>(player_index, x, y, max_steps, num_shells);
}
