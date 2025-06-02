#include "MyTankAlgorithmFactory.h"

#include "PathfindingAlgorithm.h"
#include "SimpleAlgorithm.h"

unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const {
    (void) tank_index;//todo: what is this for? maybe in algo we can keep track of tank_index?

    if (player_index % 2 == 0) {
        return std::make_unique<SimpleAlgorithm>();
    }

    return std::make_unique<PathfindingAlgorithm>();
}
