#include <iostream>

#include "AlgorithmRegistrar.h"
#include "Simulator.h"

int main(const int argc, char *argv[]) {
    std::string usageOrError;
    auto parsed = Simulator::parseArgs(argc, argv, usageOrError);
    if (!parsed) {
        std::cerr << usageOrError;
        return 1;
    }
    Simulator s;
    return s.run(*parsed);
}
