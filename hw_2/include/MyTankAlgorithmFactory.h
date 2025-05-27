//
// Created by Ishay Yemini on 27/05/2025.
//

#ifndef MYTANKALGORITHMFACTORY_H
#define MYTANKALGORITHMFACTORY_H
#include <list>

#include "TankAlgorithmFactory.h"

class MyTankAlgorithmFactory final : public TankAlgorithmFactory {
    std::list<int> tanks_indices;

public:
    unique_ptr<TankAlgorithm> create(
        int player_index, int tank_index) const override;
};

#endif //MYTANKALGORITHMFACTORY_H
