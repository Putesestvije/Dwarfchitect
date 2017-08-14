#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QFrame>
#include <QGraphicsSceneHoverEvent>
#include <QScrollArea>
#include <QSlider>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView();

    GraphicsView(QWidget *parent);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void setupTransform();
    void zoomIn();
    void zoomOut();
    void stretchToggle(bool s);

signals:
    void undo();
    void redo();
    void floorUp();
    void floorDown();

private:
    QSlider *_zoomSlider;
    QTransform _defaultTransform;
    bool _isStretched;
    QWidget *_parent;
};

#endif // GRAPHICSVIEW_H
