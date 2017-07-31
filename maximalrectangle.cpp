#include <iostream>
#include <QQueue>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QFileInfo>
#include <QString>

#include "maximalrectangle.h"
#include "designations.h"

MaximalRectangle::MaximalRectangle()
{

}

MaximalRectangle::MaximalRectangle(Floor *top, int width, int height, QWidget *parent, QString project)
    : _topFloor(top),
      _width(width),
      _height(height),
      _parent(parent),
      _project(project)

{
    _currRectangle = 1;
    _plottedSomething = true;
    _rectangles.clear();
    initKeys();
    initDirs();
}

void MaximalRectangle::generateMacro()
{
    plotSite();
    generateCommands();
    /*QString currDir = QDir::currentPath();
    std::cout << currDir.toStdString() << std::endl;*/

    QString savePath = QFileDialog::getSaveFileName(_parent, tr("Save File"), _project, "Macro Files (*.mak)", nullptr, 0);
    std::cout << savePath.toStdString() << std::endl;
    savePath.append(".mak");

    QFile file(savePath);
    QFileInfo fi(savePath);
    std::cout << fi.fileName().toStdString() << std::endl;

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox *msg = new QMessageBox(_parent);
        msg->setIcon(QMessageBox::Warning);
        msg->setText("Couldn't Open the File!");
        delete msg;
        return;
    }

    QTextStream out(&file);
    out << fi.fileName() << '\n';
    for (int i = 0; i < _commands.size(); i++){
        out << '\t' << '\t' << _keys[_commands[i]] << '\n';
        out << '\t' << _keys[EOG] << '\n';;
    }
    out << _keys[EOM] << '\n';

    file.close();
}

int MaximalRectangle::amountOfWork()
{
    Floor *c = _topFloor;
    int work = 0;
    while(c != nullptr){
        for(int i = 0; i < _height; i++){
            for(int j = 0; j < _width; j++){
                if(c->tiles()[i][j].des != D_CLEAR)
                    work++;
            }
        }
        c = c->floorBelow();
    }
    return work;
}

void MaximalRectangle::plotSite()
{
    Floor *f = _topFloor;
    int fl = 0;
    while(f != nullptr){
        plotFloor(Key::D_DIG, f);
        plotFloor(Key::D_CHANNEL, f);
        plotFloor(Key::D_UP_STAIR, f);
        plotFloor(Key::D_DOWN_STAIR, f);
        plotFloor(Key::D_UPDOWN_STAIR, f);
        plotFloor(Key::D_RAMP, f);
        f = f->floorBelow();
        std::cout << "on floor " << fl << std::endl;
        fl++;
    }

}

void MaximalRectangle::plotFloor(Key d, Floor *f)
{
    _plottedSomething = true;
    while(_plottedSomething){
        _plottedSomething = false;
        _bestUR = Coords(-1,-1);
        _bestLL = Coords(0,0);
        _cache.clear();
        for(int i = 0; i <= _height; i++)
            _cache.push_back(0);
        mainAlgorithm(f, d);
        /* after the main algorithm the _bestLL and _bestUR fields
         * should be refering to opposite corners of the largest rectangle
         * to be marked up */
        _rectangles[_currRectangle] = CoordPair(_bestLL, _bestUR);


        std::cout.flush();
        int a = area(_bestLL, _bestUR);
        std::cout << "plotted rect " << _currRectangle << "with area " << a << std::endl;
        emit progessed(a);

        std::cout << "found something at (" << _bestLL.x
                  << "," << _bestLL.y << "), (" << _bestUR.x
                  << "," << _bestUR.y << ")"
                  << "rectangle #" <<_currRectangle << std::endl;

        if(_plottedSomething)
            plotRectangle(f);
        emit syncFaces();
    }
}

void MaximalRectangle::plotRectangle(Floor *f)
{
    for (int i = _bestLL.x; i <= _bestUR.x; i++)
        for (int j = _bestLL.y; j <= _bestUR.y; j++)
            f->tiles()[i][j].rectangle = _currRectangle;

    _currRectangle++;
}

void MaximalRectangle::mainAlgorithm(Floor *f, Key d)
{
    int widestRect;
    Coords p;
    _rectStack.clear();
    for (int x = _width-1; x > -1; x--){
        updateCache(f, x, d);
        widestRect = 0;
        for (int y = 0; y <= _height; y++){
            /*placing something on the rectangle stack*/
            if (_cache[y] > widestRect){
                Coords c(y, widestRect);
                _rectStack.push_back(c);
                widestRect = _cache[y];
            }
            /*closing the rectangle stack, possibly found a better one */
            if (_cache[y] < widestRect){
                do {
                    p = _rectStack.takeLast();
                    if ((widestRect * (y-p.x)) > area(_bestLL, _bestUR)){
                        _bestLL = Coords(p.x, x);
                        _bestUR = Coords(y-1, x+widestRect-1);
                    }
                    widestRect = p.y;
                } while (_cache[y] < widestRect);
                widestRect = _cache[y];
                if(widestRect != 0){
                    Coords c(p.x, p.y);
                    _rectStack.push_back(c);
                }
            }
        }
    }
}

void MaximalRectangle::updateCache(Floor *f, int x, Key d)
{
    for (int i = 0; i < _height; i++){
        if((f->tiles()[i][x].des == d) && (!f->tiles()[i][x].rectangle)){
            _cache[i]++;
            _plottedSomething = true;
        }
        else
            _cache[i] = 0;
    }
}

Coords MaximalRectangle::findStart()
{
    Floor *f = _topFloor;
    while (f != nullptr){
        for (int i = 0; i < _height; i++)
            for (int j = 0; j < _width; j++)
                if(f->tiles()[i][j].des != D_CLEAR)
                    return Coords(i,j);
    }
    return Coords(); /*shouldn't reach this but the compiler warns me since the other return is in a loop*/
}

void MaximalRectangle::generateCommands()
{
    _cursor = findStart();
    Floor *f = _topFloor;
    while (f != nullptr){
        std::cout << "cursor starting at " << _cursor << std::endl;
        generateFloorCommands(f);
        if(f->floorBelow() != nullptr)
            _commands.push_back(DOWN_Z);
        //std::cout << "Generate Commands" << std::endl;
        f = f->floorBelow();
    }
}

void MaximalRectangle::generateFloorCommands(Floor *f)
{
    int rect;
    while (true) {
        std::cout << "Generate Floor Commands" << std::endl;
        rect = findNextRectangle(f);
        std::cout << "rect = " << rect << std::endl;
        if(!rect)
            return;
        Coords nearestCorner = findNearestCorner(rect); /*As in, nearest to the cursor*/
        moveCursorToPoint(nearestCorner);
        _commands.append(f->tiles()[nearestCorner.x][nearestCorner.y].des);
        _commands.append(SELECT);
        Coords opositeCorner = findOpositeCorner(rect, _cursor);
        moveCursorToPoint(opositeCorner);
        _commands.append(SELECT);
        markRectMacroed(f, rect);
        int a = area(rect);
        std::cout << "marked rect " << rect << " with area " << a << std::endl;
        emit progessed(a);
    }
    /*
     * ponavljaj
            nadji sledeci pravougaonik
                uradi bfs
                    stavi cursor u red
                    ponavljaj dok se ne isprazni red
                        uzmi sa vrha
                        proveri da li valja
                            ako valja
                            isprazni red
                            oslobodi bfs
                                vrati pravougaonik
                        dodaj njegove susede
                    vrati prazan
            ako nisi nasao
                izadji
            nadji najblizi cosak
            idi ka njemu
            markiraj pravougaonik
                konkretno markiraj da je makroisan
                posalji napredak
    */
}

int MaximalRectangle::findNextRectangle(Floor *f)
{    
    int rect = 0;
    QQueue<Coords> bfsQ;
    Coords c = _cursor;
    f->tiles()[c.x][c.y].bfs = true;
    bfsQ.enqueue(c);
    while (!bfsQ.empty()){
        c = bfsQ.dequeue();
        if((f->tiles()[c.x][c.y].rectangle) && !(f->tiles()[c.x][c.y].macroed)){
            rect = f->tiles()[c.x][c.y].rectangle;
            counterBfs(f);
            //counterBfs2(f);
            return rect;
        }
        addNeighbors(f, bfsQ, c, true);
    }
    return rect;
}


/* holy hell is this a dumb function but I'm so tired
 * and I can't think of anything better right now */
void MaximalRectangle::addNeighbors(Floor *f, QQueue<Coords> &bfsQ, Coords s, bool marking)
{
    for(int i = -1; i < 2; i++){
        for(int j = -1; j < 2; j++){
            if( (s.x+i >= 0) && (s.y+j >= 0) && (s.x+i < _height) && (s.y+j < _width)){
                if(marking){
                    if(!(f->tiles()[s.x+i][s.y+j].bfs)){
                        f->tiles()[s.x+i][s.y+j].bfs = true;
                        bfsQ.enqueue(Coords(s.x+i, s.y+j));
                    }
                } else {/* this branch is used by the counterBfs function*/
                    if (f->tiles()[s.x+i][s.y+j].bfs){
                        f->tiles()[s.x+i][s.y+j].bfs = false;
                        bfsQ.enqueue(Coords(s.x+i, s.y+j));
                    }
                }
            }
        }
    }
}

/* doing the bfs until a rectangle is found leaves an
 * area of tiles that were marked as used in the bfs,
 * so to be able to bfs on those tiles again the
 * algorithm must first mark them unused */
void MaximalRectangle::counterBfs(Floor *f)
{
    QQueue<Coords> counterQ;
    f->tiles()[_cursor.x][_cursor.y].bfs = false;
    counterQ.enqueue(_cursor);
    while(!counterQ.empty()){
        Coords c = counterQ.dequeue();
        addNeighbors(f, counterQ, c, false);
    }
}

Coords MaximalRectangle::findNearestCorner(int rect)
{
    CoordPair cp = _rectangles[rect];
    Coords nearest, a, b, c, d;
    a = cp.first;
    b = cp.second;
    c = Coords(a.x, b.y);
    d = Coords(b.x, a.y);
    nearest = a;
    if (CoordsDistance(_cursor, b) < CoordsDistance(_cursor, nearest))
        nearest = b;
    if (CoordsDistance(_cursor, c) < CoordsDistance(_cursor, nearest))
        nearest = c;
    if (CoordsDistance(_cursor, d) < CoordsDistance(_cursor, nearest))
        nearest = d;

    return nearest;
}

int MaximalRectangle::CoordsDistance(Coords c1, Coords c2)
{
    int dist = 0;
    int coef1, coef2;
    coef1 = sign(c1.x - c2.x);
    coef2 = sign(c1.y - c2.y);

    Coords diff = Coords(coef1, coef2);
    Coords temp = c2; /*I'm not sure why but I don't wanna change the c2 */
    while ((temp < c1) || (c1 < temp)) {
        temp = temp + diff;
        if(temp.x == c1.x)
            diff.x = 0;
        if(temp.y == c1.y)
            diff.y = 0;
        dist++;
    }
    return dist;
}

int MaximalRectangle::sign(int x)
{
    if (x < 0)
        return -1;
    else if (x > 0)
        return 1;
    else
        return 0;
}

/* oddly similar to the distance function, but not quite
 * maybe Ill fuse them later on, but this is better for
 * algorithm clarity I think*/

void MaximalRectangle::moveCursorToPoint(Coords p)
{
    Coords diff = calcMoveCoef(p, _cursor);
    Coords newDiff;
    Key dir;
    dir = _directions[diff.x+1][diff.y+1];
    while ((_cursor < p) || (p < _cursor)){
        _commands.append(dir);
        _cursor = _cursor + diff;
        newDiff = calcMoveCoef(p, _cursor);
        if(!(diff == newDiff)){
            diff = newDiff;
            dir = _directions[diff.x+1][diff.y+1];
        }
    }
}

Coords MaximalRectangle::calcMoveCoef(Coords dest, Coords start)
{
    int coef1, coef2;
    coef1 = sign(dest.x - start.x);
    coef2 = sign(dest.y - start.y);
    Coords diff(coef1, coef2);
    return diff;
}



Coords MaximalRectangle::findOpositeCorner(int rect, Coords currCorner)
{
    CoordPair originals = _rectangles[rect];

    Coords oposite;

    if(currCorner.x == originals.first.x)
        oposite.x = originals.second.x;
    else
        oposite.x = originals.first.x;

    if (currCorner.y == originals.first.y)
        oposite.y = originals.second.y;
    else
        oposite.y = originals.first.y;

    return oposite;
}

void MaximalRectangle::markRectMacroed(Floor *f, int rect)
{
    CoordPair cp = _rectangles[rect];
    for (int i = cp.first.x; i <= cp.second.x; i++)
        for (int j = cp. first.y; j <= cp.second.y; j++)
            f->tiles()[i][j].macroed = true;

}

void MaximalRectangle::counterBfs2(Floor *f)
{
    for(int i = 0; i < _height; i++)
        for(int j = 0; j < _width; j++)
            f->tiles()[i][j].bfs = false;
}




/* This is used to calculate area generally, the only time
 * it shoud return 0 is in the main algorithm when the
 * UR is (-1,-1) */
int MaximalRectangle::area(Coords LL, Coords UR)
{
    if(UR.x == -1 || UR.y == -1)
        return 0;
    return (abs(LL.x - UR.x) + 1)*(abs(LL.y - UR.y) + 1);
}

int MaximalRectangle::area(int rect)
{
    CoordPair c = _rectangles[rect];
    return area(c.first, c.second);
}


/* Clears the site of traces of the algorithm's work like
 * setting the tile's containing rectangle and so on.*/
void MaximalRectangle::clearSite()
{
    Floor *c = _topFloor;
    while(c != nullptr){
        for(int i = 0; i < _height; i++){
            for(int j = 0; j < _width; j++){
                c->tiles()[i][j].rectangle = 0;
                c->tiles()[i][j].macroed = false;
                c->tiles()[i][j].bfs = false;
            }
        }
        c = c->floorBelow();
    }
}

void MaximalRectangle::initDirs()
{
    /*for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++){
            _directions[i][j] = (Key)(i*3 + j);
        }
    */
    _directions[0][0] = UP_LEFT;
    _directions[0][1] = UP;
    _directions[0][2] = UP_RIGHT;
    _directions[1][0] = LEFT;
    _directions[1][1] = D_CLEAR;
    _directions[1][2] = RIGHT;
    _directions[2][0] = DOWN_LEFT;
    _directions[2][1] = DOWN;
    _directions[2][2] = DOWN_RIGHT;

}

void MaximalRectangle::initKeys()
{
    _keys[UP_LEFT] = QString("CURSOR_UPLEFT");
    _keys[UP] = QString("CURSOR_UP");
    _keys[UP_RIGHT] = QString("CURSOR_UPRIGHT");
    _keys[RIGHT] = QString("CURSOR_RIGHT");
    _keys[DOWN_RIGHT] = QString("CURSOR_DOWNRIGHT");
    _keys[DOWN] = QString("CURSOR_DOWN");
    _keys[DOWN_LEFT] = QString("CURSOR_DOWNLEFT");
    _keys[LEFT] = QString("CURSOR_LEFT");
    _keys[D_DIG] = QString("DESIGNATE_DIG");
    _keys[D_CHANNEL] = QString("DESIGNATE_CHANNEL");
    _keys[D_UP_STAIR] = QString("DESIGNATE_STAIR_UP");
    _keys[D_DOWN_STAIR] = QString("DESIGNATE_STAIR_DOWN");
    _keys[D_UPDOWN_STAIR] = QString("DESIGNATE_STAIR_UPDOWN");
    _keys[D_RAMP] = QString("DESIGNATE_RAMP");
    _keys[UP_Z] = QString("CURSOR_UP_Z");
    _keys[DOWN_Z] = QString("CURSOR_DOWN_Z");
    _keys[SELECT] = QString("SELECT");
    _keys[EOG] = QString("End of group");
    _keys[EOM] = "End of macro";
}
