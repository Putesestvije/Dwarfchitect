#include "picker.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <iostream>
#include <QSet>
#include <QKeyEvent>
#include <QGraphicsView>

Picker::Picker()
{

}

Picker::Picker(int width, int height, std::vector<std::vector<TileFace *> > *faces)
    : _width(width),
      _height(height),
      _faces(faces)
{
    setZValue(3);
    setFlags(ItemIsSelectable);
    _CurrentDesignation = D_DIG;
    setAcceptHoverEvents(true);

    //_pending = new QVector<Coords>;
}

Picker::~Picker()
{
    delete _faces;
}

void Picker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    //Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->drawRect(boundingRect());
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

void Picker::newFloor(Floor *f)
{
    _currentFloor = f;
    _currentFloor->synchronizeFaces(_faces);
}

void Picker::setCurrentDesignation(int k)
{
    _CurrentDesignation = static_cast<Key>(k);

    std::cout << "designation changed to " << k << std::endl;
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
    if(_pending.size() > 0){
        _currentFloor->applyChanges(_pending, _CurrentDesignation);
        _pending.clear();
        emit changesMadeToModel();
    }
}

void Picker::keyPressEvent(QKeyEvent *event)
{
    int k = event->key();
    Qt::KeyboardModifiers q = event->modifiers();
    if(k == Qt::Key_Z && q == Qt::ControlModifier)
        emit undo();
    else if ((k == Qt::Key_Z && q == (Qt::ControlModifier | Qt::ShiftModifier))
             || (k == Qt::Key_Y && q == Qt::ControlModifier))
        emit redo();
    else
        QGraphicsItem::keyPressEvent(event);
}

void Picker::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QString position("");
    int xPos = event->pos().x();
    int yPos = event->pos().y();

    /* adjusted X and Y coordinates so you can use them
     * as array indices*/
    int adjX = xPos/12;
    int adjY = yPos/12;
    position.append("(");
    position.append(QString::number(adjX));
    position.append(",");
    position.append(QString::number(adjY));
    position.append(")");

    emit mousePosition(position);
}

void Picker::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QString position("");
    int xPos = event->pos().x();
    int yPos = event->pos().y();

    /* adjusted X and Y coordinates so you can use them
     * as array indices*/
    int adjX = xPos/12;
    int adjY = yPos/12;
    position.append("(");
    position.append(QString::number(adjX));
    position.append(",");
    position.append(QString::number(adjY));
    position.append(")");

    emit mousePosition(position);
}
