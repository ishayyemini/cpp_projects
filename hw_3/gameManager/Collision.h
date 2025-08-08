#ifndef COLLISION_H
#define COLLISION_H

#include <memory>
#include <vector>

#include "GameObject.h"
#include "Mine.h"
#include "Shell.h"
#include "Wall.h"

class Collision final : public GameObject {
    std::vector<std::unique_ptr<GameObject> > elements;
    std::unique_ptr<Shell> shell = nullptr;
    std::unique_ptr<Mine> mine = nullptr;
    bool marked = false;
    bool hasWeakenedWall = false;

public:
    explicit Collision(std::unique_ptr<GameObject> element1,
                       std::unique_ptr<GameObject> element2): GameObject(element1->getPosition()),
                                                              elements(std::vector<std::unique_ptr<GameObject> >(
                                                                  0)) {
        elements.push_back(std::move(element1));
        elements.push_back(std::move(element2));
    }

    std::unique_ptr<GameObject> popElement();

    char getSymbol() const override { return 'X'; }

    void addElement(std::unique_ptr<GameObject> element) { elements.push_back(std::move(element)); }

    bool checkOkCollision();

    std::unique_ptr<Shell> getShell() { return std::move(shell); }

    Shell *getShellPtr() const { return shell.get(); }

    std::unique_ptr<Mine> getMine() { return std::move(mine); }

    std::unique_ptr<Wall> getWeakenedWall();

    bool isCollision() const override { return true; }
};

#endif //COLLISION_H
