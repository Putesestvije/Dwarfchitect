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
        dx = abs(leftAndRight.second.x - leftAndRight.first.x);
        dy = abs(leftAndRight.second.y - leftAndRight.first.y);

        /*std::cout << "first and second" << std::endl;

        std::cout << leftAndRight.first << std::endl;
        std::cout << leftAndRight.second << std::endl;
        */

        if (dx == 0){
            std::cout << "draw  vertical line" << std::endl;
            drawVerticalLine(leftAndRight);
        } else if (dy == 0){
            std::cout << "drawHorizontalLine" << std::endl;
            drawHorizontalLine(leftAndRight);
        } else if (dx > dy){
            std::cout << "drawing lower slope" << std::endl;
            drawLowerSlope(dx, dy, leftAndRight);
        } else if (dx <= dy){
            std::cout << "drawing higher slope" << std::endl;
            drawHigherSlope(dx, dy, leftAndRight);
        }
        unmarkPrevious();

        for (auto c : _underConstruction){
            markWithBrush(c);
        }
       // std::cout << "end of marking" << std::endl;
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
            int adjX = _brush[i][j].y + c.y;
            int adjY = _brush[i][j].x + c.x;
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
                    (*_faces)[adjY][adjX]->setTempDesignation(_CurrentDesignation);
                    Coords a(adjX, adjY);
                    _pending.push_back(a);
                }
            }
        }
}

void Picker::markWithBrush(Coords c)
{
    for(int i = 0; i < _brush.size(); i++)
        for(int j = 0; j < _brush.size(); j++){
            int adjY = _brush[i][j].y + c.y;
            int adjX = _brush[i][j].x + c.x;
            if((adjY >= _height/12) || (adjX >= _width/12) ||
                    (adjY < 0) || (adjX < 0))
                continue;
            else {
                if((*_faces)[adjY][adjX]->tempDesignation() != _CurrentDesignation){
                    /* the following line forces the tileface to repaint itself,
                     * otherwise it would repaint only after the cursor leaves the
                     * window */
                    (*_faces)[adjY][adjX]->setTempDesignation(_CurrentDesignation);
                    (*_faces)[adjY][adjX]->setUnderConstruction(true);
                    if(_CurrentDesignation == D_CLEAR)
                        (*_faces)[c.x][c.y]->setColor(255, 255, 255);
                    else
                        (*_faces)[c.x][c.y]->setColor(128, 128, 0);
                    Coords a(adjX, adjY);
                   // std::cout << a << std::endl;
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
    if (_fixedPoint.x < _mobilePoint.x){
        std::cout << "mob :" << _mobilePoint << std::endl;

        res.first = _fixedPoint;
        res.second = _mobilePoint;
        /*std::cout << "ordered first :" << res.first << std::endl;
        std::cout << "ordered second :" << res.second << std::endl;*/
        return res;
    } else {

        std::cout << "mob :" << _mobilePoint << std::endl;
        res.first = _mobilePoint;
        res.second = _fixedPoint;
        return res;
    }
}

void Picker::drawVerticalLine(CoordPair &ends)
{
    Coords upper, lower;

    if(ends.first.y > ends.second.y){
        upper = ends.first;
        lower = ends.second;
    } else {
        upper = ends.second;
        lower = ends.first;
    }

    _underConstruction.clear();

    Coords c = lower;

    for(int i = lower.y; i <= upper.y; i++){
        c.y = i;
        //std::cout << c << std::endl;
        _underConstruction.push_back(c);
    }
}

void Picker::drawHorizontalLine(CoordPair &ends)
{
    _underConstruction.clear();
    Coords c = ends.first;
    for(int i = ends.first.x; i <= ends.second.x; i++){
        c.x = i;
        //std::cout << c << std::endl;
        _underConstruction.push_back(c);
    }
}

void Picker::drawLowerSlope(int dx, int dy, CoordPair &ends)
{
    _underConstruction.clear();

    int D = 2*dy - dx;

    int y = ends.first.y;

    for (int x = ends.first.x; x <= ends.second.x; x++){
        Coords c = Coords(y,x);
        //std::cout << c << std::endl;
        _underConstruction.push_back(c);
        if(D > 0){
            if (ends.second.y > ends.first.y)
                y++;
            else
                y--;
            D = D - 2*dx;
        }
        D = D + 2*dy;
    }
}

void Picker::drawHigherSlope(int dx, int dy, CoordPair &ends)
{
    _underConstruction.clear();

    int D = 2*dx - dy;

    int x = ends.first.x;

    if (ends.second.y > ends.first.y){
        for (int y = ends.first.y; y <= ends.second.y; y++){
            Coords c = Coords(y,x);
            //std::cout << c << std::endl;
            _underConstruction.push_back(c);
            if(D > 0){
                x++;
                D = D - 2*dy;
            }
            D = D + 2*dx;
        }
    } else {
        for (int y = ends.first.y; y >= ends.second.y; y--){
            Coords c = Coords(y,x);
            //std::cout << c << std::endl;
            _underConstruction.push_back(c);
            if(D > 0){
                x++;
                D = D - 2*dy;
            }
            D = D + 2*dx;
        }
    }
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

    std::cout << "fix :" << _fixedPoint << " mob :" << _mobilePoint << std::endl;

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
            (*_faces)[c.x][c.y]->setCurrentDesignation(_CurrentDesignation);
            (*_faces)[c.x][c.y]->setTempDesignation((*_faces)[c.x][c.y]->currentDesignation());
            if(_CurrentDesignation == D_CLEAR)
                (*_faces)[c.x][c.y]->setColor(255, 255, 255);
            else
                (*_faces)[c.x][c.y]->setColor(128, 128, 0);
            (*_faces)[c.x][c.y]->setUnderConstruction(false);
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
    position.append(QString::number(adjY));
    position.append(",");
    position.append(QString::number(adjX));
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
    position.append(QString::number(adjY));
    position.append(",");
    position.append(QString::number(adjX));
    position.append(")");

    emit mousePosition(position);
}
