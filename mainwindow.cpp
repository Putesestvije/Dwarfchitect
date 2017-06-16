#include <QVBoxLayout>
#include <QScrollArea>
#include <QDialog>
#include <QPushButton>
#include <QFormBuilder>
#include <QFile>
#include <iostream>
#include <QVector>
#include <QProgressBar>
#include <QMap>

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


            _graphicView = new GraphicsView();
            _graphicView->setScene(scene);
            centralWidget()->layout()->addWidget(_graphicView);

            connect(_site, &Site::syncRequired, _picker, &Picker::sync);
            connect(_site, &Site::currFloorChanged, _picker, &Picker::newFloor);

            connect(ui->actionAdd_New_Bottom_Layer, &QAction::triggered, _site, &Site::addNewBottomFloor);
            connect(ui->actionAdd_New_Top_Layer, &QAction::triggered, _site, &Site::addNewTopFloor);
            connect(ui->actionAdd_New_Layer_Above_Current, &QAction::triggered, _site, &Site::addFloorAboveCurr);
            connect(ui->actionAdd_New_Layer_Below_Current, &QAction::triggered, _site, &Site::addFloorBelowCurr);

            connect(ui->actionMove_Up, &QAction::triggered, _site, &Site::moveCurrUp);
            connect(ui->actionMove_Down, &QAction::triggered, _site, &Site::moveCurrDown);

            connect(ui->actionUndo, &QAction::triggered, _site, &Site::undo);
            connect(ui->actionRedo, &QAction::triggered, _site, &Site::redo);

            connect(_graphicView, &GraphicsView::undo, _site, &Site::undo);
            connect(_graphicView, &GraphicsView::redo, _site, &Site::redo);
            connect(_graphicView, &GraphicsView::floorUp, _site, &Site::moveCurrUp);
            connect(_graphicView, &GraphicsView::floorDown, _site, &Site::moveCurrDown);

            connect(ui->actionZoom_in, &QAction::triggered, _graphicView, &GraphicsView::zoomIn);
            connect(ui->actionZoom_out, &QAction::triggered, _graphicView, &GraphicsView::zoomOut);

            connect(ui->actionSquare_Rectangle_render, &QAction::triggered, _graphicView, &GraphicsView::stretchToggle);

            connect(_picker, &Picker::mousePosition, this, &MainWindow::status);

            progressBar = new QProgressBar();
            progressBar->setMinimum(0);
            progressBar->setValue(0);
            progressBar->setFixedWidth(200);
            progressBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            ui->statusBar->addWidget(progressBar);
        }
    delete makeNew;
}

void MainWindow::exportMacro()
{
    MaximalRectangle maxRec(_site->topFloor(), _width, _height, this, _projectName);
    progressBar->setMaximum(maxRec.amountOfWork()*2);
    progressBar->setValue(0);

    progressBar->show();
    connect(&maxRec, &MaximalRectangle::progessed, this, &MainWindow::progressed);
    connect(&maxRec, &MaximalRectangle::syncFaces, _picker, &Picker::sync);

    maxRec.generateMacro();

    std::cout << progressBar->maximum() << " ";
    std::cout << progressBar->value() << std::endl;

    maxRec.clearSite();

    progressBar->hide();
}

void MainWindow::progressed(int p)
{
    progressBar->setValue(progressBar->value()+p);
}

void MainWindow::connectUponNew()
{

}


