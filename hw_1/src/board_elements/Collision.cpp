#include "../../include/Collision.h"

bool Collision::checkOkCollision() {
    if (elements.size() != 2) {
        return shell != nullptr && mine != nullptr;
    }

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

    return elements.size() == 0 && shell != nullptr && mine != nullptr;
}
