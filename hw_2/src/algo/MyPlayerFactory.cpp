#include "MyPlayerFactory.h"

#include "Player1.h"
#include "Player2.h"

unique_ptr<Player> MyPlayerFactory::create(int player_index, size_t x, size_t y, size_t max_steps,
                                           size_t num_shells) const {
    if (player_index % 2 == 0) {
        return std::make_unique<Player2>(player_index, x, y, max_steps, num_shells);
    }
    return std::make_unique<Player1>(player_index, x, y, max_steps, num_shells);
}
