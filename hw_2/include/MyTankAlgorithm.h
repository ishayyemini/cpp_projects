#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <string>

#include "MyBattleStatus.h"
#include "TankAlgorithm.h"

class MyTankAlgorithm : public TankAlgorithm {
public:
    virtual std::string getName() const { return "Algorithm"; }

    void updateBattleInfo(BattleInfo &info) override;

    virtual ~MyTankAlgorithm() = default;

protected:
    int player_id{0};
    int tank_index = {0};
    int battle_info_update{0};
    MyBattleStatus battle_status;


    MyTankAlgorithm(int player_id, int tank_index);

    ActionRequest getAction() override;

    virtual void calculateAction(ActionRequest *request, std::string *request_title) = 0;

    void printLogs(const std::string &msg) const;

    bool isTankThreatened() const;

    ActionRequest moveIfThreatened() const;




};

#endif //ALGORITHM_H
