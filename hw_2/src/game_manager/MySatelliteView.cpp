#include "MySatelliteView.h"

char MySatelliteView::getObjectAt(size_t x, size_t y) const {
    if (x >= width || y >= height) return '&';
    return board[x][y];
}

void MySatelliteView::setObjectAt(size_t x, size_t y, char c) {
    if (x >= width || y >= height) return;
    board[x][y] = c;
}
