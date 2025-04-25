#ifndef SIMPLEALGORITHM_H
#define SIMPLEALGORITHM_H

#include "Algorithm.h"

class SimpleAlgorithm final : public Algorithm {
public:
    SimpleAlgorithm();

    ~SimpleAlgorithm() override;

    [[nodiscard]] std::string getName() const override { return "SimpleAlgorithm"; }
};

#endif //SIMPLEALGORITHM_H
