#ifndef COORDS_H
#define COORDS_H
/* to be used to relay information which
 * tiles are to be changed*/
struct Coords {
    int x : 10;
    int y : 10;

    bool operator <(Coords &other){
        if (y != other.y)
            return y < other.y;
        else
            return x < other.x;
    }
};




#endif // COORDS_H
