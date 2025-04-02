#ifndef ALGO_H
#define ALGO_H

#include "game/action.h"

class Algorithm {
    virtual Action getNextAction();

    virtual ~Algorithm() = default;
};


#endif //ALGO_H
