#include "maximalrectangle.h"
#include <iostream>

MaximalRectangle::MaximalRectangle()
{

}

MaximalRectangle::MaximalRectangle(Floor *top, int width, int height)
    : _currentFloor(top),
      _width(width),
      _height(height)
{
    _currRectangle = 1;
    _plottedSomething = true;
}

void MaximalRectangle::generateMacro()
{
    plotSite();
}

int MaximalRectangle::amountOfWork()
{
    Floor *c = _currentFloor;
    int work = 0;
    while(c != nullptr){
        for(int i = 0; i < _height; i++){
            for(int j = 0; j < _width; j++){
                if(c->tiles()[i][j].des != CLEAR)
                    work++;
            }
        }
        c = c->floorBelow();
    }
    return work;
}

void MaximalRectangle::plotSite()
{
    Floor *f = _currentFloor;
    while(f != nullptr){
        plotFloor(Designation::DIG, f);
        f = _currentFloor->floorBelow();
    }

}

void MaximalRectangle::plotFloor(Designation d, Floor *f)
{
    while(_plottedSomething){
        _plottedSomething = false;
        _bestUR = Coords(-1,-1);
        _bestLL = Coords(0,0);
        _cache.clear();
        for(int i = 0; i <= _height; i++)
            _cache.push_back(0);
        mainAlgorithm(d);
        /* after the main algorithm the _bestLL and _bestUR fields
         * should be refering to opposite corners of the largest rectangle
         * to be marked up */
        std::cout.flush();
        emit progessed(area(_bestLL, _bestUR));
        std::cout << "found something at (" << _bestLL.x
                  << "," << _bestLL.y << "), (" << _bestUR.x
                  << "," << _bestUR.y << ")"
                  << "rectangle #" <<_currRectangle << std::endl;
        if(_plottedSomething)
            plotRectangle(f);
        emit syncFaces();
    }
}

void MaximalRectangle::plotRectangle(Floor *f)
{
    for (int i = _bestLL.y; i <= _bestUR.y; i++)
        for (int j = _bestLL.x; j <= _bestUR.x; j++)
            f->tiles()[i][j].rectangle = _currRectangle;

    _currRectangle++;
}

void MaximalRectangle::mainAlgorithm(Designation d)
{
    int widestRect;
    Coords p;
    _rectStack.clear();
    for (int x = _width-1; x > -1; x--){
        updateCache(x, d);
        widestRect = 0;
        for (int y = 0; y <= _height; y++){
            /*placing something on the rectangle stack*/
            if (_cache[y] > widestRect){
                Coords c(y, widestRect);
                _rectStack.push_back(c);
                widestRect = _cache[y];
            }
            /*closing the rectangle stack, possibly found a better one */
            if (_cache[y] < widestRect){
                do {
                    p = _rectStack.takeLast();
                    if ((widestRect * (y-p.x)) > area(_bestLL, _bestUR)){
                        _bestLL = Coords(x, p.x);
                        _bestUR = Coords(x+widestRect-1, y-1);
                    }
                    widestRect = p.y;
                } while (_cache[y] < widestRect);
                widestRect = _cache[y];
                if(widestRect != 0){
                    Coords c(p.x, p.y);
                    _rectStack.push_back(c);
                }
            }
        }
    }
}

void MaximalRectangle::updateCache(int x, Designation d)
{
    for (int i = 0; i < _height; i++){
        if((_currentFloor->tiles()[i][x].des == d) && (_currentFloor->tiles()[i][x].rectangle == -1)){
            _cache[i]++;
            _plottedSomething = true;
        }
        else
            _cache[i] = 0;
    }
}


/* This is used to calculate area generally, the only time
 * it shoud return 0 is in the main algorithm when the
 * UR is (-1,-1) */
int MaximalRectangle::area(Coords LL, Coords UR)
{
    if(UR.x == -1 || UR.y == -1)
        return 0;
    return (abs(LL.x - UR.x) + 1)*(abs(LL.y - UR.y) + 1);
}


/* Clears the site of traces of the algorithm's work like
 * setting the tile's containing rectangle and so on.*/
void MaximalRectangle::clearSite()
{
    Floor *c = _currentFloor;
    while(c != nullptr){
        for(int i = 0; i < _height; i++){
            for(int j = 0; j < _width; j++){
                c->tiles()[i][j].rectangle = -1;
                c->tiles()[i][j].macroed = false;
            }
        }
        c = c->floorBelow();
    }
}
