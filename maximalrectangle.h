#ifndef MAXIMALRECTANGLE_H
#define MAXIMALRECTANGLE_H

#include <QObject>
#include <QHash>

#include "floor.h"
#include "coords.h"
#include "macroalgorithm.h"

class MaximalRectangle : public MacroAlgorithm
{
    Q_OBJECT
public:


    MaximalRectangle();
    MaximalRectangle(Floor *top, int width, int height, QWidget *parent = NULL, QString project = "");

    void generateMacro();

    int amountOfWork();
    int area(Coords LL, Coords UR);
    int area(int rect);
    void clearSite();
signals:
    void progessed(int p);
    void syncFaces();
private:
    Floor *_topFloor;
    int _width;
    int _height;
    QWidget *_parent;
    QString _project;


    QVector<Coords> _rectStack;
    QVector<int> _cache;
    Coords _bestLL;
    Coords _bestUR;
    Coords _cursor;
    int _currRectangle;
    bool _plottedSomething;
    bool _topLeft;
    QHash<int, CoordPair> _rectangles;
    QHash<Key, QString> _keys;
    QVector<Key> _commands;
    Key _directions[3][3];

    void initDirs();
    void initKeys();
    void plotSite();
    void plotFloor(Key d, Floor *f);
    void plotRectangle(Floor *f);
    void mainAlgorithm(Floor *f, Key d);
    void updateCache(Floor *f, int x, Key d);
    Coords findStart();
    void generateCommands();
    void generateFloorCommands(Floor *f);
    int findNextRectangle(Floor *f);
    void addNeighbors(Floor *f, QQueue<Coords> &bfsQ, Coords s, bool marking);
    void counterBfs(Floor *f);
    Coords findNearestCorner(int rect);
    int CoordsDistance(Coords c1, Coords c2);
    int sign(int x);
    void moveCursorToPoint(Coords p);
    Coords calcMoveCoef(Coords dest, Coords start);
    Coords findOpositeCorner(int rect, Coords currCorner);
    void markRectMacroed(Floor *f, int rect);
    void counterBfs2(Floor *f);

    //int isNewRectangle(Floor *f, Coords subjectCoords);
};

#endif // MAXIMALRECTANGLE_H
