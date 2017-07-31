#ifndef TILEBACK_H
#define TILEBACK_H
#include "designations.h"
#include <iostream>

struct TileBack{
    Key des;

    /*to be used in the maximal rectangle algorithm */
    int rectangle;
    /*used in the bfs for rectangles not yet designated */
    bool bfs;
    bool macroed;
    TileBack(){
        des = D_CLEAR;
        /*-1 stands for belongs to no rectangle*/
        rectangle = 0;
        /*true once the macro designating its rectangle is made*/
        macroed = false;

        bfs = false;
    }
};



#endif // TILEBACK_H
