#ifndef COORDS_H
#define COORDS_H
#include <iostream>

/* to be used to relay information which
 * tiles are to be changed*/
struct Coords {
    /*and yes, these are bitfields*/
    int y : 10;
    int x : 10;

    bool operator <(Coords &other){
        if (x != other.x)
            return x < other.x;
        else
            return y < other.y;
    }
    Coords operator +(Coords &other){
        return Coords(y+other.y, x+other.x);
    }

    Coords operator -(Coords &other){
        return Coords(y-other.y, x-other.x);
    }
    bool operator==(Coords other){
        if (!(y == other.y))
            return false;
        else if (!(x == other.x))
            return false;
        return true;
    }

    void operator = (const Coords &other){
        y = other.y;
        x = other.x;
    }

    friend std::ostream& operator<<(std::ostream &o, const Coords &c){
        o << "(" << c.y  << "," << c.x << ")";
        return o;
    }

    Coords(){y = 0; x = 0;}
    Coords(int a, int b){y = a; x = b;}
};



struct CoordPair{
    Coords first;
    Coords second;

    CoordPair(){first = Coords(); second = Coords();}
    CoordPair(Coords one, Coords two){first = one; second = two;}

};


#endif // COORDS_H
