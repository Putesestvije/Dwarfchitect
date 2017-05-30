#include "tileface.h"
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QColor>

TileFace::TileFace(const QColor &color, int x, int y)
{
    this->_x = x;
    this->_y = y;
    this->_color = color;
    setZValue((x + y) % 2);

    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);
    _currentDesigantion = CLEAR;
    _tempDesignation = CLEAR;
}

QRectF TileFace::boundingRect() const
{
    return QRectF(0, 0, 12, 12);
}

QPainterPath TileFace::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, 12, 12);
    return path;
}

void TileFace::paint(QPainter *painter, const QStyleOptionGraphicsItem *Option, QWidget *widget)
{
    Q_UNUSED(widget);

    //QColor fillColor(255, 255, 255);/*Reimplementirati kasnije kada treba da bira boju*/

    QPen oldPen = painter->pen();
    QPen pen(Qt::black, 1);
    pen.setWidth(0);

    QBrush oldBrush = painter->brush();
    QBrush brush;
    brush.setColor(_color);
    brush.setStyle(Qt::SolidPattern);

    painter->setBrush(brush);
    painter->setPen(pen);

    painter->drawRect(boundingRect());

    painter->setBrush(oldBrush);
    painter->setPen(oldPen);

    return;
}

void TileFace::setColor(int r, int g, int b, int a)
{
    _color.setRgb(r, g, b);
    update();
    return;
}

Designation TileFace::currentDesigantion() const
{
    return _currentDesigantion;
}

void TileFace::setCurrentDesigantion(const Designation &currentDesigantion)
{
    _currentDesigantion = currentDesigantion;
}

Designation TileFace::tempDesignation() const
{
    return _tempDesignation;
}

void TileFace::setTempDesignation(const Designation &tempDesignation)
{
    _tempDesignation = tempDesignation;
}
