#ifndef PICKER_H
#define PICKER_H

#include <QObject>
#include <QWidget>
#include <QGraphicsItem>
#include <QVector>
#include <vector>
#include <set>

#include "tileface.h"
#include "designations.h"
#include "coords.h"
#include "floor.h"

class Picker : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    typedef enum {FREE, LINE, RECT, CIRCLE, BUCKET, SELECT} drawMode;
    Picker();
    Picker(int width, int height, QVector<QVector<TileFace*> > *faces);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    QRectF boundingRect() const override;

    void matchToFloor();
    Floor *currentFloor() const;
    void setCurrentFloor(Floor *currentFloor);

public slots:
    void sync();

private:
    int _width;
    int _height;
    Designation _CurrentDesignation;

    Floor *_currentFloor;

    QVector<QVector<TileFace*> > *_faces;
    QVector<Coords> _pending;
};

#endif // PICKER_H
