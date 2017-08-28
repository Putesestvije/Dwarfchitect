#ifndef PICKER_H
#define PICKER_H

#include <QObject>
#include <QWidget>
#include <QGraphicsItem>
#include <QVector>
#include <vector>
#include <set>

#include "tileface.h"
#include "designations.h"
#include "coords.h"
#include "floor.h"

class Picker : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    typedef enum {FREE, LINE, RECT, CIRCLE, BUCKET, SELECT} drawMode;
    Picker();
    Picker(int width, int height, std::vector<std::vector<TileFace*> > *faces);
    ~Picker();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    QRectF boundingRect() const override;

    void matchToFloor();
    Floor *currentFloor() const;
    void setCurrentFloor(Floor *currentFloor);

signals:
    void changesMadeToModel();
    void undo();
    void redo();
    void mousePosition(QString pos);

public slots:
    void sync();
    void newFloor(Floor *f);
    void setCurrentDesignation(int k);
    void setDrawMode(int d);
    void setBrushType(int b);
    void setupBrush();
    void resizeBrush(int size);

private:
    void drawFreeHand(QGraphicsSceneMouseEvent *event);
    void drawLine(QGraphicsSceneMouseEvent *event);
    void setupSquareBrush();
    void setupCircleBrush();
    void applyBrush(Coords c);
    void markWithBrush(Coords c);
    void unmarkPrevious();
    CoordPair orderedEnds();
    void drawVerticalLine(CoordPair &ends);
    void drawHorizontalLine(CoordPair &ends);
    void drawLowerSlope(int dx, int dy, CoordPair &ends);
    void drawHigherSlope(int dx, int dy, CoordPair &ends);
    void drawCircle();

    int _width;
    int _height;
    Key _CurrentDesignation;
    DrawMode _drawMode;
    BrushType _brushType;
    QVector <QVector <Coords> > _brush;
    QVector <Coords> _underConstruction;
    QVector <Coords> _marked;
    Coords UC;
    Coords _fixedPoint;
    Coords _mobilePoint;

    Floor *_currentFloor;

    std::vector<std::vector<TileFace*> > *_faces;
    QVector<Coords> _pending;
};

#endif // PICKER_H
