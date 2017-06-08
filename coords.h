#ifndef COORDS_H
#define COORDS_H
#include <iostream>

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
    Coords operator +(Coords &other){
        return Coords(x+other.x, y+other.y);
    }
    bool operator==(Coords other){
        if (!(x == other.x))
            return false;
        else if (!(y == other.y))
            return false;
        return true;
    }
    friend std::ostream& operator<<(std::ostream &o, const Coords &c){
        o << "(" << c.x  << "," << c.y << ")";
        return o;
    }

    Coords(){x = 0; y = 0;}
    Coords(int a, int b){x = a; y = b;}
};



struct CoordPair{
    Coords first;
    Coords second;

    CoordPair(){first = Coords(); second = Coords();}
    CoordPair(Coords one, Coords two){first = one; second = two;}

};


#endif // COORDS_H
