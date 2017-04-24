#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tileface.h"
#include "view.h"
#include <QVBoxLayout>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    populateScene();
    View *vju = new View();
    vju->view()->setScene(scene);
    setCentralWidget(vju->view());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populateScene()
{
    scene = new QGraphicsScene(this);

    for (int i = 0; i < 100; i++){
        for (int j = 0; j < 100; j++){
            QColor qcl(0, 0, 0);
            QGraphicsItem *t = new TileFace(qcl, i, j);
            t->setPos(QPointF(i*12,j*18));
            scene->addItem(t);
        }
    }
   /* QImage image(":/qt4logo.png");

    // Populate scene
    int xx = 0;
    int nitems = 0;
    for (int i = -11000; i < 11000; i += 110) {
        ++xx;
        int yy = 0;
        for (int j = -7000; j < 7000; j += 70) {
            ++yy;
            qreal x = (i + 11000) / 22000.0;
            qreal y = (j + 7000) / 14000.0;

            QColor color(image.pixel(int(image.width() * x), int(image.height() * y)));
            QGraphicsItem *item = new Chip(color, xx, yy);
            item->setPos(QPointF(i, j));
            scene->addItem(item);

            ++nitems;
        }
    }*/
}
