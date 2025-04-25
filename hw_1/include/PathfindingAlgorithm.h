#ifndef PATHFINDINGALGORITHM_H
#define PATHFINDINGALGORITHM_H

class PathfindingAlgorithm final : public Algorithm {
public:
    PathfindingAlgorithm();

    ~PathfindingAlgorithm() override;

    [[nodiscard]] std::string getName() const override { return "PathfindingAlgorithm"; }
};


#endif //PATHFINDINGALGORITHM_H
