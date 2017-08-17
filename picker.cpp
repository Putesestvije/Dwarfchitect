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

    //std::cout << "designation changed to " << k << std::endl;
}

void Picker::setDrawMode(int d)
{
    _drawMode = static_cast<DrawMode>(d);

    //std::cout << "draw mode changed to " << d << std::endl;
}

void Picker::setBrushType(int b)
{
    _brushType = static_cast<BrushType>(b);
}

void Picker::setupBrush(int size)
{
    //std::cout << "given size " << size << std::endl;

    _brush.resize(size);
    for (int i = 0; i < size; i++)
        _brush[i].resize(size);

    if (_brushType == B_SQUARE)
        setupSquareBrush();
    else if(_brushType == B_CIRCLE)
        setupSquareBrush();
}

void Picker::drawFreeHand(QGraphicsSceneMouseEvent *event)
{
    int xPos = event->pos().x();
    int yPos = event->pos().y();

    /* adjusted X and Y coordinates so you can use them
     * as array indices*/
    int adjX = xPos/12;
    int adjY = yPos/12;

    if((xPos >= _width) || (yPos >= _height) ||(xPos < 0) || (yPos < 0)){
        return;
    }
    Coords c(adjX, adjY);
    applyBrush(c);
}

void Picker::drawLine(QGraphicsSceneMouseEvent *event)
{

}

void Picker::setupSquareBrush()
{
    int s = _brush.size();

    for (int i = 0; i < s; i++){
        for (int j =0; j < s; j++){
            _brush[i][j] = Coords(i - (s/2), j - (s/2));
        }
    }
}

void Picker::setupCircleBrush()
{

}

void Picker::applyBrush(Coords c)
{
    for(int i = 0; i < _brush.size(); i++)
        for(int j = 0; j < _brush.size(); j++){
            int adjY = _brush[i][j].y + c.y;
            int adjX = _brush[i][j].x + c.x;
            if((adjY >= _height/12) || (adjX >= _width/12) ||
                    (adjY < 0) || (adjX < 0))
                continue;
            else {
                if((*_faces)[adjY][adjX]->currentDesignation() != _CurrentDesignation){
                    (*_faces)[adjY][adjX]->setColor(128, 128, 0);
                    (*_faces)[adjY][adjX]->setCurrentDesignation(_CurrentDesignation);
                    Coords a(adjY, adjX);
                    _pending.push_back(a);
                }
            }
        }
}

void Picker::mousePressEvent(QGraphicsSceneMouseEvent *event){

    drawFreeHand(event);
}

void Picker::mouseMoveEvent(QGraphicsSceneMouseEvent *event){

    drawFreeHand(event);
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
