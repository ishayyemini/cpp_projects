#ifndef SIMPLEALGORITHM_H
#define SIMPLEALGORITHM_H

#include "MyTankAlgorithm.h"

class SimpleAlgorithm : public MyTankAlgorithm {
public:
    explicit SimpleAlgorithm(int player_id, int tank_index) : MyTankAlgorithm(player_id, tank_index) {}

    ~SimpleAlgorithm() override = default;

    std::string getName() const override { return "SimpleAlgorithm"; }

    void calculateAction(ActionRequest *request, std::string *request_title) override;


private:
    bool was_threatened = false;
};

#endif //SIMPLEALGORITHM_H
