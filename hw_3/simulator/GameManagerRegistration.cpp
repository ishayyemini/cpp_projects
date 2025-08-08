#include "GameManagerRegistration.h"

#include "GameManagerRegistrar.h"

GameManagerRegistration::GameManagerRegistration(GameManagerFactory factory) {
    auto &regsitrar = GameManagerRegistrar::getGameManagerRegistrar();
    regsitrar.addGameManagerFactoryToLastEntry(std::move(factory));
}
