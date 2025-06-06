#ifndef MYGAMEMANAGER_H
#define MYGAMEMANAGER_H
#include <map>

#include "Board.h"
#include "GameManager.h"
#include "MyPlayerFactory.h"
#include "MyTankAlgorithmFactory.h"
#include "Tank.h"

enum Winner {
    TIE_AMMO,
    TIE_STEPS,
    TIE,
    PLAYER_1,
    PLAYER_2,
    NO_WINNER
};

class MyGameManager : public GameManager {
public:
    MyGameManager(const PlayerFactory &player_factory, const TankAlgorithmFactory &tank_algorithm_factory): GameManager(
        player_factory, tank_algorithm_factory) {
    }

    void readBoard(const std::string &file_name);

    void run();

    void setVisual(bool visual) { this->visual = visual; }

private:
    static constexpr int max_steps_empty_ammo = 40; //todo: who decided this?

    bool visual = false;
    size_t game_step = 0;
    bool game_over = false;
    Winner winner = NO_WINNER;
    std::unique_ptr<Board> board;
    int empty_countdown = -1;
    //todo: what tank_status keep track of?
    std::vector<std::tuple<bool, ActionRequest, bool, bool> > tank_status;
    std::vector<std::unique_ptr<Player> > players;
    std::vector<std::unique_ptr<TankAlgorithm> > tanks;

    bool tankAction(Tank &tank, ActionRequest action);

    void checkDeaths();

    bool moveForward(Tank &tank);

    bool moveBackward(Tank &tank);

    bool rotate(Tank &tank, int turn);

    bool shoot(Tank &tank);

    bool getBattleInfo(const Tank &tank, size_t player_i);

    bool allEmptyAmmo() const;

    void tanksTurn();

    void shellsTurn() const;

    void processStep();

    bool isGameOver() const { return game_over; }

    std::string getGameResult() const;

    void logStep();
};

#endif //MYGAMEMANAGER_H
