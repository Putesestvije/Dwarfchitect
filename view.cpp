#include "view.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QKeyEvent>

View::View(QWidget *parent)
    : QScrollArea(parent)
{
    //setFrameStyle(StyledPanel);
    graphicsView = new GraphicsView(this);
    graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
/*
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(graphicsView);
    setLayout(layout);
*/
}

void GraphicsView::keyPressEvent(QKeyEvent *event)
{
    /* int k = event->key();
    Qt::KeyboardModifiers q = event->modifiers();
    if(k == Qt::Key_Z && q == Qt::ControlModifier)
        emit undo();
    else if ((k == Qt::Key_Z && q == (Qt::ControlModifier | Qt::ShiftModifier))
             || (k == Qt::Key_Y && q == Qt::ControlModifier))
        emit redo();
    else
        QGraphicsView::keyPressEvent(event);
    */
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
