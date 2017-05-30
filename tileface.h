#ifndef TILEFACE_H
#define TILEFACE_H

#include <QObject>
#include <QWidget>
#include <QGraphicsItem>

#include "designations.h"


class TileFace : public QGraphicsItem
{
public:
    TileFace(const QColor &color, int x, int y);

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *Option, QWidget *widget) Q_DECL_OVERRIDE;
    void setColor(int r, int g, int b, int a = 255);

    Designation currentDesigantion() const;
    void setCurrentDesigantion(const Designation &currentDesigantion);

    Designation tempDesignation() const;
    void setTempDesignation(const Designation &tempDesignation);

private:
    int _x;
    int _y;
    QColor _color;

    Designation _currentDesigantion;
    Designation _tempDesignation;
};

#endif // TILEFACE_H
