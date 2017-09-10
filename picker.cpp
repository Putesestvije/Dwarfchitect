#include "picker.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <iostream>
#include <QSet>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QMessageBox>

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

    _hasStarter = false;

    _starterTile = Coords(-1, -1);
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
            //std::cout << "draw  vertical line" << std::endl;
            drawVerticalLine(leftAndRight);
        } else if (dy == 0){
            //std::cout << "drawHorizontalLine" << std::endl;
            drawHorizontalLine(leftAndRight);
        } else if (dx > dy){
            //std::cout << "drawing lower slope" << std::endl;
            drawLowerSlope(dx, dy, leftAndRight);
        } else if (dx <= dy){
            //std::cout << "drawing higher slope" << std::endl;
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
        //std::cout << "mob :" << _mobilePoint << std::endl;

        res.first = _fixedPoint;
        res.second = _mobilePoint;
        /*std::cout << "ordered first :" << res.first << std::endl;
        std::cout << "ordered second :" << res.second << std::endl;*/
        return res;
    } else {

        //std::cout << "mob :" << _mobilePoint << std::endl;
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

void Picker::drawEllipse(QGraphicsSceneMouseEvent *event)
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
        _underConstruction.clear();
        _mobilePoint = m;
        CoordPair leftAndRight = orderedEnds();
        int dx, dy;
        dx = abs(leftAndRight.second.x - leftAndRight.first.x);
        dy = abs(leftAndRight.second.y - leftAndRight.first.y);

        int a = dx/2 + (dx%2);
        int b = dy/2 + (dy%2);

        int x = 0;
        int y = b;

        int D1 = 4*(b*b - a*a*b) + a*a;
        //double D1 = b*b * a*a*b + a*a/4.0;
        int adx = leftAndRight.second.x + leftAndRight.first.x;
        int ady = leftAndRight.second.y + leftAndRight.first.y;
        int x0 = adx/2 + adx%2;
        int y0 = ady/2 + ady%2;

        Coords center = Coords(y0, x0);

        //std::cout << "first :" << leftAndRight.first << std::endl;
        //std::cout << "second :" << leftAndRight.second << std::endl;
        //std::cout << "center :" << center << std::endl;

        QVector<Coords> fQuad;

        Coords c = Coords(y + center.y - dy%2, x + center.x);
        fQuad.push_back(c);
        //fQuad.push_back(center);
        c = Coords(center.y, center.x + a - dx %2);
        //fQuad.push_back(c);
        //std::cout << "dx :" << dx << " dy :" << dy << std::endl;
        //std::cout << "a :" << a << " b :" << b << " top :" << y + center.y - dy%2 <<  std::endl;

        while (a * a * 2 * (2 * y - 1) > 4 * b * b * (x + 1)){
            if (D1 < 0){
                D1 = D1 + 4 * b*b * (2*x + 3);
                x++;
            } else {
                D1 = D1 + 4 * (b*b *(2*x + 3) + a*a * (-2*y + 2)) + b*b;
                x++;
                y--;
            }
            c = Coords(y+center.y - dy%2, x + center.x - dx%2);
            fQuad.push_back(c);
        }

        int D2 = 4*(b*b*(x*x+x-a*a) + a*a*(y*y-2*y+1)) + b*b;
        while (y > 0){
            if (D2 < 0){
                D2 = D2 + 4*(b*b * (2*x + 2) + a*a * (-2*y + 3));
                x++;
                y--;
            } else {
                D2 = D2 + 4*a*a*(-2*y + 3);
                y--;
            }
            c = Coords(y + center.y - dy%2, x + center.x - dx%2);
            fQuad.push_back(c);
        }

        plotOtherQuadrants(fQuad, dx, dy, center);

        unmarkPrevious();

        for (auto c : _underConstruction){
            markWithBrush(c);
        }
       // std::cout << "end of marking" << std::endl;
    }
}

void Picker::plotOtherQuadrants(QVector<Coords> &fQuad, int &dx, int &dy, Coords &center)
{
    int cx = center.x;
    int cy = center.y;
    Coords c;
    for (int i = 0; i < fQuad.size(); i++){
        _underConstruction.push_back(fQuad[i]);
        c = Coords(fQuad[i].y, fQuad[i].x - 2*(fQuad[i].x-cx) - dx % 2);
        _underConstruction.push_back(c);
        c = Coords(fQuad[i].y - 2*(fQuad[i].y-cy) - dy % 2, fQuad[i].x - 2*(fQuad[i].x-cx) - dx % 2);
        _underConstruction.push_back(c);
        c = Coords(fQuad[i].y - 2*(fQuad[i].y-cy) - dy % 2, fQuad[i].x);
        _underConstruction.push_back(c);
    }
}

void Picker::drawRectangle(QGraphicsSceneMouseEvent *event)
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
    int left, right, top, bottom;
    Coords m = Coords(adjY, adjX);

    if(!(m == _mobilePoint)){ /*meaning, if there was movement between tiles */
        _underConstruction.clear();
        _mobilePoint = m;
        CoordPair leftAndRight = orderedEnds();
        left = leftAndRight.first.x;
        right = leftAndRight.second.x;
        if(leftAndRight.first.y > leftAndRight.second.y){
            top = leftAndRight.first.y;
            bottom = leftAndRight.second.y;
        } else {
            top = leftAndRight.second.y;
            bottom = leftAndRight.first.y;
        }
        Coords c;
        for(int i = left; i <= right; i++){
            c = Coords(top, i);
            _underConstruction.push_back(c);
            c = Coords(bottom, i);
            _underConstruction.push_back(c);
        }
        for (int i = bottom; i <= top; i++){
            c = Coords(i, left);
            _underConstruction.push_back(c);
            c = Coords(i, right);
            _underConstruction.push_back(c);
        }

        unmarkPrevious();

        for (auto c : _underConstruction){
            markWithBrush(c);
        }
    }

}

void Picker::paintBucket(QGraphicsSceneMouseEvent *event)
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
    QVector<Coords> neighbors;
    Coords n = Coords(adjY, adjX);
    Coords c;
    Key toBePaintedOver = (*_faces)[adjY][adjX]->currentDesignation();
    if (toBePaintedOver == _CurrentDesignation)
        return;

    neighbors.push_back(n);
    while (neighbors.size() > 0){
        n = neighbors.takeFirst();
        _pending.push_back(Coords(n.x, n.y));
        (*_faces)[n.y][n.x]->setCurrentDesignation(_CurrentDesignation);
        if(n.y+1 < _faces->size())
            if((*_faces)[n.y+1][n.x]->currentDesignation() == toBePaintedOver){
                c = Coords(n.y+1, n.x);
                (*_faces)[n.y+1][n.x]->setCurrentDesignation(_CurrentDesignation);
                (*_faces)[n.y+1][n.x]->setColor(128, 128, 0);
                neighbors.push_back(c);
            }
        if(n.y-1 >= 0)
            if((*_faces)[n.y-1][n.x]->currentDesignation() == toBePaintedOver){
                c = Coords(n.y-1, n.x);
                (*_faces)[n.y-1][n.x]->setCurrentDesignation(_CurrentDesignation);
                (*_faces)[n.y-1][n.x]->setColor(128, 128, 0);
                neighbors.push_back(c);
            }
        if(n.x+1 < (*_faces)[0].size())
            if((*_faces)[n.y][n.x+1]->currentDesignation() == toBePaintedOver){
                c = Coords(n.y, n.x+1);
                (*_faces)[n.y][n.x+1]->setCurrentDesignation(_CurrentDesignation);
                (*_faces)[n.y][n.x+1]->setColor(128, 128, 0);
                neighbors.push_back(c);
            }
        if(n.x-1 >= 0)
            if((*_faces)[n.y][n.x-1]->currentDesignation() == toBePaintedOver){
                c = Coords(n.y, n.x-1);
                (*_faces)[n.y][n.x-1]->setCurrentDesignation(_CurrentDesignation);
                (*_faces)[n.y][n.x-1]->setColor(128, 128, 0);
                neighbors.push_back(c);
            }
    }
}

bool Picker::hasStarter() const
{
    return _hasStarter;
}

void Picker::setHasStarter(bool hasStarter)
{
    _hasStarter = hasStarter;
}

Coords Picker::starterTile() const
{
    return _starterTile;
}

void Picker::setStarterTile(const Coords &starterTile)
{
    _starterTile = starterTile;
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

    //std::cout << "fix :" << _fixedPoint << " mob :" << _mobilePoint << std::endl;

    if(_CurrentDesignation == D_START){
        if (_currentFloor->floorAbove() != nullptr){
            QMessageBox msg;
            msg.setIcon(QMessageBox::Warning);
            msg.setText("Starter tile can only be placed on the top floor.");
            msg.exec();
            return;
        }
        if (_hasStarter){
            (*_faces)[_starterTile.y][_starterTile.x]->setStarter(false);
            (*_faces)[_starterTile.y][_starterTile.x]->setColor(128, 128, 0);
            _starterTile = Coords(adjY, adjX);
        }
        _hasStarter = true;
        _starterTile = Coords(adjY, adjX);
        (*_faces)[adjY][adjX]->setStarter(true);
        (*_faces)[adjY][adjX]->setColor(0, 255, 0);
    } else {
            switch (_drawMode){
        case M_FREEHAND :
            drawFreeHand(event);
            break;
        case M_LINE :
            _underConstruction.clear();
            drawLine(event);
            break;
        case M_ELLIPSE :
            _underConstruction.clear();
            drawEllipse(event);
            break;
        case M_RECT :
            _underConstruction.clear();
            drawRectangle(event);
            break;
        case M_FILL :
            paintBucket(event);
            break;
        default :
            break;
        }
    }
}

void Picker::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(_CurrentDesignation == D_START)
        return;
    switch (_drawMode){
    case M_FREEHAND :
        drawFreeHand(event);
        break;
    case M_LINE :
        drawLine(event);
        break;
    case M_ELLIPSE :
        _underConstruction.clear();
        drawEllipse(event);
        break;
    case M_RECT :
        _underConstruction.clear();
        drawRectangle(event);
        break;
    default :
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
