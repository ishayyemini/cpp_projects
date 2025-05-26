#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <string>

#include "MyBattleInfo.h"
#include "TankAlgorithm.h"

class MyTankAlgorithm : public TankAlgorithm {
public:
    MyTankAlgorithm() = default;

    virtual std::string getName() const { return "Algorithm"; }

    virtual ~MyTankAlgorithm() = default;

protected:
    ActionRequest moveIfThreatened(const MyBattleInfo &state) const;

    int turn_number = 0;

    MyBattleInfo state;
};

#endif //ALGORITHM_H
