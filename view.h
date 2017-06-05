#ifndef VIEW_H
#define VIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QFrame>
#include <QGraphicsSceneHoverEvent>
#include <QScrollArea>

class View;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(){
        setFocusPolicy(Qt::StrongFocus);
    }
    GraphicsView(View *v) : QGraphicsView(), view(v) {
        setFocusPolicy(Qt::StrongFocus);
    }
    void keyPressEvent(QKeyEvent *event);

signals:
    void undo();
    void redo();

private:
    View *view;
};

class View : public QScrollArea
{
public:
    explicit View(QWidget *parent = 0);

    QGraphicsView *gview() const;

private:
    GraphicsView *graphicsView;
};

#endif // VIEW_H
