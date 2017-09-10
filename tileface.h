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

    Key currentDesignation() const;
    void setCurrentDesignation(const Key &currentDesignation);

    Key tempDesignation() const;
    void setTempDesignation(const Key &tempDesignation);

    int rect() const;
    void setRect(int rect);
    static void setDorfFont(int id);

    bool underConstruction() const;
    void setUnderConstruction(bool underConstruction);

    bool starter() const;
    void setStarter(bool starter);

private:
    /* coresponds to the tileback's rectangle
     * used in debugging to check if the plotter
     * is working properly*/
    int _rect;

    int _x;
    int _y;
    QColor _color;

    static int _dorfFont;

    Key _currentDesigantion;
    Key _tempDesignation;

    bool _underConstruction;
    bool _starter;
};

#endif // TILEFACE_H
