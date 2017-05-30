#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tileface.h"
#include "view.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QDialog>
#include "makenewdialog.h"
#include <QPushButton>
#include <QFormBuilder>
#include <QFile>
#include <iostream>
#include <QVector>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::makeNew);

}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populateScene()
{
    QVector<QVector<TileFace*> > *f = new QVector<QVector<TileFace*> >();
    f->resize(tilesX);
    for(int i = 0; i < tilesY; i++)
        (*f)[i].resize(tilesY);
    _picker = new Picker(tilesX * 12, tilesY * 12,  f);
    _picker->setPos(0, 0);

    scene = new QGraphicsScene(this);
    scene->addItem(_picker);

    for (int i = 0; i < tilesX; i++){
        for (int j = 0; j < tilesY; j++){
            QColor qcl(255, 255, 255);
            TileFace *t = new TileFace(qcl, i, j);
            t->setPos(QPointF(i*12,j*12));
            scene->addItem(t);
            (*f)[i][j] = t;
        }
    }
}

void MainWindow::makeNew()
{
    int returnCode;
    MakeNewDialog *makeNew = new MakeNewDialog();

    returnCode = makeNew->exec();

    tilesX = makeNew->levelWidth();
    tilesY = makeNew->levelHeight();
    if(returnCode){
        populateScene();
        _site = new Site(tilesX, tilesY);
        _picker->setCurrentFloor(_site->currFloor());

        connect(_site, &Site::syncRequired, _picker, &Picker::sync);
        connect(ui->actionUndo, &QAction::triggered, _site, &Site::undo);
        connect(ui->actionRedo, &QAction::triggered, _site, &Site::redo);
        View *vju = new View();
        vju->view()->setScene(scene);
        setCentralWidget(vju->view());
    }
    delete makeNew;
}
