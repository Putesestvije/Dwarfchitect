#ifndef VIEW_H
#define VIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QFrame>
#include <QScrollArea>

class View;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(View *v) : QGraphicsView(), view(v) { }

private:
    View *view;
};

class View : public QScrollArea
{
public:
    explicit View(QWidget *parent = 0);

    QGraphicsView *view() const;

private:
    GraphicsView *graphicsView;
};

#endif // VIEW_H
