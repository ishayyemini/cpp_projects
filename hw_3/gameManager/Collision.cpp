#include "Collision.h"

#include "Tank.h"
#include "Wall.h"

std::unique_ptr<GameObject> Collision::popElement() {
    if (elements.empty()) return nullptr;
    std::unique_ptr<GameObject> popped = std::move(elements.front());
    elements.erase(elements.begin());
    return popped;
}

bool Collision::checkOkCollision() {
    if (marked) return shell != nullptr && mine != nullptr;
    marked = true;

    for (auto it = elements.begin(); it != elements.end();) {
        if (auto *shellPtr = dynamic_cast<Shell *>(it->get())) {
            if (shell != nullptr) break;
            setDirection(shellPtr->getDirection());
            shell.reset(shellPtr);
            it->release();
            it = elements.erase(it);
        } else if (auto *minePtr = dynamic_cast<Mine *>(it->get())) {
            if (mine != nullptr) break;
            mine.reset(minePtr);
            it->release();
            it = elements.erase(it);
        } else ++it;
    }

    if (elements.size() == 0 && shell != nullptr && mine != nullptr) {
        mine->setPosition(getPosition());
        shell->setPosition(getPosition());
        return true;
    }
    if (mine != nullptr) elements.push_back(std::move(mine));
    if (shell != nullptr) elements.push_back(std::move(shell));

    for (auto it = elements.begin(); it != elements.end();) {
        if (auto *wallPtr = dynamic_cast<Wall *>(it->get())) {
            wallPtr->takeDamage();
            if (!wallPtr->isDestroyed()) hasWeakenedWall = true;
            else hasWeakenedWall = false;
        } else it->get()->destroy();
        ++it;
    }

    return false;
}

std::unique_ptr<Wall> Collision::getWeakenedWall() {
    if (!hasWeakenedWall) return nullptr;

    std::unique_ptr<Wall> weakenedWall = nullptr;

    for (auto it = elements.begin(); it != elements.end();) {
        if (auto *wallPtr = dynamic_cast<Wall *>(it->get())) {
            setDirection(wallPtr->getDirection());
            weakenedWall.reset(wallPtr);
            it->release();
            return weakenedWall;
        }
        ++it;
    }

    return weakenedWall;
}