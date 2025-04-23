#ifndef ALGO_H
#define ALGO_H

#include "game/action.h"

class Algorithm {
public:
    virtual ~Algorithm() = default;

private:
    virtual Action getNextAction() { return NONE; }
};


#endif //ALGO_H
