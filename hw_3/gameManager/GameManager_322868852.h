#ifndef MYGAMEMANAGER_H
#define MYGAMEMANAGER_H
#include <map>
#include <fstream>

#include "Board.h"
#include "Player.h"
#include "TankAlgorithm.h"
#include "Tank.h"
#include "AbstractGameManager.h"

namespace GameManager_322868852_340849710 {
    enum Winner {
        TIE_AMMO,
        TIE_STEPS,
        TIE,
        PLAYER_1,
        PLAYER_2,
        NO_WINNER
    };

    class GameManager_322868852 : public AbstractGameManager {
        // TODO don't ignore the verbose!
        bool verbose;

    public:
        GameManager_322868852(const bool verbose)
            : verbose(verbose), satellite_view(MySatelliteView()) {
        }

        void initBoard(size_t map_width, size_t map_height,
                       const SatelliteView &map,
                       const std::string &map_name,
                       size_t max_steps, size_t num_shells);

        GameResult run(size_t map_width, size_t map_height,
                       const SatelliteView &map, // <= a snapshot, NOT updated
                       std::string map_name,
                       size_t max_steps, size_t num_shells,
                       Player &player1, std::string name1, Player &player2, std::string name2,
                       TankAlgorithmFactory player1_tank_algo_factory,
                       TankAlgorithmFactory player2_tank_algo_factory) override;

        void updateCounters(Tank &tank, ActionRequest action);

        void setVisual(bool visual) { this->visual = visual; }

        // Add visualization output capabilities
        void enableGameStateExport(const std::string &output_file) {
            export_game_state = true;
            game_state_file.open(output_file);
        }

    private:
        static constexpr int max_steps_empty_ammo = 40;

        bool visual = true; // TODO change!
        bool export_game_state = false;
        std::ofstream game_state_file;
        size_t game_step = 0;
        bool game_over = false;
        Winner winner = NO_WINNER;
        std::unique_ptr<Board> board;
        int empty_countdown = -1;
        std::vector<std::tuple<bool, ActionRequest, bool, bool> > tank_status;
        std::vector<std::string> player_names;
        std::vector<Player *> players;
        std::vector<TankAlgorithmFactory> ta_factories;
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

        std::string getGameResult(GameResult &result) const;

        void logStep();

        // New method to export game state data for visualization
        void exportGameState();
    };
}

#endif //MYGAMEMANAGER_H
