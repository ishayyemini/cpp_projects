#ifndef MYTANKALGORITHMFACTORY_H
#define MYTANKALGORITHMFACTORY_H

#include "TankAlgorithm.h"

class MyTankAlgorithmFactory final : public TankAlgorithmFactory {
public:
    unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override;
};

#endif //MYTANKALGORITHMFACTORY_H
