#include "view.h"
#include <QGridLayout>
#include <QVBoxLayout>

View::View(QWidget *parent)
    : QScrollArea(parent)
{
    //setFrameStyle(StyledPanel);
    graphicsView = new GraphicsView(this);
    graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(graphicsView);
    setLayout(layout);

}

QGraphicsView *View::view() const
{
    return static_cast<QGraphicsView *>(graphicsView);
}
