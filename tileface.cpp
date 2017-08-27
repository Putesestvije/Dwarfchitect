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
    _currentDesigantion = D_CLEAR;
    _tempDesignation = D_CLEAR;
    _rect = 0;
    _underConstruction = false;
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

    Key designationToBePainted;

    if (_underConstruction){
        designationToBePainted = _tempDesignation;
        //std::cout << "TEMP DES" << std::endl;
    }
    else{
        designationToBePainted = _currentDesigantion;
        //std::cout << "CLEAR" << std::endl;
    }

    if(designationToBePainted == D_CLEAR)
        _color.setRgb(255, 255, 255);
    else
        _color.setRgb(128, 128, 0);

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
    QFont font = QFont("Px437 IBM BIOS");
    font.setPointSize(8);
    painter->setFont(font);
    char c = 30;
    QString ramp(c);
    c = 22;
    QString channel(c);


    switch (designationToBePainted){
    case D_DIG:
        painter->drawText(boundingRect(), Qt::AlignJustify | Qt::AlignHCenter |Qt::AlignVCenter, " ");
        //painter->drawText(boundingRect(), Qt::AlignJustify | Qt::AlignHCenter |Qt::AlignVCenter, );
        break;
    case D_CHANNEL :
        painter->drawText(boundingRect(), Qt::AlignJustify | Qt::AlignHCenter |Qt::AlignVCenter, channel);
        break;
    case D_UPDOWN_STAIR :
        painter->drawText(boundingRect(), Qt::AlignJustify | Qt::AlignHCenter |Qt::AlignVCenter, "X");
        break;
    case D_UP_STAIR :
        painter->drawText(boundingRect(), Qt::AlignJustify | Qt::AlignHCenter |Qt::AlignVCenter, "<");
        break;
    case D_DOWN_STAIR :
        painter->drawText(boundingRect(), Qt::AlignJustify | Qt::AlignHCenter |Qt::AlignVCenter, ">");
        break;
    case D_RAMP :
        painter->drawText(boundingRect(), Qt::AlignJustify | Qt::AlignHCenter |Qt::AlignVCenter, ramp);
        break;
    default:
        break;
    }


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

Key TileFace::currentDesignation() const
{
    return _currentDesigantion;
}

void TileFace::setCurrentDesignation(const Key &currentDesigantion)
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

bool TileFace::underConstruction() const
{
    return _underConstruction;
}

void TileFace::setUnderConstruction(bool underConstruction)
{
    _underConstruction = underConstruction;
}

