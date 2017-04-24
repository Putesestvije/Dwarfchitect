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
    scene = new QGraphicsScene(this);

    for (int i = 0; i < tilesX; i++){
        for (int j = 0; j < tilesY; j++){
            QColor qcl(0, 0, 0);
            QGraphicsItem *t = new TileFace(qcl, i, j);
            t->setPos(QPointF(i*12,j*18));
            scene->addItem(t);
        }
    }
}

void MainWindow::makeNew()
{
    int returnCode;
    MakeNewDialog *makeNew = new MakeNewDialog();

    returnCode  = makeNew->exec();

    tilesX = makeNew->levelWidth();
    tilesY = makeNew->levelHeight();
    if(returnCode){
        populateScene();
        View *vju = new View();
        vju->view()->setScene(scene);
        setCentralWidget(vju->view());
    }
    delete makeNew;
}
