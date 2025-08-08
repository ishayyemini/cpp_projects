#ifndef GAMEMANAGERREGISTRATION_H
#define GAMEMANAGERREGISTRATION_H

struct GameManagerRegistration {
    GameManagerRegistration (std::function<std::unique_ptr<AbstractGameManager>()>);
};

#define REGISTER_GAME_MANAGER(class_name) \
GameManagerRegistration register_me_##class_name \
( [] (bool verbose) { return std::make_unique<class_name>(verbose); } );

#endif //GAMEMANAGERREGISTRATION_H
