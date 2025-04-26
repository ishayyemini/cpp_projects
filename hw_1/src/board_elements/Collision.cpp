#include "../../include/Collision.h"

#include "Tank.h"
#include "Wall.h"

std::unique_ptr<GameObject> Collision::popElement() {
    if (elements.size() == 0) return nullptr;
    std::unique_ptr<GameObject> popped = std::move(elements.front());
    elements.erase(elements.begin());
    return popped;
}

bool Collision::checkOkCollision() {
    if (marked) {
        return shell != nullptr && mine != nullptr;
    }

    marked = true;

    for (auto it = elements.begin(); it != elements.end();) {
        if (auto *shellPtr = dynamic_cast<Shell *>(it->get())) {
            setDirection(shellPtr->getDirection());
            shell.reset(shellPtr);
            it->release();
            it = elements.erase(it);
        } else if (auto *minePtr = dynamic_cast<Mine *>(it->get())) {
            mine.reset(minePtr);
            it->release();
            it = elements.erase(it);
        } else {
            ++it;
        }
    }

    if (elements.size() == 0 && shell != nullptr && mine != nullptr) {
        return true;
    }
    if (mine != nullptr) elements.push_back(std::move(mine));
    if (shell != nullptr) elements.push_back(std::move(shell));

    for (auto it = elements.begin(); it != elements.end();) {
        if (auto *wallPtr = dynamic_cast<Wall *>(it->get())) {
            wallPtr->takeDamage();
        } else {
            it->get()->destroy();
        }
        ++it;
    }

    return false;
}
