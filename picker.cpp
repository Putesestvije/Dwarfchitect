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
    //change the next line or remove it completely later
    UC = Coords(0, 0);

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
    setupBrush();
}

void Picker::setupBrush()
{
    if (_brushType == B_SQUARE)
        setupSquareBrush();
    else if(_brushType == B_CIRCLE)
        setupCircleBrush();
}

void Picker::resizeBrush(int size)
{
    _brush.resize(size);
    for (int i = 0; i < size; i++)
        _brush[i].resize(size);
    //std::cout << "given size " << size << std::endl;
    setupBrush();
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
    int xPos = event->pos().x();
    int yPos = event->pos().y();

    /* adjusted X and Y coordinates so you can use them
     * as array indices*/
    int adjX = xPos/12;
    int adjY = yPos/12;

    if((xPos >= _width) || (yPos >= _height) ||(xPos < 0) || (yPos < 0)){
        return;
    }

    Coords m = Coords(adjY, adjX);

    if(!(m == _mobilePoint)){ /*meaning, if there was movement between tiles */
        _mobilePoint = m;
        CoordPair leftAndRight = orderedEnds();
        int dx, dy;
        dx = abs(leftAndRight.second.y - leftAndRight.first.y);
        dy = abs(leftAndRight.second.x - leftAndRight.first.x);
        if (dx == 0){
            drawVerticalLine(leftAndRight);
        } else if (dy == 0){
            drawHorizontalLine(leftAndRight);
        } else if (dx > dy){

        } else {

        }
        unmarkPrevious();

        for (auto c : _underConstruction){
            markWithBrush(c);
        }
    }
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
    int s = _brush.size();

    //for both the width and the height of the circle
    int center = 100 * s - 1;

    int radiusSqr = ((center-50) * (center-50)) ;

    int tileX, tileY;

    int dx, dy, distSqr;

    for (int i = 0; i < s; i++)
        for (int j = 0; j < s; j++){
            tileX = 100 + 200*j - 1;
            tileY = 100 + 200*i - 1;;
            dx = tileX - center;
            dy = tileY - center;
            distSqr = dx * dx + dy * dy;

            if(distSqr < radiusSqr)
                _brush[i][j] = Coords(i-(s/2), j-(s/2));
            else
                _brush[i][j] = Coords(0, 0);

        }
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
                    /* the following line forces the tileface to repaint itself,
                     * otherwise it would repaint only after the cursor leaves the
                     * window */
                    (*_faces)[adjY][adjX]->setColor(128, 128, 0);
                    (*_faces)[adjY][adjX]->setCurrentDesignation(_CurrentDesignation);
                    Coords a(adjY, adjX);
                    _pending.push_back(a);
                }
            }
        }
}

void Picker::markWithBrush(Coords c)
{
    for(int i = 0; i < _brush.size(); i++)
        for(int j = 0; j < _brush.size(); j++){
            int adjX = _brush[i][j].y + c.y;
            int adjY = _brush[i][j].x + c.x;
            if((adjY >= _height/12) || (adjX >= _width/12) ||
                    (adjY < 0) || (adjX < 0))
                continue;
            else {
                if((*_faces)[adjY][adjX]->tempDesignation() != _CurrentDesignation){
                    /* the following line forces the tileface to repaint itself,
                     * otherwise it would repaint only after the cursor leaves the
                     * window */
                    (*_faces)[adjY][adjX]->setColor(128, 128, 0);
                    (*_faces)[adjY][adjX]->setTempDesignation(_CurrentDesignation);
                    (*_faces)[adjY][adjX]->setUnderConstruction(true);
                    /*uncomment and change later when you add line drawing
                    _underConstruction.push_back(a);
                    */
                    Coords a(adjY, adjX);
                    _marked.push_back(a);
                }
            }
        }
}

void Picker::unmarkPrevious()
{
    for (auto c : _marked){
        (*_faces)[c.x][c.y]->setTempDesignation((*_faces)[c.x][c.y]->currentDesignation());
        (*_faces)[c.x][c.y]->setUnderConstruction(false);
    }
    _marked.clear();
}

CoordPair Picker::orderedEnds()
{
    CoordPair res;
    if (_fixedPoint.y < _mobilePoint.y){
        res.first = _fixedPoint;
        res.second = _mobilePoint;
        return res;
    } else {
        res.first = _mobilePoint;
        res.second = _fixedPoint;
        return res;
    }
}

void Picker::drawVerticalLine(CoordPair &ends)
{
    Coords upper, lower;

    if(ends.first.x > ends.second.x){
        upper = ends.first;
        lower = ends.second;
    } else {
        upper = ends.second;
        lower = ends.first;
    }

    _underConstruction.clear();

    Coords c = lower;

    for(int i = lower.x; i <= upper.x; i++){
        c.x = i;
        _underConstruction.push_back(c);
    }
}

void Picker::drawHorizontalLine(CoordPair &ends)
{
    _underConstruction.clear();
    Coords c = ends.first;
    for(int i = ends.first.y; i <= ends.second.y; i++){
        c.y = i;
        _underConstruction.push_back(c);
    }
}

void Picker::drawLowerSlope(int dx, int dy, CoordPair &ends)
{
    double k = dy/(dx*1.0);

    _underConstruction.clear();

    int x = ends.first.y;
    int y = ends.first.x;
}

void Picker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    int xPos = event->pos().x();
    int yPos = event->pos().y();

    /* adjusted X and Y coordinates so you can use them
     * as array indices*/
    int adjX = xPos/12;
    int adjY = yPos/12;

    _fixedPoint = Coords(adjY, adjX);
    _mobilePoint = Coords(adjY, adjX);

    switch (_drawMode){
    case M_FREEHAND :
        drawFreeHand(event);
        break;
    case M_LINE :
        _underConstruction.clear();
        drawLine(event);
        break;
    }
}

void Picker::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    switch (_drawMode){
    case M_FREEHAND :
        drawFreeHand(event);
        break;
    case M_LINE :
        drawLine(event);
        break;
    }
}

void Picker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    if(_pending.size() > 0){
        _currentFloor->applyChanges(_pending, _CurrentDesignation);
        _pending.clear();
        emit changesMadeToModel();
    } else if(_marked.size() > 0){
        _currentFloor->applyChanges(_marked, _CurrentDesignation);
        for (auto c : _marked){
            (*_faces)[c.x][c.y]->setTempDesignation((*_faces)[c.x][c.y]->currentDesignation());
            (*_faces)[c.x][c.y]->setUnderConstruction(false);
            (*_faces)[c.x][c.y]->setCurrentDesignation(_CurrentDesignation);
        }
        _marked.clear();
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
