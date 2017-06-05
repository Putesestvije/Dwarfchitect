#ifndef FLOOR_H
#define FLOOR_H

#include <QObject>
#include <QVector>
#include <queue>
#include <deque>

#include "coords.h"
#include "tileback.h"
#include "tileface.h"
class Floor : public QObject
{
    Q_OBJECT
public:

    Floor();
    Floor(int width, int height);
    Floor(int width, int height, Floor* above, Floor *below);

    Floor *floorAbove() const;
    void setFloorAbove(Floor *floorAbove);

    Floor *floorBelow() const;
    void setFloorBelow(Floor *floorBelow);

    void undo();
    void redo();

    void applyChanges(QVector<Coords> &changes, Designation newDes);
    void synchronizeFaces(QVector<QVector <TileFace*> > *faces);
    QVector<QVector<TileBack> > & tiles();

signals:
    void syncRequired();

private:
    void setupTiles(int width, int height);

    int _width;
    int _height;

    Floor *_floorAbove;
    Floor *_floorBelow;

    QVector<QVector< TileBack > > _tiles;
    std::deque<QVector < QVector <TileBack> > > _undoQueue;
    std::deque<QVector < QVector <TileBack> > > _redoQueue;

};

#endif // FLOOR_H
