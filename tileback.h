#ifndef TILEBACK_H
#define TILEBACK_H
#include "designations.h"


struct TileBack{
    Designation des;

    /*to be used in the maximal rectangle
     * algorithm */
    int rectangle;

    TileBack(){
        des = CLEAR;
        /*-1 stands for "belongs to rectangle*/
        rectangle = -1;
    }
};

#endif // TILEBACK_H
