#ifndef TILEBACK_H
#define TILEBACK_H
#include "designations.h"


struct TileBack{
    Designation des;

    /*to be used in the maximal rectangle algorithm */
    int rectangle;
    bool macroed;
    TileBack(){
        des = CLEAR;
        /*-1 stands for belongs to no rectangle*/
        rectangle = -1;
        /*true once the macro drwaign its rectangle is made*/
        macroed = false;
    }
};

#endif // TILEBACK_H
