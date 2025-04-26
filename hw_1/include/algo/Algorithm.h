#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <string>

#include "Action.h"
#include "GameState.h"

class Algorithm {
public:
    Algorithm() = default;

    virtual std::string getName() const=0;

    virtual Action decideAction(const GameState &state) const = 0;

    virtual ~Algorithm() = default;
};

#endif //ALGORITHM_H
