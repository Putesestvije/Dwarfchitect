#ifndef MAXIMALRECTANGLE_H
#define MAXIMALRECTANGLE_H

#include <QObject>


#include "floor.h"
#include "coords.h"
#include "macroalgorithm.h"

class MaximalRectangle : public MacroAlgorithm
{
    Q_OBJECT
public:
    MaximalRectangle();
    MaximalRectangle(Floor *top, int width, int height);

    void generateMacro();
    int amountOfWork();
    int area(Coords LL, Coords UR);
    void clearSite();
signals:
    void progessed(int p);
    void syncFaces();
private:
    Floor *_currentFloor;
    int _width;
    int _height;


    QVector<Coords> _rectStack;
    QVector<int> _cache;
    Coords _bestLL;
    Coords _bestUR;
    int _currRectangle;
    bool _plottedSomething;

    void plotSite();
    void plotFloor(Designation d, Floor *f);
    void plotRectangle(Floor *f);
    void mainAlgorithm(Designation d);
    void updateCache(int x, Designation d);
};

#endif // MAXIMALRECTANGLE_H
