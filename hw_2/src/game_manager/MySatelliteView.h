//
// Created by Ishay Yemini on 29/05/2025.
//

#ifndef MYSATELLITEVIEW_H
#define MYSATELLITEVIEW_H

#include <list>

#include "SatelliteView.h"

class MySatelliteView : public SatelliteView {
    const size_t width;
    const size_t height;
    std::vector<std::vector<char> > board;

public:
    MySatelliteView(const size_t width, const size_t height): width(width), height(height),
                                                              board(width, std::vector<char>(height)) {
    }

    char getObjectAt(size_t x, size_t y) const override;

    void setObjectAt(size_t x, size_t y, char c);
};

#endif //MYSATELLITEVIEW_H
