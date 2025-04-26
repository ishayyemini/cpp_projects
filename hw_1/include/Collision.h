#ifndef COLLISION_H
#define COLLISION_H

#include "GameObject.h"

class Collision final : public GameObject {
    std::vector<std::unique_ptr<GameObject> > elements;

public:
    explicit Collision(std::unique_ptr<GameObject> element1,
                       std::unique_ptr<GameObject> element2): GameObject(element1->getPosition()),
                                                              elements(std::vector<std::unique_ptr<GameObject> >(
                                                                  0)) {
        elements.push_back(std::move(element1));
        elements.push_back(std::move(element2));
    }

    void addElement(std::unique_ptr<GameObject> element) { elements.push_back(std::move(element)); }
};

#endif //COLLISION_H
