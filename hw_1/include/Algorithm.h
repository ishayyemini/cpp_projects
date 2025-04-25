#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <string>

#include "Action.h"
#include "GameState.h"

class Algorithm {
public:
    Algorithm() = default;

    [[nodiscard]] virtual std::string getName() const;

    [[nodiscard]] virtual Action decideAction(const GameState &state) const;

    virtual ~Algorithm() = default;
};

#endif //ALGORITHM_H
