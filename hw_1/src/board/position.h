#ifndef POSITION_H
#define POSITION_H

class Position {
int x;
int y;

public:
    Position(int x, int y): x(x),y(y) {};

    void updatePosition(int x, int y) {
        this->x = x;
        this->y = y;
    }


};


#endif //POSITION_H
