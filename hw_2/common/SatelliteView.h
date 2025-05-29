//
// Created by Ishay Yemini on 26/05/2025.
//

#ifndef SATELLITEVIEW_H
#define SATELLITEVIEW_H

#include <cstddef>

class SatelliteView {
public:
    virtual ~SatelliteView() {
    }

    virtual char getObjectAt(size_t x, size_t y) const = 0;
};


#endif //SATELLITEVIEW_H
