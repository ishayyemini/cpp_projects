#ifndef MYGAMEMANAGER_H
#define MYGAMEMANAGER_H
#include <map>
#include <fstream>

#include "Board.h"
#include "Player.h"
#include "TankAlgorithm.h"
#include "Tank.h"

namespace GameManager_322868852_340849710 {
    enum Winner {
        TIE_AMMO,
        TIE_STEPS,
        TIE,
        PLAYER_1,
        PLAYER_2,
        NO_WINNER
    };

    class GameManager {
        const PlayerFactory &player_factory;
        const TankAlgorithmFactory &tank_algorithm_factory;

    public:
        GameManager(const PlayerFactory &player_factory, const TankAlgorithmFactory &tank_algorithm_factory)
            : player_factory(player_factory), tank_algorithm_factory(tank_algorithm_factory),
              satellite_view(MySatelliteView()) {
        }

        void readBoard(const std::string &file_name);

        void run();

        void updateCounters(Tank &tank, ActionRequest action);

        void setVisual(bool visual) { this->visual = visual; }

        // Add visualization output capabilities
        void enableGameStateExport(const std::string &output_file) {
            export_game_state = true;
            game_state_file.open(output_file);
        }

    private:
        static constexpr int max_steps_empty_ammo = 40;

        bool visual = false;
        bool export_game_state = false;
        std::ofstream game_state_file;
        size_t game_step = 0;
        bool game_over = false;
        Winner winner = NO_WINNER;
        std::unique_ptr<Board> board;
        int empty_countdown = -1;
        std::vector<std::tuple<bool, ActionRequest, bool, bool> > tank_status;
        std::vector<std::unique_ptr<Player> > players;
        std::vector<std::unique_ptr<TankAlgorithm> > tanks;
        MySatelliteView satellite_view;

        bool tankAction(Tank &tank, ActionRequest action);

        void checkDeaths();

        bool moveForward(Tank &tank);

        bool moveBackward(Tank &tank);

        bool rotate(Tank &tank, int turn);

        bool shoot(Tank &tank);

        bool getBattleInfo(const Tank &tank, size_t player_i);

        void updateSatelliteView();

        bool allEmptyAmmo() const;

        void tanksTurn();

        void shellsTurn() const;

        void processStep();

        bool isGameOver() const { return game_over; }

        std::string getGameResult() const;

        void logStep();

        // New method to export game state data for visualization
        void exportGameState();
    };
}

#endif //MYGAMEMANAGER_H
