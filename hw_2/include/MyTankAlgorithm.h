#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <string>

#include "MyBattleInfo.h"
#include "TankAlgorithm.h"

class MyTankAlgorithm : public TankAlgorithm {
public:
    virtual std::string getName() const { return "Algorithm"; }

    void updateBattleInfo(BattleInfo &info) override {
        (void) info;
    }

    virtual ~MyTankAlgorithm() = default;

protected:
    ActionRequest moveIfThreatened() const;

    int turn_number = 0;

    std::unique_ptr<MyBattleInfo> state;
};

#endif //ALGORITHM_H
