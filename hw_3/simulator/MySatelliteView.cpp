#include "MySatelliteView.h"

void MySatelliteView::setDimensions(const size_t width, const size_t height) {
    this->width = width;
    this->height = height;
    this->board = std::vector(width, std::vector(height, ' '));
}

char MySatelliteView::getObjectAt(size_t x, size_t y) const {
    if (x >= width || y >= height) return '&';
    return board[x][y];
}

void MySatelliteView::setObjectAt(size_t x, size_t y, char c) {
    if (x >= width || y >= height) return;
    board[x][y] = c;
}
