#include <QVBoxLayout>
#include <QScrollArea>
#include <QDialog>
#include <QPushButton>
#include <QFormBuilder>
#include <QFile>
#include <iostream>
#include <QVector>
#include <QProgressBar>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tileface.h"
#include "makenewdialog.h"
#include "view.h"
#include "maximalrectangle.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->mainToolBar->setMovable(false);
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::makeNew);
    connect(ui->actionExport_Macro, &QAction::triggered, this, &MainWindow::exportMacro);

}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::status(QString status)
{
    ui->statusBar->showMessage(status);
}

void MainWindow::populateScene()
{
    QVector<QVector<TileFace*> > *f = new QVector<QVector<TileFace*> >();
    f->resize(_height);
    for(int i = 0; i < _height; i++)
        (*f)[i].resize(_width);
    _picker = new Picker(_width * 12, _height * 12,  f);
    _picker->setPos(0, 0);

    scene = new QGraphicsScene(this);
    scene->addItem(_picker);
    for (int i = 0; i < _height; i++){
        for (int j = 0; j < _width; j++){
            QColor qcl(255, 255, 255);
            TileFace *t = new TileFace(qcl, i, j);
            t->setPos(QPointF(j*12,i*12));
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

        _width = makeNew->levelWidth();
        _height = makeNew->levelHeight();
        _projectName = makeNew->acquiredName();
        if(returnCode){
            populateScene();
            _site = new Site(_width, _height);
            _picker->setCurrentFloor(_site->currFloor());

            connect(_site, &Site::syncRequired, _picker, &Picker::sync);
            connect(ui->actionUndo, &QAction::triggered, _site, &Site::undo);
            connect(ui->actionRedo, &QAction::triggered, _site, &Site::redo);

            _graphicView = new GraphicsView();
            _graphicView->setScene(scene);
            centralWidget()->layout()->addWidget(_graphicView);

            connect(_graphicView, &GraphicsView::undo, _site, &Site::undo);
            connect(_graphicView, &GraphicsView::redo, _site, &Site::redo);

            connect(_picker, &Picker::mousePosition, this, &MainWindow::status);

            progressBar = new QProgressBar();
            progressBar->setMinimum(0);
            progressBar->setValue(0);
            progressBar->setFixedWidth(200);
            progressBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            ui->statusBar->addWidget(progressBar);
            /*centralWidget()->layout()->addWidget(progressBar);*/
            //progressBar->hide();
        }
    delete makeNew;
}

void MainWindow::exportMacro()
{
    //QFile macroFile;
    //TODO Output to file
    MaximalRectangle maxRec(_site->topFloor(), _width, _height);
    progressBar->setMaximum(maxRec.amountOfWork()*2);
    progressBar->reset();

    progressBar->show();
    connect(&maxRec, &MaximalRectangle::progessed, this, &MainWindow::progressed);
    connect(&maxRec, &MaximalRectangle::syncFaces, _picker, &Picker::sync);

    maxRec.generateMacro();

    maxRec.clearSite();
    /* uncomment once the whole algorithm works
     * progressBar->hide(); */

}

void MainWindow::progressed(int p)
{
    progressBar->setValue(progressBar->value()+p);
}


