#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <string>

#include "GameState.h"
#include "TankAlgorithm.h"

class MyTankAlgorithm : public TankAlgorithm {
public:
    MyTankAlgorithm() = default;

    virtual std::string getName() const { return "Algorithm"; };

    virtual ActionRequest decideAction(const GameState &_) { return ActionRequest::DoNothing; }

    virtual ~MyTankAlgorithm() = default;

protected:
    ActionRequest moveIfThreatened(const GameState &state) const;
};

#endif //ALGORITHM_H
