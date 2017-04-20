#ifndef TILEFACE_H
#define TILEFACE_H

#include <QObject>
#include <QWidget>
#include <QGraphicsItem>


class TileFace : public QGraphicsItem
{
public:
    TileFace(const QColor &color, int x, int y);

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *Option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    int x;
    int y;
    QColor color;
};

#endif // TILEFACE_H
