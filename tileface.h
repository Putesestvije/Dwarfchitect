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

    Key currentDesigantion() const;
    void setCurrentDesigantion(const Key &currentDesigantion);

    Key tempDesignation() const;
    void setTempDesignation(const Key &tempDesignation);

    int rect() const;
    void setRect(int rect);

private:
    /* coresponds to the tileback's rectangle
     * used in debugging to check if the plotter
     * is working properly*/
    int _rect;

    int _x;
    int _y;
    QColor _color;

    Key _currentDesigantion;
    Key _tempDesignation;
};

#endif // TILEFACE_H
