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
        Logger::getInstance().error("Failed to load library '" + path + "': " + dlerror());
        return nullptr;
    }
    return h;
}

void Simulator::closeAlgWrap(AlgWrap &wrap) {
    wrap.player_factory = nullptr;
    wrap.tank_factory = nullptr;
    if (wrap.has_dl) dlclose(wrap.dl);
}

void Simulator::closeAlgWrap(std::optional<AlgWrap> &wrap) {
    if (wrap) closeAlgWrap(*wrap);
}

void Simulator::closeGmWrap(GmWrap &wrap) {
    wrap.make_game_manager = nullptr;
    if (wrap.has_dl) dlclose(wrap.dl);
}

void Simulator::closeGmWrap(std::optional<GmWrap> &wrap) {
    if (wrap) closeGmWrap(*wrap);
}

void Simulator::closeAlgWrap(std::vector<AlgWrap> &handles) {
    for (int i = handles.size() - 1; i >= 0; i--) {
        closeAlgWrap(handles[i]);
    }
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

Simulator::ThreadPool::ThreadPool(size_t n_threads) : impl_(std::make_unique<Impl>()) {
    impl_->workers.reserve(n_threads);
    for (size_t i = 0; i < n_threads; i++) {
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

std::optional<Args> Simulator::handleUnknownArgs(const std::vector<std::string> &unknown, std::string &usage_or_error) {
    std::ostringstream oss;
    oss << "Error: Unsupported arguments: ";
    for (size_t i = 0; i < unknown.size(); ++i) {
        if (i) oss << ", ";
        oss << "'" << unknown[i] << "'";
    }
    oss << "\n" << usageComparative() << usageCompetition();
    usage_or_error = oss.str();
    Logger::getInstance().error(usage_or_error);
    return std::nullopt;
}

bool Simulator::checkComparativeArgsErrors(const Args &args, std::string &usage_or_error) {
    std::vector<std::string> missing;
    if (args.game_map_file.empty()) missing.push_back("game_map");
    if (args.game_managers_folder.empty()) missing.push_back("game_managers_folder");
    if (args.algorithm1_file.empty()) missing.push_back("algorithm1");
    if (args.algorithm2_file.empty()) missing.push_back("algorithm2");
    if (!missing.empty()) {
        std::ostringstream oss;
        oss << "Error: Missing required arguments: ";
        for (size_t i = 0; i < missing.size(); ++i) {
            if (i) oss << ", ";
            oss << missing[i];
        }
        oss << "\n" << usageComparative();
        usage_or_error = oss.str();
        return true;
    }
    if (!fileExistsReadable(args.game_map_file)) {
        usage_or_error = "Error: game_map file cannot be opened: " + args.game_map_file + "\n" + usageComparative();
        return true;
    }
    if (!dirExistsListable(args.game_managers_folder)) {
        usage_or_error = "Error: game_managers_folder cannot be traversed: " + args.game_managers_folder + "\n" +
                       usageComparative();
        return true;
    }
    if (!fileExistsReadable(args.algorithm1_file)) {
        usage_or_error = "Error: algorithm1 file cannot be opened: " + args.algorithm1_file + "\n" +
                       usageComparative();
        return true;
    }
    if (!fileExistsReadable(args.algorithm2_file)) {
        usage_or_error = "Error: algorithm2 file cannot be opened: " + args.algorithm2_file + "\n" +
                       usageComparative();
        return true;
    }
    return false;
}

bool Simulator::checkCompetitionArgsErrors(const Args &args, std::string &usage_or_error) {
    std::vector<std::string> missing;
    if (args.game_maps_folder.empty()) missing.push_back("game_maps_folder");
    if (args.game_manager_file.empty()) missing.push_back("game_manager");
    if (args.algorithms_folder.empty()) missing.push_back("algorithms_folder");
    if (!missing.empty()) {
        std::ostringstream oss;
        oss << "Error: Missing required arguments: ";
        for (size_t i = 0; i < missing.size(); ++i) {
            if (i) oss << ", ";
            oss << missing[i];
        }
        oss << "\n" << usageCompetition();
        usage_or_error = oss.str();
        return true;
    }
    if (!dirExistsListable(args.game_maps_folder)) {
        usage_or_error = "Error: game_maps_folder cannot be traversed: " + args.game_maps_folder + "\n" +
                       usageCompetition();
        return true;
    }
    if (!fileExistsReadable(args.game_manager_file)) {
        usage_or_error = "Error: game_manager file cannot be opened: " + args.game_manager_file + "\n" +
                       usageCompetition();
        return true;
    }
    if (!dirExistsListable(args.algorithms_folder)) {
        usage_or_error = "Error: algorithms_folder cannot be traversed: " + args.algorithms_folder + "\n" +
                       usageCompetition();
        return true;
    }
    return false;
}

bool Simulator::checkModeErrors(const Args &args, std::string &usage_or_error) {
    if (args.mode == Args::Mode::Comparative) {
        if (checkComparativeArgsErrors(args, usage_or_error)) {
            Logger::getInstance().error(usage_or_error);
            return true;
        }
        auto gms = listFilesWithExtension(args.game_managers_folder, ".so");
        if (gms.empty()) {
            usage_or_error = "Error: game_managers_folder has no .so files: " + args.game_managers_folder + "\n" +
                           usageComparative();
            Logger::getInstance().error(usage_or_error);
            return true;
        }
    } else {
        // Competition
        if (checkCompetitionArgsErrors(args, usage_or_error)) {
            Logger::getInstance().error(usage_or_error);
            return true;
        }
        auto maps = listRegularFiles(args.game_maps_folder);
        if (maps.empty()) {
            usage_or_error = "Error: game_maps_folder has no map files\n" + usageCompetition();
            Logger::getInstance().error(usage_or_error);
            return true;
        }
        auto algs = listFilesWithExtension(args.algorithms_folder, ".so");
        if (algs.size() < 2) {
            usage_or_error = "Error: algorithms_folder has fewer than 2 algorithms (.so files)\n" + usageCompetition();
            Logger::getInstance().error(usage_or_error);
            return true;
        }
    }

    return false;
}

std::optional<Args::Mode> Simulator::sortArg(const std::string &a, const bool mode_set, Args &args,
                                             std::string &usage_or_error, std::vector<std::string> &unknown,
                                             const std::unordered_set<std::string> &supported) {
    if (a == "-comparative") {
        if (mode_set && args.mode != Args::Mode::Comparative) {
            usage_or_error = "Error: Both -comparative and -competition provided.\n" + usageComparative() +
                           usageCompetition();
            return std::nullopt;
        }
        return Args::Mode::Comparative;
    }
    if (a == "-competition") {
        if (mode_set && args.mode != Args::Mode::Competition) {
            usage_or_error = "Error: Both -comparative and -competition provided.\n" + usageComparative() +
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
    if (k == "game_map") args.game_map_file = v;
    else if (k == "game_managers_folder") args.game_managers_folder = v;
    else if (k == "algorithm1") args.algorithm1_file = v;
    else if (k == "algorithm2") args.algorithm2_file = v;
    else if (k == "game_maps_folder") args.game_maps_folder = v;
    else if (k == "game_manager") args.game_manager_file = v;
    else if (k == "algorithms_folder") args.algorithms_folder = v;
    else if (k == "num_threads") {
        try { args.num_threads = std::max(1, std::stoi(v)); } catch (...) { args.num_threads = 1; }
    }
    return std::nullopt;
}

std::optional<Args> Simulator::parseArgs(int argc, char **argv, std::string &usage_or_error) {
    if (argc < 2) {
        usage_or_error = usageComparative() + usageCompetition();
        Logger::getInstance().error(usage_or_error);
        return std::nullopt;
    }

    Args args{};
    bool mode_set = false;
    std::unordered_set<std::string> supported = {
        "-comparative", "-competition", "-verbose",
        "game_map", "game_managers_folder", "algorithm1", "algorithm2",
        "game_maps_folder", "game_manager", "algorithms_folder",
        "num_threads"
    };
    std::vector<std::string> unknown;

    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        std::optional<Args::Mode> mode = sortArg(a, mode_set, args, usage_or_error, unknown, supported);
        if (mode.has_value()) {
            args.mode = mode.value();
            mode_set = true;
        }
    }
    const auto processor_count = std::thread::hardware_concurrency();
    args.num_threads = std::min(args.num_threads, processor_count);

    if (!unknown.empty()) {
        return handleUnknownArgs(unknown, usage_or_error);
    }

    if (!mode_set) {
        usage_or_error = "Error: Missing mode (-comparative or -competition)\n" + usageComparative() + usageCompetition();
        Logger::getInstance().error(usage_or_error);
        return std::nullopt;
    }

    if (checkModeErrors(args, usage_or_error)) return std::nullopt;
    return args;
}

// -------------------- Registrar loading --------------------
std::optional<Simulator::AlgWrap> Simulator::loadAlgorithmSo(const std::string &so_path) {
    auto &reg = AlgorithmRegistrar::getAlgorithmRegistrar();
    const std::string name = basenameNoExt(so_path);

    AlgWrap wrap;
    wrap.name = name;
    wrap.dl = nullptr;
    wrap.has_dl = false;

    if (!reg.find(name)) {
        reg.createAlgorithmFactoryEntry(name);
        void *h = dlopenOrNull(so_path);
        if (!h) {
            reg.removeLast();
            return std::nullopt;
        }
        wrap.dl = h;
        wrap.has_dl = true;

        try {
            reg.validateLastRegistration();
        } catch (const AlgorithmRegistrar::BadRegistrationException &e) {
            Logger::getInstance().error("Algorithm registration failed for '" + name + "':"
                                        + " hasName=" + std::to_string(e.has_name)
                                        + " hasPlayerFactory=" + std::to_string(e.has_player_factory)
                                        + " hasTankAlgorithmFactory=" + std::to_string(e.has_tank_algorithm_factory));
            reg.removeLast();
            dlclose(h);
            return std::nullopt;
        }
    }

    wrap.player_factory = reg.find(name)->getPlayerFactory();
    wrap.tank_factory = reg.find(name)->getTankAlgorithmFactory();

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

std::optional<Simulator::GmWrap> Simulator::loadGameManagerSo(const std::string &so_path) {
    auto &reg = GameManagerRegistrar::getGameManagerRegistrar();
    const std::string name = basenameNoExt(so_path);
    GmWrap wrap;
    wrap.name = name;
    wrap.dl = nullptr;
    wrap.has_dl = false;

    if (!reg.find(name)) {
        reg.createGameManagerFactoryEntry(name);
        void *h = dlopenOrNull(so_path);
        if (!h) {
            reg.removeLast();
            return std::nullopt;
        }
        wrap.dl = h;
        wrap.has_dl = true;

        try {
            reg.validateLastRegistration();
        } catch (const GameManagerRegistrar::BadRegistrationException &) {
            Logger::getInstance().error("GameManager registration failed for '" + name + "'.");
            reg.removeLast();
            dlclose(h);
            return std::nullopt;
        }
    }

    wrap.make_game_manager = reg.find(name)->getGameManagerFactory();

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
std::vector<std::pair<size_t, size_t> > Simulator::computePairsForK(size_t N, size_t k, bool dedup_unordered) {
    std::vector<std::pair<size_t, size_t> > pairs;
    pairs.reserve(N);
    std::unordered_set<uint64_t> seen;
    auto key = [](size_t a, size_t b)-> uint64_t {
        if (a > b) std::swap(a, b);
        return (uint64_t(a) << 32) | uint64_t(b);
    };
    for (size_t i = 0; i < N; i++) {
        size_t j = (i + 1 + (k % (N - 1))) % N;
        if (dedup_unordered) {
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
    for (auto &line: k.final_map_dump) {
        h ^= hs(line) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
    }
    return h;
}

bool Simulator::ComparativeKeyEq::operator()(const ComparativeKey &a, const ComparativeKey &b) const noexcept {
    return a.winner == b.winner && a.reason == b.reason && a.rounds == b.rounds && a.final_map_dump == b.final_map_dump;
}

// -------------------- Output formatting --------------------
std::string Simulator::resultMessage(ComparativeKey key, size_t max_steps) {
    size_t tanks1 = 0, tanks2 = 0;
    if (key.remaining_tanks.at(0)) tanks1 = key.remaining_tanks.at(0);
    if (key.remaining_tanks.at(1)) tanks2 = key.remaining_tanks.at(1);

    switch (key.winner) {
        case 0:
            switch (key.reason) {
                case GameResult::ALL_TANKS_DEAD:
                    return "Tie, both players have zero tanks";
                case GameResult::MAX_STEPS:
                    return "Tie, reached max steps = " + std::to_string(max_steps) + ", player 1 has " +
                           std::to_string(tanks1) + " tanks, player 2 has " + std::to_string(tanks2) + " tanks";
                case GameResult::ZERO_SHELLS:
                    return "Tie, both players have zero shells for " + std::to_string(max_steps_empty_ammo) + " steps";
                default:
                    return "Bad result";
            }
        case 1:
            return "Player 1 won with " + std::to_string(tanks1) + " tanks still alive";
        case 2:
            return "Player 2 won with " + std::to_string(tanks2) + " tanks still alive";
        default:
            return "Bad result";
    }
}

std::string Simulator::formatComparativeOutput(
    const std::string &game_map,
    const std::string &alg1,
    const std::string &alg2,
    const std::vector<std::pair<std::vector<std::string>, ComparativeKey> > &grouped,
    size_t max_steps
) {
    std::ostringstream out;
    out << "game_map=" << game_map << "\n";
    out << "algorithm1=" << basenameNoExt(alg1) << "\n";
    out << "algorithm2=" << basenameNoExt(alg2) << "\n";
    out << "\n";
    bool first_group = true;
    for (auto &[gm_names, key]: grouped) {
        if (!first_group) out << "\n";
        first_group = false;
        for (size_t i = 0; i < gm_names.size(); ++i) {
            if (i) out << ", ";
            out << gm_names[i];
        }
        out << "\n";
        out << resultMessage(key, max_steps) << "\n";
        out << key.rounds << "\n";
        for (auto &line: key.final_map_dump) out << line << "\n";
    }
    return out.str();
}

std::string Simulator::formatCompetitionOutput(
    const std::string &game_maps_folder,
    const std::string &game_manager_so,
    const std::vector<std::pair<std::string, int> > &scores_sorted) {
    std::ostringstream out;
    out << "game_maps_folder=" << game_maps_folder << "\n";
    out << "game_manager=" << basenameNoExt(game_manager_so) << "\n";
    out << "\n";
    for (auto &[name, score]: scores_sorted) {
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
        Logger::getInstance().error("Fatal error");
        std::cerr << e.what() << std::endl;
        return 1;
    }
}

void Simulator::cleanComparative(std::optional<AlgWrap> &alg1, std::optional<AlgWrap> &alg2, std::vector<GmWrap> &gms) {
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    GameManagerRegistrar::getGameManagerRegistrar().clear();
    if (alg1.has_value()) closeAlgWrap(alg1);
    if (alg2.has_value()) closeAlgWrap(alg2);
    closeGmWrap(gms);
}

int Simulator::loadComparative(const Args &args, std::optional<AlgWrap> &alg1, std::optional<AlgWrap> &alg2,
                               std::vector<GmWrap> &gms, std::unique_ptr<SatelliteView> &map_view, size_t &width,
                               size_t &height, size_t &max_steps, size_t &num_shells, std::string &map_name) {
    // Fresh registrars
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    GameManagerRegistrar::getGameManagerRegistrar().clear();

    // Load algorithms (single dlopen each)
    alg1 = loadAlgorithmSo(args.algorithm1_file);
    alg2 = loadAlgorithmSo(args.algorithm2_file);
    // Load game managers
    gms = loadGameManagersFolder(args.game_managers_folder);

    if (!alg1 || !alg2) {
        Logger::getInstance().error("Error: failed to load algorithm(s).\n" + usageComparative());
        std::cerr << "Error: failed to load algorithm(s).\n" << usageComparative();
        cleanComparative(alg1, alg2, gms);
        return 1;
    }
    if (gms.empty()) {
        Logger::getInstance().error(
            "Error: game_managers_folder has no valid .so game managers.\n" + usageComparative());
        std::cerr << "Error: game_managers_folder has no valid .so game managers.\n" << usageComparative();
        cleanComparative(alg1, alg2, gms);
        return 1;
    }

    // Parse map using project InputParser (initializes Logger as needed)
    InputParser parser;
    parser.parseInputFile(args.game_map_file);
    map_view = parser.getSatelliteView();
    if (!map_view) {
        Logger::getInstance().error("Error: failed to parse game map: " + args.game_map_file + "\n" + usageComparative());
        std::cerr << "Error: failed to parse game map: " << args.game_map_file << "\n" << usageComparative();
        cleanComparative(alg1, alg2, gms);
        return 1;
    }

    width = parser.getWidth();
    height = parser.getHeight();
    max_steps = parser.getMaxSteps();
    num_shells = parser.getNumShells();
    map_name = parser.getBoardDescription();

    return 0;
}

void Simulator::runOneComparative(const GmWrap &gw, const Args &args, const std::optional<AlgWrap> &alg1,
                                  const std::optional<AlgWrap> &alg2, const std::unique_ptr<SatelliteView> &map_view,
                                  const size_t &width, const size_t &height, const size_t &max_steps,
                                  const size_t &num_shells, const std::string &map_name, std::mutex &mtx,
                                  std::vector<OneRes> &results) {
    try {
        auto gm = gw.make_game_manager(args.verbose);
        if (!gm) return;

        auto p1 = alg1->player_factory(1, width, height, max_steps, num_shells);
        auto p2 = alg2->player_factory(2, width, height, max_steps, num_shells);
        if (!p1 || !p2) return;

        GameResult gr = gm->run(width, height, *map_view, map_name, max_steps, num_shells,
                                *p1, alg1->name, *p2, alg2->name, alg1->tank_factory, alg2->tank_factory);

        ComparativeKey key;
        key.winner = gr.winner;
        key.reason = gr.reason;
        key.rounds = gr.rounds;
        key.remaining_tanks = gr.remaining_tanks; // index 0 = player 1, etc.

        // Dump final game state
        std::vector<std::string> dump;
        dump.reserve(height);
        const SatelliteView *state = gr.gameState ? gr.gameState.get() : map_view.get();
        for (size_t y = 0; y < height; ++y) {
            std::string row;
            row.reserve(width);
            for (size_t x = 0; x < width; ++x) row.push_back(state->getObjectAt(x, y));
            dump.push_back(std::move(row));
        }
        key.final_map_dump = std::move(dump);

        std::lock_guard lk(mtx);
        results.push_back({gw.name, std::move(key)});
    } catch (...) {
    }
}

std::vector<std::pair<std::vector<std::string>, Simulator::ComparativeKey> >
Simulator::groupResComparative(const std::vector<OneRes> &results) {
    std::unordered_map<ComparativeKey, std::vector<std::string>, ComparativeKeyHash, ComparativeKeyEq> groups;
    for (auto &r: results) groups[r.key].push_back(r.gm_name);

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

    return grouped;
}

int Simulator::runComparative(const Args &args) {
    std::optional<AlgWrap> alg1, alg2;
    std::vector<GmWrap> gms;
    std::unique_ptr<SatelliteView> map_view;
    size_t width, height, max_steps, num_shells;
    std::string map_name;
    std::mutex mtx;
    std::vector<OneRes> results;

    if (loadComparative(args, alg1, alg2, gms, map_view, width, height, max_steps, num_shells, map_name) == 1)
        return 1;

    if (args.num_threads <= 1) {
        for (const auto &gm: gms)
            runOneComparative(gm, args, alg1, alg2, map_view, width, height, max_steps, num_shells,
                              map_name, mtx, results);
    } else {
        ThreadPool pool(static_cast<size_t>(args.num_threads));
        for (const auto &gm: gms)
            pool.enqueue([&, gm] {
                runOneComparative(gm, args, alg1, alg2, map_view, width, height, max_steps, num_shells,
                                  map_name, mtx, results);
            });
        pool.waitIdle();
    }

    if (results.empty()) {
        Logger::getInstance().error("Error: no game managers produced results.");
        std::cerr << "Error: no game managers produced results.\n";
        cleanComparative(alg1, alg2, gms);
        return 1;
    }
    auto grouped = groupResComparative(results);
    const auto out_path = joinPath(args.game_managers_folder, "comparative_results_" + epochTimeId() + ".txt");
    const auto content = formatComparativeOutput(args.game_map_file, args.algorithm1_file, args.algorithm2_file, grouped,
                                                 max_steps);
    if (std::string err; !writeTextFile(out_path, content, err)) {
        Logger::getInstance().error("Error: " + err);
        Logger::getInstance().log(content);
    }
    cleanComparative(alg1, alg2, gms);
    return 0;
}

void Simulator::cleanCompetition(std::optional<GmWrap> &gm_wrap, std::vector<AlgWrap> &algs) {
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    GameManagerRegistrar::getGameManagerRegistrar().clear();
    if (gm_wrap.has_value()) closeGmWrap(gm_wrap);
    closeAlgWrap(algs);
}

int Simulator::loadCompetition(const Args &args, std::optional<GmWrap> &gm_wrap, std::vector<AlgWrap> &algs,
                               std::vector<std::string> &map_files) {
    // Fresh registrars
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    GameManagerRegistrar::getGameManagerRegistrar().clear();

    // Load GM
    gm_wrap = loadGameManagerSo(args.game_manager_file);
    if (!gm_wrap) {
        Logger::getInstance().error("Error: failed to load game_manager.\n" + usageCompetition());
        std::cerr << "Error: failed to load game_manager.\n" << usageCompetition();
        cleanCompetition(gm_wrap, algs);
        return 1;
    }

    // Load algorithms folder
    algs = loadAlgorithmFolder(args.algorithms_folder);
    if (algs.size() < 2) {
        Logger::getInstance().
                error("Error: algorithms_folder has fewer than 2 valid .so files.\n" + usageCompetition());
        std::cerr << "Error: algorithms_folder has fewer than 2 valid .so files.\n" << usageCompetition();
        cleanCompetition(gm_wrap, algs);
        return 1;
    }

    // List maps
    map_files = listRegularFiles(args.game_maps_folder);
    if (map_files.empty()) {
        std::cerr << "Error: no map files in game_maps_folder.\n" << usageCompetition();
        Logger::getInstance().error("Error: no map files in game_maps_folder.\n");
        cleanCompetition(gm_wrap, algs);
        return 1;
    }

    return 0;
}

void Simulator::runOneCompetition(const Args &args, std::optional<GmWrap> &gm_wrap, std::vector<AlgWrap> &algs,
                                  const std::string &map_path, size_t ai, size_t aj, std::mutex &mtx,
                                  std::vector<int> &scores) {
    auto addWin = [&](size_t idx) { scores[idx] += 3; };
    auto addTie = [&](size_t a, size_t b) {
        scores[a] += 1;
        scores[b] += 1;
    };

    try {
        std::lock_guard glk(g_logger_mutex);
        InputParser parser;
        parser.parseInputFile(map_path);
        std::unique_ptr<SatelliteView> map_view = parser.getSatelliteView();
        if (!map_view) return;

        const size_t width = parser.getWidth();
        const size_t height = parser.getHeight();
        const size_t max_steps = parser.getMaxSteps();
        const size_t num_shells = parser.getNumShells();
        const std::string map_name = parser.getBoardDescription();

        auto gm = gm_wrap->make_game_manager(args.verbose);
        if (!gm) return;

        auto p1 = algs[ai].player_factory(1, width, height, max_steps, num_shells);
        auto p2 = algs[aj].player_factory(2, width, height, max_steps, num_shells);
        if (!p1 || !p2) return;

        GameResult gr = gm->run(width, height, *map_view, map_name, max_steps, num_shells,
                                *p1, algs[ai].name, *p2, algs[aj].name,
                                algs[ai].tank_factory, algs[aj].tank_factory);

        std::lock_guard lk(mtx);
        if (gr.winner == 1) addWin(ai);
        else if (gr.winner == 2) addWin(aj);
        else addTie(ai, aj);
    } catch (...) {
        // Swallow to keep tournament running; earlier logging should have captured clues
    }
}

void Simulator::buildSortedScoreboard(
    std::vector<AlgWrap> &algs, std::vector<int> &scores, std::vector<std::pair<std::string, int> > &out) {
    out.reserve(algs.size());
    for (size_t i = 0; i < algs.size(); ++i) out.emplace_back(algs[i].name, scores[i]);
    std::sort(out.begin(), out.end(), [](const auto &a, const auto &b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });
}

int Simulator::runCompetition(const Args &args) {
    std::optional<GmWrap> gm_wrap;
    std::vector<AlgWrap> algs;
    std::vector<std::string> map_files;
    if (loadCompetition(args, gm_wrap, algs, map_files) == 1)
        return 1;
    std::vector scores(algs.size(), 0);
    std::mutex mtx;

    bool use_pool = args.num_threads >= 2;
    std::unique_ptr<ThreadPool> pool;
    if (use_pool) pool = std::make_unique<ThreadPool>(static_cast<size_t>(args.num_threads));
    const size_t N = algs.size();
    for (size_t k = 0; k < map_files.size(); ++k) {
        bool dedup = (N % 2 == 0) && (k == (N / 2 - 1));
        auto pairs = computePairsForK(N, k, dedup);
        for (auto [i,j]: pairs) {
            if (use_pool)
                pool->enqueue([&, i, j, k] {
                    runOneCompetition(args, gm_wrap, algs, map_files[k], i, j, mtx, scores);
                });
            else runOneCompetition(args, gm_wrap, algs, map_files[k], i, j, mtx, scores);
        }
    }
    if (use_pool) pool->waitIdle();

    std::vector<std::pair<std::string, int> > out;
    buildSortedScoreboard(algs, scores, out);
    const auto content = formatCompetitionOutput(args.game_maps_folder, args.game_manager_file, out);
    const auto out_path = joinPath(args.algorithms_folder, "competition_" + epochTimeId() + ".txt");
    std::string err;
    if (!writeTextFile(out_path, content, err)) {
        Logger::getInstance().error("Error: " + err);
        Logger::getInstance().log(content);
    }

    scores.clear();
    map_files.clear();
    if (pool) pool.reset();
    cleanCompetition(gm_wrap, algs);
    return 0;
}
