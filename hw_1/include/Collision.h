#ifndef COLLISION_H
#define COLLISION_H

#include "GameObject.h"
#include "Mine.h"
#include "Shell.h"

class Collision final : public GameObject {
    std::vector<std::unique_ptr<GameObject> > elements;
    std::unique_ptr<Shell> shell = nullptr;
    std::unique_ptr<Mine> mine = nullptr;

public:
    explicit Collision(std::unique_ptr<GameObject> element1,
                       std::unique_ptr<GameObject> element2): GameObject(element1->getPosition()),
                                                              elements(std::vector<std::unique_ptr<GameObject> >(
                                                                  0)) {
        elements.push_back(std::move(element1));
        elements.push_back(std::move(element2));
    }

    std::vector<std::unique_ptr<GameObject> > &getElements() { return elements; }

    void addElement(std::unique_ptr<GameObject> element) { elements.push_back(std::move(element)); }

    bool checkOkCollision();

    std::unique_ptr<Shell> getShell() { return std::move(shell); }

    std::unique_ptr<Mine> getMine() { return std::move(mine); }
};

#endif //COLLISION_H
