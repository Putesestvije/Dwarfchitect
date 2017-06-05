#ifndef COORDS_H
#define COORDS_H
/* to be used to relay information which
 * tiles are to be changed*/
struct Coords {
    /*and yes, these are bitfields*/
    int x : 10;
    int y : 10;

    bool operator <(Coords &other){
        if (y != other.y)
            return y < other.y;
        else
            return x < other.x;
    }
    Coords(){x = 0; y = 0;}
    Coords(int a, int b){x = a; y = b;}
};




#endif // COORDS_H
