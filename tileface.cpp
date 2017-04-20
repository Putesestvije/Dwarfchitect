#include "tileface.h"
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QColor>

TileFace::TileFace(const QColor &color, int x, int y)
{
    this->x = x;
    this->y = y;
    this->color = color;
    setZValue((x + y) % 2);

    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);
}

QRectF TileFace::boundingRect() const
{
    return QRectF(0, 0, 12, 18);
}

QPainterPath TileFace::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, 12, 18);
    return path;
}

void TileFace::paint(QPainter *painter, const QStyleOptionGraphicsItem *Option, QWidget *widget)
{
    Q_UNUSED(widget);

    QColor fillColor(20, 20, 20);/*Reimplementirati kasnije kada treba da bira boju*/

    QPen oldPen = painter->pen();
    QPen pen(Qt::white, 1);
    pen.setWidth(0);

    QBrush oldBrush = painter->brush();
    QBrush brush;
    brush.setColor(fillColor);
    brush.setStyle(Qt::SolidPattern);

    painter->setBrush(brush);
    painter->setPen(pen);

    painter->drawRect(boundingRect());

    painter->setBrush(oldBrush);
    painter->setPen(oldPen);

    return;
}
