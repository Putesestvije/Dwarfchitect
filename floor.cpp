#include "floor.h"

Floor::Floor()
    : _width(0),
      _height(0),
      _floorAbove(nullptr),
      _floorBelow(nullptr)
{

}

Floor::Floor(int width, int height)
    : _width(width),
      _height(height),
      _floorAbove(nullptr),
      _floorBelow(nullptr)
{
    setupTiles(width, height);
}

Floor::Floor(int width, int height, Floor *above, Floor *below)
    : _width(width),
      _height(height),
      _floorAbove(above),
      _floorBelow(below)
{
    setupTiles(width, height);
}

Floor *Floor::floorAbove() const
{
    return _floorAbove;
}

void Floor::setFloorAbove(Floor *floorAbove)
{
    _floorAbove = floorAbove;
}

Floor *Floor::floorBelow() const
{
    return _floorBelow;
}

void Floor::setFloorBelow(Floor *floorBelow)
{
    _floorBelow = floorBelow;
}

void Floor::undo()
{
    if(_undoQueue.size()>0){
        _redoQueue.push_front(_tiles);
        _tiles = _undoQueue.front();
        _undoQueue.pop_front();
        emit syncRequired();
    }
}

void Floor::redo()
{
    if(_redoQueue.size()>0){
        _undoQueue.push_front(_tiles);
        _tiles = _redoQueue.front();
        _redoQueue.pop_front();
        emit syncRequired();
    }
}

void Floor::applyChanges(QVector<Coords> &changes, Key newDes)
{
    _undoQueue.push_front(_tiles);
    if(_undoQueue.size()>100)
        _undoQueue.pop_back();
    for(auto c : changes){
        _tiles[c.x][c.y].des = newDes;
    }
    _redoQueue.clear();
}

void Floor::synchronizeFaces(std::vector<std::vector<TileFace *> > *faces)
{
    for(int i = 0; i < _height; i++)
        for(int j = 0; j < _width; j++){
            (*faces)[i][j]->setCurrentDesignation(_tiles[i][j].des);
            (*faces)[i][j]->setTempDesignation(_tiles[i][j].des);
            (*faces)[i][j]->setRect(_tiles[i][j].rectangle);

            if(_tiles[i][j].des == D_CLEAR)
                (*faces)[i][j]->setColor(255,255,255);
            else
                (*faces)[i][j]->setColor(128,128,0);
        }
}

void Floor::setupTiles(int width, int height)
{
    _tiles.resize(height);
    for(int i = 0; i < height; i++){
        _tiles[i].resize(width);
    }
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            TileBack t;
            _tiles[i][j] = t;
        }
    }
}

QVector<QVector<TileBack> > &Floor::tiles()
{
    return _tiles;
}
