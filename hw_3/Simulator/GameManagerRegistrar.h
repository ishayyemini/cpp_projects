#ifndef TANKS_GAME_GAMEMANAGERREGISTRAR_H
#define TANKS_GAME_GAMEMANAGERREGISTRAR_H

#include <string>

#include "AbstractGameManager.h"

class GameManagerRegistrar {
    class GameManagerFactories {
        std::string so_name;
        GameManagerFactory game_manager_factory;

    public:
        GameManagerFactories(const std::string &so_name) : so_name(so_name) {
        }

        void setGameManagerFactory(GameManagerFactory &&factory) {
            if (game_manager_factory == nullptr)
                game_manager_factory = std::move(factory);
        }

        const std::string &name() const { return so_name; }

        std::unique_ptr<AbstractGameManager> createGameManager(bool verbose) const {
            return game_manager_factory(verbose);
        }

        GameManagerFactory getGameManagerFactory() const { return game_manager_factory; }

        bool hasGameManagerFactory() const {
            return game_manager_factory != nullptr;
        }
    };

    std::vector<GameManagerFactories> game_managers;
    static GameManagerRegistrar registrar;

public:
    static GameManagerRegistrar &getGameManagerRegistrar();

    void createGameManagerFactoryEntry(const std::string &name) {
        game_managers.emplace_back(name);
    }

    void addGameManagerFactoryToLastEntry(GameManagerFactory &&factory) {
        game_managers.back().setGameManagerFactory(std::move(factory));
    }

    struct BadRegistrationException {
        std::string name;
        bool hasName, hasGameManagerFactory;
    };

    void validateLastRegistration() {
        const auto &last = game_managers.back();
        bool hasName = (last.name() != "");
        if (!hasName || !last.hasGameManagerFactory()) {
            throw BadRegistrationException{
                .name = last.name(),
                .hasName = hasName,
                .hasGameManagerFactory = last.hasGameManagerFactory()
            };
        }
    }

    void removeLast() {
        game_managers.pop_back();
    }

    auto lastReg() {
        validateLastRegistration();
        return &game_managers.back();
    }

    auto begin() const {
        return game_managers.begin();
    }

    auto end() const {
        return game_managers.end();
    }

    const GameManagerFactories *find(const std::string &name) const {
        for (const auto &gm: game_managers) {
            if (gm.name() == name) {
                return &gm;
            }
        }
        return nullptr;
    }

    std::size_t count() const { return game_managers.size(); }
    void clear() { game_managers.clear(); }
};


#endif //TANKS_GAME_GAMEMANAGERREGISTRAR_H
