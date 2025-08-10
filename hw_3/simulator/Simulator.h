#ifndef TANKS_GAME_SIMULATOR_H
#define TANKS_GAME_SIMULATOR_H

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Use the project's common API
#include "AbstractGameManager.h"
#include "AlgorithmRegistrar.h"
#include "GameManagerRegistrar.h"
#include "InputParser.h"
#include "Logger.h"
#include "SatelliteView.h"
#include "TankAlgorithm.h"

struct Args {
    enum class Mode { Comparative, Competition };

    Mode mode;

    // comparative
    std::string gameMapFile;
    std::string gameManagersFolder;
    std::string algorithm1File;
    std::string algorithm2File;

    // competition
    std::string gameMapsFolder;
    std::string gameManagerFile;
    std::string algorithmsFolder;

    // common
    int numThreads = 1;
    bool verbose = false;
};

class Simulator {
public:
    Simulator() = default;

    int run(const Args &args);

    // CLI parsing and usage
    static std::optional<Args> parseArgs(int argc, char **argv, std::string &usageOrError);

    static std::string usageComparative();

    static std::string usageCompetition();

    static std::optional<Args> handleUnknownArgs(const std::vector<std::string> &unknown, std::string &usageOrError);

    static bool checkComparativeArgsErrors(const Args &args, std::string &usageOrError);

    static bool checkCompetitionArgsErrors(const Args &args, std::string &usageOrError);

    static bool checkModeErrors(const Args &args, std::string &usageOrError);

    static std::optional<Args::Mode> sortArg(const std::string &a, bool modeSet, Args &args, std::string &usageOrError,
                                             std::vector<std::string> &unknown,
                                             const std::unordered_set<std::string> &supported);

private:
    // Core runners
    int runComparative(const Args &args);

    int runCompetition(const Args &args);

    // Filesystem helpers
    static bool fileExistsReadable(const std::string &path);

    static bool dirExistsListable(const std::string &path);

    static std::vector<std::string> listRegularFiles(const std::string &dir);

    static std::vector<std::string> listFilesWithExtension(const std::string &dir, const std::string &ext);

    static std::string joinPath(const std::string &a, const std::string &b);

    // Dynamic load helpers
    static void *dlopenOrNull(const std::string &path);

    // Registrar-based loading
    struct AlgWrap {
        void *dl;
        bool has_dl;
        std::string name;
        PlayerFactory playerFactory;
        TankAlgorithmFactory tankFactory;
    };

    static void closeAlgWrap(std::optional<AlgWrap> &wrap);

    static void closeAlgWrap(AlgWrap &wrap);

    static void closeAlgWrap(std::vector<AlgWrap> &handles);

    // Loads a single algorithm .so into AlgorithmRegistrar. Returns nullopt on failure.
    static std::optional<AlgWrap> loadAlgorithmSo(const std::string &soPath);

    // Loads many algorithm .so files, returns loaded algorithms and their dls.
    static std::vector<Simulator::AlgWrap> loadAlgorithmFolder(const std::string &folder);

    struct GmWrap {
        void *dl;
        bool has_dl = false;
        std::string name;
        GameManagerFactory makeGameManager;
    };

    static void closeGmWrap(std::optional<GmWrap> &wrap);

    static void closeGmWrap(GmWrap &wrap);

    static void closeGmWrap(std::vector<GmWrap> &handles);

    static void cleanComparative(std::optional<AlgWrap> &alg1, std::optional<AlgWrap> &alg2, std::vector<GmWrap> &gms);

    static int loadComparative(const Args &args, std::optional<AlgWrap> &alg1, std::optional<AlgWrap> &alg2,
                               std::vector<GmWrap> &gms, std::unique_ptr<SatelliteView> &mapView, size_t &width,
                               size_t &height,
                               size_t &maxSteps,
                               size_t &numShells, std::string &mapName);


    static void cleanCompetition(std::optional<GmWrap> &gmWrap, std::vector<AlgWrap> &algs);

    static int loadCompetition(const Args &args, std::optional<GmWrap> &gmWrap, std::vector<AlgWrap> &algs,
                               std::vector<std::string> &mapFiles);

    static void runOneCompetition(const Args &args, std::optional<GmWrap> &gmWrap, std::vector<AlgWrap> &algs,
                                  const std::string &mapPath, size_t ai, size_t aj, std::mutex &mtx,
                                  std::vector<int> &scores);

    static void
    buildSortedScoreboard(std::vector<AlgWrap> &algs, std::vector<int> &scores,
                          std::vector<std::pair<std::string, int> > &out);

    // Loads a single game manager .so into GameManagerRegistrar. Returns nullopt on failure.
    static std::optional<GmWrap> loadGameManagerSo(const std::string &soPath);

    // Loads many GMs from a folder
    static std::vector<GmWrap> loadGameManagersFolder(
        const std::string &folder);

    // Thread pool (created only when numThreads >= 2; workers = numThreads; main waits)
    class ThreadPool {
    public:
        explicit ThreadPool(size_t nThreads);

        ~ThreadPool();

        void enqueue(std::function<void()> task);

        void waitIdle();

        // TODO rule of 5

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

    // Utilities
    static std::string basenameNoExt(const std::string &path);

    static std::string epochTimeId();

    static bool writeTextFile(const std::string &path, const std::string &content, std::string &err);

    // Comparative result key (group identical results)
    struct ComparativeKey {
        int winner; // 0 tie, 1,2
        GameResult::Reason reason;
        size_t rounds;
        std::vector<std::string> finalMapDump; // lines of the final game state
    };

    struct ComparativeKeyHash {
        size_t operator()(const ComparativeKey &k) const noexcept;
    };

    struct ComparativeKeyEq {
        bool operator()(const ComparativeKey &a, const ComparativeKey &b) const noexcept;
    };

    // Pairing for competition (dedup unordered pairs only for the even-N special case)
    static std::vector<std::pair<size_t, size_t> > computePairsForK(size_t N, size_t k, bool dedupUnordered);

    struct OneRes {
        std::string gmName;
        ComparativeKey key;
    };

    static void runOneComparative(const GmWrap &gw, const Args &args, const std::optional<AlgWrap> &alg1,
                                  const std::optional<AlgWrap> &alg2, const std::unique_ptr<SatelliteView> &mapView,
                                  const size_t &width, const size_t &height, const size_t &maxSteps,
                                  const size_t &numShells,
                                  const std::string &mapName, std::mutex &mtx, std::vector<OneRes> &results);

    static std::vector<std::pair<std::vector<std::string>, ComparativeKey> > groupResComparative(
        const std::vector<OneRes> &results);

    // Output
    static std::string formatComparativeOutput(
        const std::string &gameMap,
        const std::string &alg1,
        const std::string &alg2,
        const std::vector<std::pair<std::vector<std::string>, ComparativeKey> > &grouped);

    static std::string formatCompetitionOutput(
        const std::string &gameMapsFolder,
        const std::string &gameManagerSo,
        const std::vector<std::pair<std::string, int> > &scoresSorted);
};

#endif //TANKS_GAME_SIMULATOR_H
