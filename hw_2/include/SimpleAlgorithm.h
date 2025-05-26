#ifndef SIMPLEALGORITHM_H
#define SIMPLEALGORITHM_H

#include "MyTankAlgorithm.h"

class SimpleAlgorithm : public MyTankAlgorithm {
public:
    SimpleAlgorithm() = default;

    ~SimpleAlgorithm() override = default;

    std::string getName() const override { return "SimpleAlgorithm"; }

    ActionRequest getAction() override;
};

#endif //SIMPLEALGORITHM_H
