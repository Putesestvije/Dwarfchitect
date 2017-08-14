#include "graphicsview.h"
#include <QKeyEvent>
#include <QWheelEvent>
#include <qmath.h>
#include <iostream>

GraphicsView::GraphicsView(){
    setFocusPolicy(Qt::StrongFocus);
    _zoomSlider = new QSlider();
    _zoomSlider->setMaximum(250);
    _zoomSlider->setMinimum(0);
    _zoomSlider->setValue(250);
    _defaultTransform = transform();
    _isStretched = false;
    connect(_zoomSlider, &QSlider::valueChanged, this, &GraphicsView::setupTransform);
}

GraphicsView::GraphicsView(QWidget *parent) :
    _parent(parent){
    setFocusPolicy(Qt::StrongFocus);
    _zoomSlider = new QSlider();
    _zoomSlider->setMaximum(250);
    _zoomSlider->setMinimum(0);
    _zoomSlider->setValue(250);
    _defaultTransform = transform();
    _isStretched = false;
    connect(_zoomSlider, &QSlider::valueChanged, this, &GraphicsView::setupTransform);
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::keyPressEvent(QKeyEvent *event)
{

    int k = event->key();
    Qt::KeyboardModifiers q = event->modifiers();

    if (q == Qt::ControlModifier)
        switch (k) {
        case Qt::Key_Z :
            emit undo();
            break;
        case Qt::Key_Y :
            emit redo();
            break;
        }
    else if (q == Qt::ShiftModifier)
        switch (k) {
        case Qt::Key_Less :
            emit floorUp();
            break;
        case Qt::Key_Greater :
            emit floorDown();
            break;
        }
    else if(q == (Qt::ControlModifier | Qt::ShiftModifier))
        switch (k){
        case Qt::Key_Z :
            emit redo();
        }
    else
        QGraphicsView::keyPressEvent(event);
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() & Qt::ControlModifier){
        if(event->delta() > 0)
            _zoomSlider->setValue(_zoomSlider->value()+6);
        else if (event->delta() < 0 )
            _zoomSlider->setValue(_zoomSlider->value()-6);
        event->accept();
    } else
        QGraphicsView::wheelEvent(event);

}

void GraphicsView::setupTransform()
{
    qreal scale = qPow(2,  (_zoomSlider->value()- 250) / qreal(50));
    //std::cout << scale << std::endl;

    QTransform tr = QTransform();
    tr.scale(scale,scale);
    setTransform(tr);
}

void GraphicsView::zoomIn()
{
    _zoomSlider->setValue(_zoomSlider->value()+6);
}

void GraphicsView::zoomOut()
{
    _zoomSlider->setValue(_zoomSlider->value()-6);
}

void GraphicsView::stretchToggle(bool s)
{
    if (s){
        QTransform tr;
        tr.scale(1,1.5);
        _defaultTransform = tr;
    } else {
        QTransform tr;
        _defaultTransform = tr;
    }
    setupTransform();
}

