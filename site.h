#ifndef SITE_H
#define SITE_H

#include <QObject>

#include "floor.h"

class Site : public QObject
{
    Q_OBJECT
public:

    Site();
    Site(int width, int height);

    ~Site();

    void addFloorAboveCurr();
    void addFloorBelowCurr();
    void addNewTopFloor();
    void addNewBottomFloor();
    void removeCurrentFloor();
    Floor *currFloor() const;
    void setCurrFloor(Floor *currFloor);

    void moveCurrUp();
    void moveCurrDown();

    Floor *topFloor() const;

    Floor *bottomFloor() const;

    int width() const;
    void setWidth(int width);

    int height() const;
    void setHeight(int height);

signals:
    void syncRequired();
    void currFloorChanged(Floor *f);
    void topFloorChanged();

public slots:
    void undo();
    void redo();
    void tester();

private:
    int _width;
    int _height;

    Floor *_currFloor;
    Floor *_topFloor;
    Floor *_bottomFloor;
};



#endif // SITE_H
