#include <iostream>

#include "AlgorithmRegistrar.h"
#include "Simulator.h"

int main(const int argc, char *argv[]) {
    std::string usage_or_error;
    auto parsed = Simulator::parseArgs(argc, argv, usage_or_error);
    if (!parsed) {
        std::cerr << usage_or_error;
        return 1;
    }
    Simulator s;
    return s.run(*parsed);
}
