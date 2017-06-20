#include "tileface.h"
#include <iostream>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QColor>
#include <QStyleOptionGraphicsItem>

int TileFace::_dorfFont;

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
    _rect = 0;
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
    //Q_UNUSED(Option);
    /*if (_x == 0 && _y==0)
        std::cout << Option->levelOfDetailFromTransform(painter->worldTransform()) << std::endl;*/

    //QColor fillColor(255, 255, 255);/*Reimplement later for th ecorrect color choice*/

    QPen oldPen = painter->pen();
    QPen pen(Qt::black, 1);
    pen.setWidth(0);

    QBrush oldBrush = painter->brush();
    QBrush brush;
    brush.setColor(_color);
    brush.setStyle(Qt::SolidPattern);

    painter->setBrush(brush);
    painter->setPen(pen);

    if (Option->levelOfDetailFromTransform(painter->worldTransform()) < 0.25)
        painter->fillRect(boundingRect(), _color);
    else
        painter->drawRect(boundingRect());
    QFont oldfont = painter->font();
    QFont font = QFont("DF Curses 8x12");
    font.setPointSize(12);
    painter->setFont(font);
    /*uncomment for debugging*/
    if (_currentDesigantion == D_DIG)
        //painter->drawText(boundingRect(), "d");
        painter->drawText(boundingRect(), Qt::AlignJustify | Qt::AlignHCenter |Qt::AlignVCenter, " ");

    painter->setFont(oldfont);
    painter->setBrush(oldBrush);
    painter->setPen(oldPen);

    return;
}

void TileFace::setColor(int r, int g, int b, int a)
{
    Q_UNUSED(a);
    _color.setRgb(r, g, b);
    update();
    return;
}

Key TileFace::currentDesigantion() const
{
    return _currentDesigantion;
}

void TileFace::setCurrentDesigantion(const Key &currentDesigantion)
{
    _currentDesigantion = currentDesigantion;
}

Key TileFace::tempDesignation() const
{
    return _tempDesignation;
}

void TileFace::setTempDesignation(const Key &tempDesignation)
{
    _tempDesignation = tempDesignation;
}

int TileFace::rect() const
{
    return _rect;
}

void TileFace::setRect(int rect)
{
    _rect = rect;
}

void TileFace::setDorfFont(int id)
{
    _dorfFont = id;
}
