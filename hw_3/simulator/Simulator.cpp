#include "Simulator.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <thread>
#include <utility>
#include <condition_variable>

#include <dlfcn.h>

using namespace UserCommon_322868852_340849710;

namespace fs = std::filesystem;

// Guard all Logger usage and any code paths that might write via Logger
namespace {
    std::mutex g_logger_mutex;
}

// -------------------- Utils --------------------
static inline std::string trim(const std::string &s) {
    size_t i = 0, j = s.size();
    while (i < j && std::isspace((unsigned char) s[i])) ++i;
    while (j > i && std::isspace((unsigned char) s[j - 1])) --j;
    return s.substr(i, j - i);
}

std::string Simulator::basenameNoExt(const std::string &path) {
    fs::path p(path);
    return p.stem().string();
}

std::string Simulator::epochTimeId() {
    using namespace std::chrono;
    auto now = time_point_cast<microseconds>(system_clock::now()).time_since_epoch().count();
    return std::to_string(now);
}

bool Simulator::writeTextFile(const std::string &path, const std::string &content, std::string &err) {
    try {
        std::ofstream out(path);
        if (!out) {
            err = "Cannot open file for writing: " + path;
            return false;
        }
        out << content;
        if (!out.good()) {
            err = "Write failed: " + path;
            return false;
        }
        return true;
    } catch (const std::exception &e) {
        err = e.what();
        return false;
    }
}

std::string Simulator::joinPath(const std::string &a, const std::string &b) {
    return (fs::path(a) / fs::path(b)).string();
}

// -------------------- Filesystem --------------------
bool Simulator::fileExistsReadable(const std::string &path) {
    std::error_code ec;
    return fs::exists(path, ec) && fs::is_regular_file(path, ec);
}

bool Simulator::dirExistsListable(const std::string &path) {
    std::error_code ec;
    return fs::exists(path, ec) && fs::is_directory(path, ec);
}

std::vector<std::string> Simulator::listRegularFiles(const std::string &dir) {
    std::vector<std::string> out;
    std::error_code ec;
    for (auto &de: fs::directory_iterator(dir, ec)) {
        if (ec) break;
        if (de.is_regular_file()) out.push_back(de.path().string());
    }
    std::sort(out.begin(), out.end());
    return out;
}

std::vector<std::string> Simulator::listFilesWithExtension(const std::string &dir, const std::string &ext) {
    std::vector<std::string> out;
    std::error_code ec;
    for (auto &de: fs::directory_iterator(dir, ec)) {
        if (ec) break;
        if (de.is_regular_file() && de.path().extension() == ext) out.push_back(de.path().string());
    }
    std::sort(out.begin(), out.end());
    return out;
}

// -------------------- Dynamic loading helpers --------------------
void *Simulator::dlopenOrNull(const std::string &path) {
    void *h = dlopen(path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!h) {
        std::cerr << "Failed to load library '" << path << "': " << dlerror() << "\n";
        return nullptr;
    }
    return h;
}

void Simulator::closeAlgWrap(AlgWrap &wrap) {
    wrap.playerFactory = nullptr;
    wrap.tankFactory = nullptr;
    dlclose(wrap.dl);
}

void Simulator::closeAlgWrap(std::optional<AlgWrap> &wrap) {
    if (wrap) closeAlgWrap(*wrap);
}

void Simulator::closeGmWrap(GmWrap &wrap) {
    wrap.makeGameManager = nullptr;
    dlclose(wrap.dl);
}

void Simulator::closeGmWrap(std::optional<GmWrap> &wrap) {
    if (wrap) closeGmWrap(*wrap);
}

void Simulator::closeAlgWrap(std::vector<AlgWrap> &handles) {
    for (AlgWrap &wrap: handles) closeAlgWrap(wrap);
    handles.clear();
}

void Simulator::closeGmWrap(std::vector<GmWrap> &handles) {
    for (GmWrap &wrap: handles) closeGmWrap(wrap);
    handles.clear();
}

// -------------------- ThreadPool --------------------
struct Simulator::ThreadPool::Impl {
    std::vector<std::thread> workers;
    std::queue<std::function<void()> > q;
    std::mutex m;
    std::condition_variable cv;
    bool stop = false;
    size_t active = 0;
};

Simulator::ThreadPool::ThreadPool(size_t nThreads) : impl_(std::make_unique<Impl>()) {
    impl_->workers.reserve(nThreads);
    for (size_t i = 0; i < nThreads; i++) {
        impl_->workers.emplace_back([this] {
            for (;;) {
                std::function<void()> task; {
                    std::unique_lock<std::mutex> lk(impl_->m);
                    impl_->cv.wait(lk, [this] { return impl_->stop || !impl_->q.empty(); });
                    if (impl_->stop && impl_->q.empty()) return;
                    task = std::move(impl_->q.front());
                    impl_->q.pop();
                    impl_->active++;
                }
                try { task(); } catch (...) {
                } {
                    std::lock_guard<std::mutex> lk(impl_->m);
                    impl_->active--;
                }
                impl_->cv.notify_all();
            }
        });
    }
}

Simulator::ThreadPool::~ThreadPool() { {
        std::lock_guard<std::mutex> lk(impl_->m);
        impl_->stop = true;
    }
    impl_->cv.notify_all();
    for (auto &t: impl_->workers) if (t.joinable()) t.join();
}

void Simulator::ThreadPool::enqueue(std::function<void()> task) { {
        std::lock_guard<std::mutex> lk(impl_->m);
        impl_->q.push(std::move(task));
    }
    impl_->cv.notify_one();
}

void Simulator::ThreadPool::waitIdle() {
    std::unique_lock<std::mutex> lk(impl_->m);
    impl_->cv.wait(lk, [this] { return impl_->q.empty() && impl_->active == 0; });
}

// -------------------- Args parsing & usage --------------------
static bool parseKV(const std::string &s, std::string &key, std::string &val) {
    auto pos = s.find('=');
    if (pos == std::string::npos) return false;
    std::string lhs = s.substr(0, pos);
    std::string rhs = s.substr(pos + 1);
    key = trim(lhs);
    val = trim(rhs);
    return !(key.empty() || val.empty());
}

std::string Simulator::usageComparative() {
    return
            "Usage: ./simulator_<ids> -comparative game_map=<file> game_managers_folder=<folder> algorithm1=<file> algorithm2=<file> [num_threads=<num>] [-verbose]\n";
}

std::string Simulator::usageCompetition() {
    return
            "Usage: ./simulator_<ids> -competition game_maps_folder=<folder> game_manager=<file> algorithms_folder=<folder> [num_threads=<num>] [-verbose]\n";
}

std::optional<Args> Simulator::handleUnknownArgs(const std::vector<std::string> &unknown, std::string &usageOrError) {
    std::ostringstream oss;
    oss << "Error: Unsupported arguments: ";
    for (size_t i = 0; i < unknown.size(); ++i) {
        if (i) oss << ", ";
        oss << "'" << unknown[i] << "'";
    }
    oss << "\n" << usageComparative() << usageCompetition();
    usageOrError = oss.str();
    return std::nullopt;
}

bool Simulator::checkComparativeArgsErrors(const Args &args, std::string &usageOrError) {
    std::vector<std::string> missing;
    if (args.gameMapFile.empty()) missing.push_back("game_map");
    if (args.gameManagersFolder.empty()) missing.push_back("game_managers_folder");
    if (args.algorithm1File.empty()) missing.push_back("algorithm1");
    if (args.algorithm2File.empty()) missing.push_back("algorithm2");
    if (!missing.empty()) {
        std::ostringstream oss;
        oss << "Error: Missing required arguments: ";
        for (size_t i = 0; i < missing.size(); ++i) {
            if (i) oss << ", ";
            oss << missing[i];
        }
        oss << "\n" << usageComparative();
        usageOrError = oss.str();
        return true;
    }
    if (!fileExistsReadable(args.gameMapFile)) {
        usageOrError = "Error: game_map file cannot be opened: " + args.gameMapFile + "\n" + usageComparative();
        return true;
    }
    if (!dirExistsListable(args.gameManagersFolder)) {
        usageOrError = "Error: game_managers_folder cannot be traversed: " + args.gameManagersFolder + "\n" +
                       usageComparative();
        return true;
    }
    if (!fileExistsReadable(args.algorithm1File)) {
        usageOrError = "Error: algorithm1 file cannot be opened: " + args.algorithm1File + "\n" +
                       usageComparative();
        return true;
    }
    if (!fileExistsReadable(args.algorithm2File)) {
        usageOrError = "Error: algorithm2 file cannot be opened: " + args.algorithm2File + "\n" +
                       usageComparative();
        return true;
    }
    return false;
}

bool Simulator::checkCompetitionArgsErrors(const Args &args, std::string &usageOrError) {
    std::vector<std::string> missing;
    if (args.gameMapsFolder.empty()) missing.push_back("game_maps_folder");
    if (args.gameManagerFile.empty()) missing.push_back("game_manager");
    if (args.algorithmsFolder.empty()) missing.push_back("algorithms_folder");
    if (!missing.empty()) {
        std::ostringstream oss;
        oss << "Error: Missing required arguments: ";
        for (size_t i = 0; i < missing.size(); ++i) {
            if (i) oss << ", ";
            oss << missing[i];
        }
        oss << "\n" << usageCompetition();
        usageOrError = oss.str();
        return true;
    }
    if (!dirExistsListable(args.gameMapsFolder)) {
        usageOrError = "Error: game_maps_folder cannot be traversed: " + args.gameMapsFolder + "\n" +
                       usageCompetition();
        return true;
    }
    if (!fileExistsReadable(args.gameManagerFile)) {
        usageOrError = "Error: game_manager file cannot be opened: " + args.gameManagerFile + "\n" +
                       usageCompetition();
        return true;
    }
    if (!dirExistsListable(args.algorithmsFolder)) {
        usageOrError = "Error: algorithms_folder cannot be traversed: " + args.algorithmsFolder + "\n" +
                       usageCompetition();
        return true;
    }
    return false;
}

bool Simulator::checkModeErrors(const Args &args, std::string &usageOrError) {
    if (args.mode == Args::Mode::Comparative) {
        if (checkComparativeArgsErrors(args, usageOrError)) {
            return true;
        }
        auto gms = listFilesWithExtension(args.gameManagersFolder, ".so");
        if (gms.empty()) {
            usageOrError = "Error: game_managers_folder has no .so files: " + args.gameManagersFolder + "\n" +
                           usageComparative();
            return true;
        }
    } else {
        // Competition
        if (checkCompetitionArgsErrors(args, usageOrError)) {
            return true;
        }
        auto maps = listRegularFiles(args.gameMapsFolder);
        if (maps.empty()) {
            usageOrError = "Error: game_maps_folder has no map files\n" + usageCompetition();
            return true;
        }
        auto algs = listFilesWithExtension(args.algorithmsFolder, ".so");
        if (algs.size() < 2) {
            usageOrError = "Error: algorithms_folder has fewer than 2 algorithms (.so files)\n" + usageCompetition();
            return true;
        }
    }

    return false;
}

std::optional<Args::Mode> Simulator::sortArg(const std::string &a, const bool modeSet, Args &args,
                                             std::string &usageOrError, std::vector<std::string> &unknown,
                                             const std::unordered_set<std::string> &supported) {
    if (a == "-comparative") {
        if (modeSet && args.mode != Args::Mode::Comparative) {
            usageOrError = "Error: Both -comparative and -competition provided.\n" + usageComparative() +
                           usageCompetition();
            return std::nullopt;
        }
        return Args::Mode::Comparative;
    }
    if (a == "-competition") {
        if (modeSet && args.mode != Args::Mode::Competition) {
            usageOrError = "Error: Both -comparative and -competition provided.\n" + usageComparative() +
                           usageCompetition();
            return std::nullopt;
        }
        return Args::Mode::Competition;
    }
    if (a == "-verbose") {
        args.verbose = true;
        return std::nullopt;
    }
    std::string k, v;
    if (!parseKV(a, k, v)) {
        unknown.push_back(a);
        return std::nullopt;
    }
    if (!supported.count(k)) {
        unknown.push_back(a);
        return std::nullopt;
    }
    if (k == "game_map") args.gameMapFile = v;
    else if (k == "game_managers_folder") args.gameManagersFolder = v;
    else if (k == "algorithm1") args.algorithm1File = v;
    else if (k == "algorithm2") args.algorithm2File = v;
    else if (k == "game_maps_folder") args.gameMapsFolder = v;
    else if (k == "game_manager") args.gameManagerFile = v;
    else if (k == "algorithms_folder") args.algorithmsFolder = v;
    else if (k == "num_threads") {
        try { args.numThreads = std::max(1, std::stoi(v)); } catch (...) { args.numThreads = 1; }
    }
    return std::nullopt;
}

std::optional<Args> Simulator::parseArgs(int argc, char **argv, std::string &usageOrError) {
    if (argc < 2) {
        usageOrError = usageComparative() + usageCompetition();
        return std::nullopt;
    }

    Args args{};
    bool modeSet = false;
    std::unordered_set<std::string> supported = {
        "-comparative", "-competition", "-verbose",
        "game_map", "game_managers_folder", "algorithm1", "algorithm2",
        "game_maps_folder", "game_manager", "algorithms_folder",
        "num_threads"
    };
    std::vector<std::string> unknown;

    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        std::optional<Args::Mode> mode = sortArg(a, modeSet, args, usageOrError, unknown, supported);
        if (mode.has_value()) {
            args.mode = mode.value();
            modeSet = true;
        }
    }

    if (!unknown.empty()) {
        return handleUnknownArgs(unknown, usageOrError);
    }

    if (!modeSet) {
        usageOrError = "Error: Missing mode (-comparative or -competition)\n" + usageComparative() + usageCompetition();
        return std::nullopt;
    }

    if (checkModeErrors(args, usageOrError)) return std::nullopt;
    return args;
}

// -------------------- Registrar loading --------------------
std::optional<Simulator::AlgWrap> Simulator::loadAlgorithmSo(const std::string &soPath) {
    auto &reg = AlgorithmRegistrar::getAlgorithmRegistrar();
    const std::string name = basenameNoExt(soPath);

    reg.createAlgorithmFactoryEntry(name);
    void *h = dlopenOrNull(soPath);
    if (!h) {
        reg.removeLast();
        return std::nullopt;
    }

    try {
        reg.validateLastRegistration();
    } catch (const AlgorithmRegistrar::BadRegistrationException &e) {
        std::cerr << "Algorithm registration failed for '" << name << "':"
                << " hasName=" << e.hasName
                << " hasPlayerFactory=" << e.hasPlayerFactory
                << " hasTankAlgorithmFactory=" << e.hasTankAlgorithmFactory
                << "\n";
        reg.removeLast();
        dlclose(h);
        return std::nullopt;
    }

    // Build a stable wrapper capturing the registrar index (not a reference)
    AlgWrap wrap;
    wrap.name = name;
    wrap.dl = h;
    wrap.playerFactory = reg.lastReg()->getPlayerFactory();
    wrap.tankFactory = reg.lastReg()->getTankAlgorithmFactory();

    return wrap;
}

std::vector<Simulator::AlgWrap> Simulator::loadAlgorithmFolder(const std::string &folder) {
    std::vector<AlgWrap> out;
    const auto sos = listFilesWithExtension(folder, ".so");
    for (auto &so: sos) {
        if (auto wrap = loadAlgorithmSo(so); wrap.has_value())
            out.emplace_back(wrap.value());
    }
    return out;
}

std::optional<Simulator::GmWrap> Simulator::loadGameManagerSo(const std::string &soPath) {
    auto &reg = GameManagerRegistrar::getGameManagerRegistrar();
    const std::string name = basenameNoExt(soPath);

    reg.createGameManagerFactoryEntry(name);
    void *h = dlopenOrNull(soPath);
    if (!h) {
        reg.removeLast();
        return std::nullopt;
    }

    try {
        reg.validateLastRegistration();
    } catch (const GameManagerRegistrar::BadRegistrationException &) {
        std::cerr << "GameManager registration failed for '" << name << "'.\n";
        reg.removeLast();
        dlclose(h);
        return std::nullopt;
    }

    GmWrap wrap;
    wrap.dl = h;
    wrap.name = name;
    wrap.makeGameManager = reg.lastReg()->getGameManagerFactory();

    return wrap;
}

std::vector<Simulator::GmWrap> Simulator::loadGameManagersFolder(const std::string &folder) {
    std::vector<GmWrap> out;
    std::vector<void *> handles;
    const auto sos = listFilesWithExtension(folder, ".so");
    for (auto &so: sos) {
        if (auto wrap = loadGameManagerSo(so); wrap.has_value())
            out.emplace_back(wrap.value());
    }
    return out;
}

// -------------------- Pairing --------------------
std::vector<std::pair<size_t, size_t> > Simulator::computePairsForK(size_t N, size_t k, bool dedupUnordered) {
    std::vector<std::pair<size_t, size_t> > pairs;
    pairs.reserve(N);
    std::unordered_set<uint64_t> seen;
    auto key = [](size_t a, size_t b)-> uint64_t {
        if (a > b) std::swap(a, b);
        return (uint64_t(a) << 32) | uint64_t(b);
    };
    for (size_t i = 0; i < N; i++) {
        size_t j = (i + 1 + (k % (N - 1))) % N;
        if (dedupUnordered) {
            uint64_t kk = key(i, j);
            if (seen.count(kk)) continue;
            seen.insert(kk);
        }
        pairs.emplace_back(i, j);
    }
    return pairs;
}

// -------------------- Comparative key hashing --------------------
size_t Simulator::ComparativeKeyHash::operator()(const ComparativeKey &k) const noexcept {
    std::hash<std::string> hs;
    size_t h = std::hash<int>()(k.winner) ^ std::hash<int>()(int(k.reason)) ^ std::hash<size_t>()(k.rounds);
    for (auto &line: k.finalMapDump) {
        h ^= hs(line) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
    }
    return h;
}

bool Simulator::ComparativeKeyEq::operator()(const ComparativeKey &a, const ComparativeKey &b) const noexcept {
    return a.winner == b.winner && a.reason == b.reason && a.rounds == b.rounds && a.finalMapDump == b.finalMapDump;
}

// -------------------- Output formatting --------------------
static std::string reasonToString(GameResult::Reason r) {
    switch (r) {
        case GameResult::ALL_TANKS_DEAD: return "all tanks dead";
        case GameResult::MAX_STEPS: return "max steps";
        case GameResult::ZERO_SHELLS: return "zero shells";
        default: return "unknown";
    }
}

static std::string resultMessage(int winner, GameResult::Reason reason) {
    if (winner == 1) return "Player 1 wins: " + reasonToString(reason);
    if (winner == 2) return "Player 2 wins: " + reasonToString(reason);
    return "Tie: " + reasonToString(reason);
}

std::string Simulator::formatComparativeOutput(
    const std::string &gameMap,
    const std::string &alg1,
    const std::string &alg2,
    const std::vector<std::pair<std::vector<std::string>, ComparativeKey> > &grouped) {
    std::ostringstream out;
    out << "game_map=" << gameMap << "\n";
    out << "algorithm1=" << alg1 << "\n";
    out << "algorithm2=" << alg2 << "\n";
    out << "\n";
    bool firstGroup = true;
    for (auto &[gmNames, key]: grouped) {
        if (!firstGroup) out << "\n";
        firstGroup = false;
        for (size_t i = 0; i < gmNames.size(); ++i) {
            if (i) out << ", ";
            out << gmNames[i];
        }
        out << "\n";
        out << resultMessage(key.winner, key.reason) << "\n";
        out << key.rounds << "\n";
        for (auto &line: key.finalMapDump) out << line << "\n";
    }
    return out.str();
}

std::string Simulator::formatCompetitionOutput(
    const std::string &gameMapsFolder,
    const std::string &gameManagerSo,
    const std::vector<std::pair<std::string, int> > &scoresSorted) {
    std::ostringstream out;
    out << "game_maps_folder=" << gameMapsFolder << "\n";
    out << "game_manager=" << gameManagerSo << "\n";
    out << "\n";
    for (auto &[name, score]: scoresSorted) {
        out << name << " " << score << "\n";
    }
    return out.str();
}

// -------------------- Runners --------------------
int Simulator::run(const Args &args) {
    try {
        if (args.mode == Args::Mode::Comparative) return runComparative(args);
        return runCompetition(args);
    } catch (const std::exception &e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
}

// TODO too long
int Simulator::runComparative(const Args &args) {
    // Fresh registrars
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    GameManagerRegistrar::getGameManagerRegistrar().clear();

    // Load algorithms (single dlopen each)
    std::optional<AlgWrap> alg1 = loadAlgorithmSo(args.algorithm1File);
    std::optional<AlgWrap> alg2 = loadAlgorithmSo(args.algorithm2File);
    if (!alg1 || !alg2) {
        std::cerr << "Error: failed to load algorithm(s).\n" << usageComparative();
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
        closeAlgWrap(alg1);
        closeAlgWrap(alg2);
        return 1;
    }

    // Load game managers
    auto gms = loadGameManagersFolder(args.gameManagersFolder);
    if (gms.empty()) {
        std::cerr << "Error: game_managers_folder has no valid .so game managers.\n" << usageComparative();
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
        GameManagerRegistrar::getGameManagerRegistrar().clear();
        closeAlgWrap(alg1);
        closeAlgWrap(alg2);
        closeGmWrap(gms);
        return 1;
    }

    // Parse map using project InputParser (initializes Logger as needed)
    Logger::getInstance().init(args.gameMapFile);
    InputParser parser;
    parser.parseInputFile(args.gameMapFile);
    SatelliteView *mapView = parser.getSatelliteView();
    if (!mapView) {
        std::cerr << "Error: failed to parse game map: " << args.gameMapFile << "\n" << usageComparative();
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
        GameManagerRegistrar::getGameManagerRegistrar().clear();
        closeAlgWrap(alg1);
        closeAlgWrap(alg2);
        closeGmWrap(gms);
        return 1;
    }

    const size_t width = parser.getWidth();
    const size_t height = parser.getHeight();
    const size_t maxSteps = parser.getMaxSteps();
    const size_t numShells = parser.getNumShells();
    const std::string mapName = parser.getBoardDescription();

    struct OneRes {
        std::string gmName;
        ComparativeKey key;
    };
    std::mutex mtx;
    std::vector<OneRes> results;

    auto runOne = [&](const GmWrap &gw) {
        auto gm = gw.makeGameManager(args.verbose);
        if (!gm) return;

        auto p1 = alg1->playerFactory(1, width, height, maxSteps, numShells);
        auto p2 = alg2->playerFactory(2, width, height, maxSteps, numShells);
        if (!p1 || !p2) return;

        GameResult gr = gm->run(width, height, *mapView, mapName, maxSteps, numShells,
                                *p1, alg1->name, *p2, alg2->name, alg1->tankFactory, alg2->tankFactory);

        ComparativeKey key;
        key.winner = gr.winner;
        key.reason = gr.reason;
        key.rounds = gr.rounds;

        // Dump final game state
        std::vector<std::string> dump;
        dump.reserve(height);
        SatelliteView *state = gr.gameState ? gr.gameState.get() : mapView;
        for (size_t y = 0; y < height; ++y) {
            std::string row;
            row.reserve(width);
            for (size_t x = 0; x < width; ++x) row.push_back(state->getObjectAt(x, y));
            dump.push_back(std::move(row));
        }
        key.finalMapDump = std::move(dump);

        std::lock_guard<std::mutex> lk(mtx);
        results.push_back({gw.name, std::move(key)});
    };

    if (args.numThreads <= 1) {
        for (const auto &gm: gms) runOne(gm);
    } else {
        ThreadPool pool(static_cast<size_t>(args.numThreads));
        for (const auto &gm: gms) pool.enqueue([&, gm] { runOne(gm); });
        pool.waitIdle();
    }

    if (results.empty()) {
        std::cerr << "Error: no game managers produced results.\n";
        // Clean up in correct order
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
        GameManagerRegistrar::getGameManagerRegistrar().clear();
        closeAlgWrap(alg1);
        closeAlgWrap(alg2);
        closeGmWrap(gms);
        return 1;
    }

    // Group identical results
    std::unordered_map<ComparativeKey, std::vector<std::string>, ComparativeKeyHash, ComparativeKeyEq> groups;
    for (auto &r: results) groups[r.key].push_back(r.gmName);

    std::vector<std::pair<std::vector<std::string>, ComparativeKey> > grouped;
    grouped.reserve(groups.size());
    for (auto &kv: groups) {
        auto key = kv.first;
        auto names = kv.second;
        std::sort(names.begin(), names.end());
        grouped.emplace_back(std::move(names), std::move(key));
    }

    std::sort(grouped.begin(), grouped.end(), [](const auto &a, const auto &b) {
        return a.first.size() > b.first.size();
    });

    // Emit output
    const auto outPath = joinPath(args.gameManagersFolder, "comparative_results_" + epochTimeId() + ".txt");
    const auto content = formatComparativeOutput(args.gameMapFile, args.algorithm1File, args.algorithm2File, grouped);
    std::string err;
    if (!writeTextFile(outPath, content, err)) {
        std::cerr << "Error: " << err << "\n";
        std::cout << content;
    }

    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    GameManagerRegistrar::getGameManagerRegistrar().clear();
    closeAlgWrap(alg1);
    closeAlgWrap(alg2);
    closeGmWrap(gms);

    return 0;
}

// TODO too long
int Simulator::runCompetition(const Args &args) {
    // Fresh registrars
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    GameManagerRegistrar::getGameManagerRegistrar().clear();

    // Load GM
    auto gmWrap = loadGameManagerSo(args.gameManagerFile);
    if (!gmWrap) {
        std::cerr << "Error: failed to load game_manager.\n" << usageCompetition();
        closeGmWrap(gmWrap);
        return 1;
    }

    // Load algorithms folder
    auto algs = loadAlgorithmFolder(args.algorithmsFolder);
    if (algs.size() < 2) {
        std::cerr << "Error: algorithms_folder has fewer than 2 valid .so files.\n" << usageCompetition();
        // Ensure registrars are empty and close libs
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
        GameManagerRegistrar::getGameManagerRegistrar().clear();
        closeAlgWrap(algs);
        closeGmWrap(gmWrap);
        return 1;
    }

    // List maps
    auto mapFiles = listRegularFiles(args.gameMapsFolder);
    if (mapFiles.empty()) {
        std::cerr << "Error: no map files in game_maps_folder.\n" << usageCompetition();
        // Cleanup ordering
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
        GameManagerRegistrar::getGameManagerRegistrar().clear();
        closeAlgWrap(algs);
        closeGmWrap(gmWrap);
        return 1;
    }

    std::vector<int> scores(algs.size(), 0);
    auto addWin = [&](size_t idx) { scores[idx] += 3; };
    auto addTie = [&](size_t a, size_t b) {
        scores[a] += 1;
        scores[b] += 1;
    };

    std::mutex mtx;

    auto runOne = [&](const std::string &mapPath, size_t ai, size_t aj) {
        try {
            std::lock_guard<std::mutex> glk(g_logger_mutex);
            Logger::getInstance().init(mapPath);
            InputParser parser;
            parser.parseInputFile(mapPath);
            SatelliteView *mapView = parser.getSatelliteView();
            if (!mapView) return;

            const size_t width = parser.getWidth();
            const size_t height = parser.getHeight();
            const size_t maxSteps = parser.getMaxSteps();
            const size_t numShells = parser.getNumShells();
            const std::string mapName = parser.getBoardDescription();

            auto gm = gmWrap->makeGameManager(args.verbose);
            if (!gm) return;

            auto p1 = algs[ai].playerFactory(1, width, height, maxSteps, numShells);
            auto p2 = algs[aj].playerFactory(2, width, height, maxSteps, numShells);
            if (!p1 || !p2) return;

            GameResult gr = gm->run(width, height, *mapView, mapName, maxSteps, numShells,
                                    *p1, algs[ai].name, *p2, algs[aj].name,
                                    algs[ai].tankFactory, algs[aj].tankFactory);

            std::lock_guard<std::mutex> lk(mtx);
            if (gr.winner == 1) addWin(ai);
            else if (gr.winner == 2) addWin(aj);
            else addTie(ai, aj);
        } catch (...) {
            // Swallow to keep tournament running; earlier logging should have captured clues
        }
    };

    bool usePool = args.numThreads >= 2;
    std::unique_ptr<ThreadPool> pool;
    if (usePool) pool = std::make_unique<ThreadPool>(static_cast<size_t>(args.numThreads));

    const size_t N = algs.size();
    for (size_t k = 0; k < mapFiles.size(); ++k) {
        bool dedup = (N % 2 == 0) && (k == (N / 2 - 1));
        auto pairs = computePairsForK(N, k, dedup);
        for (auto [i,j]: pairs) {
            if (usePool) pool->enqueue([=] { runOne(mapFiles[k], i, j); });
            else runOne(mapFiles[k], i, j);
        }
    }
    if (usePool) pool->waitIdle();

    // Build sorted scoreboard
    std::vector<std::pair<std::string, int> > out;
    out.reserve(algs.size());
    for (size_t i = 0; i < algs.size(); ++i) out.emplace_back(algs[i].name, scores[i]);
    std::sort(out.begin(), out.end(), [](const auto &a, const auto &b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });
    // Emit output
    const auto content = formatCompetitionOutput(args.gameMapsFolder, args.gameManagerFile, out);
    const auto outPath = joinPath(args.algorithmsFolder, "competition_" + epochTimeId() + ".txt");
    std::string err;
    if (!writeTextFile(outPath, content, err)) {
        std::cerr << "Error: " << err << "\n";
        std::cout << content;
    }

    // IMPORTANT: Clean up in the correct order
    // 1) Destroy all std::function and lambdas that might reference plugin code
    out.clear();
    scores.clear();
    mapFiles.clear();
    if (pool) pool.reset();

    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    GameManagerRegistrar::getGameManagerRegistrar().clear();
    closeAlgWrap(algs);
    closeGmWrap(gmWrap);

    return 0;
}
