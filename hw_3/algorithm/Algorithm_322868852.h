#ifndef PATHFINDINGALGORITHM_H
#define PATHFINDINGALGORITHM_H

#include "Direction.h"
#include "MyBattleStatus.h"
#include "TankAlgorithm.h"

namespace Algorithm_322868852_340849710 {
    class Algorithm_322868852 : public TankAlgorithm {
        int player_id{0};
        int tank_index = {0};
        MyBattleStatus battle_status;

    public:
        explicit Algorithm_322868852(int player_id, int tank_index) : player_id{player_id},
                                                                      tank_index(tank_index),
                                                                      battle_status(
                                                                          MyBattleStatus(player_id, tank_index)) {
            initLatestEnemyPosition();
        }

        std::string getName() const { return "Algorithm_322868852"; }

        void calculateAction(ActionRequest *request, std::string *request_title);

        void updateBattleInfo(BattleInfo &info);

        void printLogs(const std::string &msg) const;

        ActionRequest getAction();

        bool isTankThreatened() const;

        ActionRequest moveIfThreatened() const;

    private:
        bool was_threatened{false};
        std::vector<Position> last_enemy_positions = {};

        void initLatestEnemyPosition();

        void updateLatestEnemyPosition();

        void handleTankThreatened(ActionRequest *request, std::string *request_title);

        void tryShootEnemy(ActionRequest *request, std::string *request_title);

        bool rotateToEnemy(ActionRequest *request, std::string *request_title) const;

        void updatePathIfNeeded();

        void handleEmptyPath(ActionRequest *request, std::string *request_title) const;

        void followPathOrRotate(ActionRequest *request, std::string *request_title);

        std::vector<Direction::DirectionType> computeBFS();

        bool hasEnemyMoved() const;
    };
}

#endif //PATHFINDINGALGORITHM_H
