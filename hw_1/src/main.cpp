#include <iostream>

#include "game/action.h"
#include "../utils/FilesUtils.h"


int main() {
    //todo: change it so we get it from user
    std::string path = "../input/map.txt";

    int status = FilesUtils::loadBoard(path);
    if (status == 0) {
        std::cout << "Map loaded successfully." << std::endl;
    } else {
        std::cerr << "Failed to load map." << std::endl;
    }

}
