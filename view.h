#ifndef VIEW_H
#define VIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QFrame>
#include <QGraphicsSceneHoverEvent>
#include <QScrollArea>
#include <QSlider>
class View;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(){
        setFocusPolicy(Qt::StrongFocus);
        _zoomSlider = new QSlider();
        _zoomSlider->setMaximum(125);
        _zoomSlider->setMinimum(0);
        _zoomSlider->setValue(125);
        _normal = QTransform();
        _stretch = QTransform();
        _stretch.scale(1,1.5);
        _isStretched = false;
        connect(_zoomSlider, &QSlider::valueChanged, this, &GraphicsView::setupTransform);
    }
    GraphicsView(View *v) : QGraphicsView(), view(v) {
        setFocusPolicy(Qt::StrongFocus);
    }
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
    View *view;
    QSlider *_zoomSlider;
    QTransform _normal;
    QTransform _stretch;
    bool _isStretched;
};


/* The whole class is quite unecessary, I think I'll
 * get rid of it later*/

class View : public QScrollArea
{
public:
    explicit View(QWidget *parent = 0);

    QGraphicsView *gview() const;

private:
    GraphicsView *graphicsView;
};

#endif // VIEW_H
