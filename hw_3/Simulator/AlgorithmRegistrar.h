#ifndef TANKS_GAME_ALGORITHMREGISTRAR_H
#define TANKS_GAME_ALGORITHMREGISTRAR_H

#include <assert.h>
#include <string>

#include "Player.h"
#include "TankAlgorithm.h"

class AlgorithmRegistrar {
    class AlgorithmAndPlayerFactories {
        std::string so_name;
        TankAlgorithmFactory tankAlgorithmFactory;
        PlayerFactory playerFactory;

    public:
        AlgorithmAndPlayerFactories(const std::string &so_name) : so_name(so_name) {
        }

        void setTankAlgorithmFactory(TankAlgorithmFactory &&factory) {
            if (tankAlgorithmFactory == nullptr)
                tankAlgorithmFactory = std::move(factory);
        }

        void setPlayerFactory(PlayerFactory &&factory) {
            if (playerFactory == nullptr)
                playerFactory = std::move(factory);
        }

        const std::string &name() const { return so_name; }

        std::unique_ptr<Player> createPlayer(int player_index, size_t x, size_t y, size_t max_steps,
                                             size_t num_shells) const {
            return playerFactory(player_index, x, y, max_steps, num_shells);
        }

        std::unique_ptr<TankAlgorithm> createTankAlgorithm(int player_index, int tank_index) const {
            return tankAlgorithmFactory(player_index, tank_index);
        }

        PlayerFactory getPlayerFactory() const { return playerFactory; }

        TankAlgorithmFactory getTankAlgorithmFactory() const { return tankAlgorithmFactory; }

        bool hasPlayerFactory() const {
            return playerFactory != nullptr;
        }

        bool hasTankAlgorithmFactory() const {
            return tankAlgorithmFactory != nullptr;
        }
    };

    std::vector<AlgorithmAndPlayerFactories> algorithms;
    static AlgorithmRegistrar registrar;

public:
    static AlgorithmRegistrar &getAlgorithmRegistrar();

    void createAlgorithmFactoryEntry(const std::string &name) {
        algorithms.emplace_back(name);
    }

    void addPlayerFactoryToLastEntry(PlayerFactory &&factory) {
        algorithms.back().setPlayerFactory(std::move(factory));
    }

    void addTankAlgorithmFactoryToLastEntry(TankAlgorithmFactory &&factory) {
        algorithms.back().setTankAlgorithmFactory(std::move(factory));
    }

    struct BadRegistrationException {
        std::string name;
        bool hasName, hasPlayerFactory, hasTankAlgorithmFactory;
    };

    void validateLastRegistration() {
        const auto &last = algorithms.back();
        bool hasName = (last.name() != "");
        if (!hasName || !last.hasPlayerFactory() || !last.hasTankAlgorithmFactory()) {
            throw BadRegistrationException{
                .name = last.name(),
                .hasName = hasName,
                .hasPlayerFactory = last.hasPlayerFactory(),
                .hasTankAlgorithmFactory = last.hasTankAlgorithmFactory()
            };
        }
    }

    auto lastReg() {
        validateLastRegistration();
        return &algorithms.back();
    }

    void removeLast() {
        algorithms.pop_back();
    }

    auto begin() const {
        return algorithms.begin();
    }

    auto end() const {
        return algorithms.end();
    }

    const AlgorithmAndPlayerFactories *find(const std::string &name) const {
        for (const auto &algorithm: algorithms) {
            if (algorithm.name() == name) {
                return &algorithm;
            }
        }
        return nullptr;
    }

    std::size_t count() const { return algorithms.size(); }
    void clear() { algorithms.clear(); }
};


#endif //TANKS_GAME_ALGORITHMREGISTRAR_H
