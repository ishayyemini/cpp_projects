#ifndef MYSATELLITEVIEW_H
#define MYSATELLITEVIEW_H

#include <vector>

#include "SatelliteView.h"

class MySatelliteView : public SatelliteView {
    size_t width;
    size_t height;
    std::vector<std::vector<char> > board;

public:
    MySatelliteView() : width(0), height(0) {
    }

    MySatelliteView(const size_t width, const size_t height) : width(width), height(height),
                                                               board(width, std::vector<char>(height)) {
    }

    MySatelliteView(const MySatelliteView &obj) : width(obj.width), height(obj.height), board(obj.board) {
    }

    void setDimensions(size_t width, size_t height);

    char getObjectAt(size_t x, size_t y) const override;

    void setObjectAt(size_t x, size_t y, char c);
};


#endif //MYSATELLITEVIEW_H
