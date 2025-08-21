#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <unordered_set>
#include <string>
#include <memory>

#include "MySatelliteView.h"

class InputParser {
    std::vector<std::string> error_messages;
    std::unique_ptr<MySatelliteView> satellite_view;
    std::string board_description;
    size_t width{};
    size_t height{};
    size_t max_steps{};
    size_t num_shells{};
    std::vector<std::pair<int, int> > tanks;
    const char default_symbol = ' ';
    const std::unordered_set<char> valid_symbols = {'1', '2', '@', '#', ' '};

    bool parseBoardConfig(std::ifstream &inFile, size_t &retrieved_data, const std::string &expected_field_name);

    void populateBoard(std::ifstream &in_file);

    void addErrorMessage(const std::string &message);

    void processLine(size_t row, const std::string &line);

    bool isValidSymbol(char c);

    void validateLineLength(size_t row, const std::string &line);

    bool parseBoardInfo(std::ifstream &in_file);

    void addErrorMessagesToLog();

    InputParser(const InputParser &) = delete;

    InputParser &operator=(const InputParser &) = delete;

    InputParser(InputParser &&) = delete;

    InputParser &operator=(InputParser &&) = delete;

public:
    InputParser() = default;

    void parseInputFile(const std::string &file_name);

    bool hasErrors() const { return !error_messages.empty(); }

    std::vector<std::pair<int, int> > getTanks() const { return tanks; }

    const std::vector<std::string> &getErrorMessages() const { return error_messages; }

    std::string getBoardDescription() const { return board_description; }

    size_t getWidth() const { return width; }

    size_t getHeight() const { return height; }

    size_t getMaxSteps() const { return max_steps; }

    size_t getNumShells() const { return num_shells; }

    std::unique_ptr<SatelliteView> getSatelliteView();

    ~InputParser() = default;
};

#endif //INPUTPARSER_H
