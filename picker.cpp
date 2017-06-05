#include "picker.h"
#include <QGraphicsSceneMouseEvent>
#include <iostream>
#include <QSet>

Picker::Picker()
{

}

Picker::Picker(int width, int height, QVector<QVector<TileFace *> > *faces)
    : _width(width),
      _height(height),
      _faces(faces)
{
    setZValue(3);
    setFlags(ItemIsSelectable);
    _CurrentDesignation = DIG;
    //_pending = new QVector<Coords>;
}

void Picker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

QRectF Picker::boundingRect() const {
    return QRectF(0, 0, _width, _height);
}

Floor *Picker::currentFloor() const
{
    return _currentFloor;
}

void Picker::setCurrentFloor(Floor *currentFloor)
{
    _currentFloor = currentFloor;
}

void Picker::sync()
{
    _currentFloor->synchronizeFaces(_faces);
}

void Picker::mousePressEvent(QGraphicsSceneMouseEvent *event){

    int xPos = event->pos().x();
    int yPos = event->pos().y();

    /* adjusted X and Y coordinates so you can use them
     * as array indices*/
    int adjX = xPos/12;
    int adjY = yPos/12;

    if((xPos >= _width) || (yPos >= _height) ||(xPos < 0) || (yPos < 0)){
        return;
    }
    if((*_faces)[adjY][adjX]->currentDesigantion() != _CurrentDesignation){
        (*_faces)[adjY][adjX]->setCurrentDesigantion(_CurrentDesignation);
        (*_faces)[adjY][adjX]->setColor(128, 128, 0);
        Coords c(adjY, adjX);
        _pending.push_back(c);
    }

}

void Picker::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    int xPos = event->pos().x();
    int yPos = event->pos().y();

    /* adjusted X and Y coordinates so you can use them
     * as array indices*/
    int adjX = xPos/12;
    int adjY = yPos/12;


    if((xPos >= _width) || (yPos >= _height) ||(xPos < 0) || (yPos < 0)){
        return;
    }
    if((*_faces)[adjY][adjX]->currentDesigantion() != _CurrentDesignation){
        (*_faces)[adjY][adjX]->setCurrentDesigantion(_CurrentDesignation);
        (*_faces)[adjY][adjX]->setColor(128, 128, 0);
        Coords c(adjY, adjX);
        _pending.push_back(c);
    }
}

void Picker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event);
    _currentFloor->applyChanges(_pending, _CurrentDesignation);
    _pending.clear();
}
