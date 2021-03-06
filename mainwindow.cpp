#include <QFontDatabase>
//#include <QFormBuilder>
#include <QProgressBar>
#include <QStringList>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QCloseEvent>
#include <QTextEdit>
#include <QSpinBox>
#include <iostream>
#include <QDialog>
#include <QRegExp>
#include <QString>
#include <QVector>
#include <string>
#include <QFile>
#include <QMap>

#include "maximalrectangle.h"
#include "ui_mainwindow.h"
#include "makenewdialog.h"
#include "mainwindow.h"
#include "tileface.h"
#include "graphicsview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initCsvMap();
    //ui->mainToolBar->setMovable(false);
    _scene = nullptr;
    _picker = nullptr;
    _site = nullptr;
    _graphicView = new GraphicsView();
    _unsavedChanges = false;


    QFontDatabase::addApplicationFont(":/DF_Curses_8x12.ttf");
    QFontDatabase::addApplicationFont(":/Px437_IBM_BIOS.ttf");
    QFontDatabase::addApplicationFont(":/Px437_IBM_BIOS-2y.ttf");

    /*for debugging fonts*/
   /* QStringList fonts = QFontDatabase::applicationFontFamilies(id);
    for (int i = 0; i < fonts.size(); i++){
        QString s(fonts.at(i));
        std::cout << s.toStdString() << std::endl;
    }
    fonts = QFontDatabase::applicationFontFamilies(id3);
    for (int i = 0; i < fonts.size(); i++){
        QString s(fonts.at(i));
        std::cout << s.toStdString() << std::endl;
    }
*/
    _progressBar = new QProgressBar();
    _progressBar->setMinimum(0);
    _progressBar->setValue(0);
    _progressBar->setFixedWidth(200);
    _progressBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui->statusBar->addWidget(_progressBar);

    _progressBar->hide();

    _savePath = "";

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::makeNew);
    connect(ui->actionExport_Macro, &QAction::triggered, this, &MainWindow::exportMacro);
    connect(ui->actionZoom_in, &QAction::triggered, ui->graphicsView, &GraphicsView::zoomIn);
    connect(ui->actionZoom_out, &QAction::triggered, ui->graphicsView, &GraphicsView::zoomOut);

    connect(ui->actionSquare_Rectangle_render, &QAction::triggered, ui->graphicsView, &GraphicsView::stretchToggle);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveAs);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);


    _buttonToDesignation = new QSignalMapper(this);

    _buttonToDesignation->setMapping(ui->digButton, D_DIG);
    _buttonToDesignation->setMapping(ui->channelButton, D_CHANNEL);
    _buttonToDesignation->setMapping(ui->UDStairButton, D_UPDOWN_STAIR);
    _buttonToDesignation->setMapping(ui->upStairButton, D_UP_STAIR);
    _buttonToDesignation->setMapping(ui->downStairButton, D_DOWN_STAIR);
    _buttonToDesignation->setMapping(ui->rampUpButton, D_RAMP);
    _buttonToDesignation->setMapping(ui->clearButton, D_CLEAR);
    _buttonToDesignation->setMapping(ui->startDesButton, D_START);



    connect(ui->digButton, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));
    connect(ui->channelButton, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));
    connect(ui->UDStairButton, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));
    connect(ui->upStairButton, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));
    connect(ui->downStairButton, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));
    connect(ui->rampUpButton, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));
    connect(ui->clearButton, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));
    connect(ui->startDesButton, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));

    _designationButtons = new QButtonGroup(this);

    _designationButtons->setExclusive(true);
    _designationButtons->addButton(ui->digButton);
    _designationButtons->addButton(ui->channelButton);
    _designationButtons->addButton(ui->UDStairButton);
    _designationButtons->addButton(ui->upStairButton);
    _designationButtons->addButton(ui->downStairButton);
    _designationButtons->addButton(ui->rampUpButton);
    _designationButtons->addButton(ui->clearButton);
    _designationButtons->addButton(ui->startDesButton);

    _buttonToDrawMode = new QSignalMapper();

    _buttonToDrawMode->setMapping(ui->pencilButton, M_FREEHAND);
    _buttonToDrawMode->setMapping(ui->lineButton, M_LINE);
    _buttonToDrawMode->setMapping(ui->ellipseButton, M_ELLIPSE);
    _buttonToDrawMode->setMapping(ui->rectangleButton, M_RECT);
    _buttonToDrawMode->setMapping(ui->bucketButton, M_FILL);

    connect(ui->pencilButton, SIGNAL(toggled(bool)), _buttonToDrawMode, SLOT(map()));
    connect(ui->lineButton, SIGNAL(toggled(bool)), _buttonToDrawMode, SLOT(map()));
    connect(ui->ellipseButton, SIGNAL(toggled(bool)), _buttonToDrawMode, SLOT(map()));
    connect(ui->rectangleButton, SIGNAL(toggled(bool)), _buttonToDrawMode, SLOT(map()));
    connect(ui->bucketButton, SIGNAL(toggled(bool)), _buttonToDrawMode, SLOT(map()));

    _buttonToBrushType = new QSignalMapper();

    _buttonToBrushType->setMapping(ui->squareBrushButton, B_SQUARE);
    _buttonToBrushType->setMapping(ui->circularBrushButton, B_CIRCLE);

    connect(ui->squareBrushButton, SIGNAL(toggled(bool)), _buttonToBrushType, SLOT(map()));
    connect(ui->circularBrushButton, SIGNAL(toggled(bool)), _buttonToBrushType, SLOT(map()));

    _drawButtons = new QButtonGroup(this);

    _drawButtons->setExclusive(true);
    _drawButtons->addButton(ui->toggleGrabButton);
    _drawButtons->addButton(ui->pencilButton);
    _drawButtons->addButton(ui->lineButton);
    _drawButtons->addButton(ui->ellipseButton);
    _drawButtons->addButton(ui->rectangleButton);
    _drawButtons->addButton(ui->bucketButton);

    _brushButtons = new QButtonGroup();

    _brushButtons->setExclusive(true);
    _brushButtons->addButton(ui->squareBrushButton);
    _brushButtons->addButton(ui->circularBrushButton);

    connect(_buttonToDesignation, SIGNAL(mapped(int)), this, SLOT(changeDesignationPreview(int)));

    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    _hasStarter = false;
    _starterTile = Coords(-1, -1);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete _site;
    delete _picker;
}

void MainWindow::status(QString status)
{
    ui->statusBar->showMessage(status);
}

void MainWindow::toggleUnsavedChanges()
{
    _unsavedChanges = true;
    _title = QString(_projectName);
    _title.append("* - Dwarfchitect");
    setWindowTitle(_title);
}

void MainWindow::removeStarterTile()
{
    _hasStarter = false;
}

void MainWindow::populateScene()
{
    std::vector<std::vector<TileFace*> > *faces = new std::vector<std::vector<TileFace*> >();
    faces->resize(_height);
    for(int i = 0; i < _height; i++)
        (*faces)[i].resize(_width);
    _picker = new Picker(_width * 12, _height * 12,  faces);
    _picker->setPos(0, 0);


    for (int i = 0; i < _height; i++){
        for (int j = 0; j < _width; j++){
            QColor qcl(255, 255, 255);
            TileFace *t = new TileFace(qcl, i, j);
            t->setPos(QPointF(j*12,i*12));
            _scene->addItem(t);
            (*faces)[i][j] = t;
        }
    }
    _scene->addItem(_picker);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (_unsavedChanges){
        QMessageBox msgBox;
        QFont dorfs("DF Curses 8x12");
        dorfs.setPointSize(12);
        msgBox.setFont(dorfs);
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret){
        case QMessageBox::Save :
            save();
            event->accept();
            break;
        case QMessageBox::Discard :
            event->accept();
            break;
        case QMessageBox::Cancel :
            event->ignore();
            break;
        }
    } else
        event->accept();
}

bool MainWindow::hasStarter() const
{
    return _hasStarter;
}

void MainWindow::setHasStarter(bool hasStarter)
{
    _hasStarter = hasStarter;
}

Coords MainWindow::starterTile() const
{
    return _starterTile;
}

void MainWindow::setStarterTile(const Coords &starterTile)
{
    _starterTile = starterTile;

    _hasStarter = true;
}

void MainWindow::initCsvMap()
{
    _csvMap[" "] = D_CLEAR;
    _csvMap[""] = D_CLEAR;
    _csvMap["'"] = D_CLEAR;
    _csvMap["`"] = D_CLEAR;
    _csvMap["~"] = D_CLEAR;
    _csvMap["#"] = D_CLEAR;
    _csvMap["d"] = D_DIG;
    _csvMap["h"] = D_CHANNEL;
    _csvMap["u"] = D_UP_STAIR;
    _csvMap["j"] = D_DOWN_STAIR;
    _csvMap["i"] = D_UPDOWN_STAIR;
    _csvMap["r"] = D_RAMP;
    _csvMap["#>"] = DOWN_Z;
}

void MainWindow::makeNew()
{

    /* saveUponNewOrOpen might be misleading, it returns
     * true when you want to proceed with making a
     * new project whether you save the old one or not
     * and false when you click cancel*/
    if(!saveUponNewOrOpen())
        return;

    int returnCode;
    MakeNewDialog *makeNew = new MakeNewDialog();

    returnCode = makeNew->exec();

    if(returnCode){
        _hasStarter = false;
        _width = makeNew->levelWidth();
        _height = makeNew->levelHeight();
        _projectName = makeNew->acquiredName();
        QGraphicsScene *oldScene = _scene;
        _scene = new QGraphicsScene(0, 0, _width*12, _height*12);
        populateScene();
        ui->graphicsView->setScene(_scene);
        ui->digButton->setChecked(true);
        _picker->setCurrentDesignation(D_DIG);
        if (oldScene != nullptr)
           delete oldScene;

        if(_site != nullptr)
           delete _site;
        _site = new Site(_width, _height);
        _picker->setCurrentFloor(_site->currFloor());

        connectUponNew();

        _title = QString(_projectName);
        _title.append(" - Dwarfchitect");
        setWindowTitle(_title);

    }
    delete makeNew;

}

void MainWindow::exportMacro()
{
    MaximalRectangle maxRec(_site->topFloor(), _width, _height, _projectName, this);
    if(maxRec.amountOfWork() == 0)
        return;
    _progressBar->setMaximum(maxRec.amountOfWork()*2);
    _progressBar->setValue(0);

    _progressBar->show();
    connect(&maxRec, &MaximalRectangle::progessed, this, &MainWindow::progressed);
    connect(&maxRec, &MaximalRectangle::syncFaces, _picker, &Picker::sync);

    maxRec.generateMacro();

    //std::cout << _progressBar->maximum() << " ";
    //std::cout << _progressBar->value() << std::endl;

    maxRec.clearSite();

    _progressBar->hide();
}

void MainWindow::progressed(int p)
{
    _progressBar->setValue(_progressBar->value()+p);
}

void MainWindow::connectUponNew()
{
    connect(_site, &Site::syncRequired, _picker, &Picker::sync);
    connect(_site, &Site::currFloorChanged, _picker, &Picker::newFloor);
    connect(_site, &Site::topFloorChanged, _picker, &Picker::removeStarterTile);
    connect(_site, &Site::topFloorChanged, this, &MainWindow::removeStarterTile);

    connect(ui->actionAdd_New_Bottom_Layer, &QAction::triggered, _site, &Site::addNewBottomFloor);
    connect(ui->actionAdd_New_Top_Layer, &QAction::triggered, _site, &Site::addNewTopFloor);
    connect(ui->actionAdd_New_Layer_Above_Current, &QAction::triggered, _site, &Site::addFloorAboveCurr);
    connect(ui->actionAdd_New_Layer_Below_Current, &QAction::triggered, _site, &Site::addFloorBelowCurr);

    connect(ui->actionMove_Up, &QAction::triggered, _site, &Site::moveCurrUp);
    connect(ui->actionMove_Down, &QAction::triggered, _site, &Site::moveCurrDown);
    connect(ui->actionDelete_Current_Layer, &QAction::triggered, _site, &Site::removeCurrentFloor);

    connect(ui->actionUndo, &QAction::triggered, _site, &Site::undo);
    connect(ui->actionRedo, &QAction::triggered, _site, &Site::redo);

    connect(ui->graphicsView, &GraphicsView::undo, _site, &Site::undo);
    connect(ui->graphicsView, &GraphicsView::redo, _site, &Site::redo);
    connect(ui->graphicsView, &GraphicsView::floorUp, _site, &Site::moveCurrUp);
    connect(ui->graphicsView, &GraphicsView::floorDown, _site, &Site::moveCurrDown);

    connect(_picker, &Picker::mousePosition, this, &MainWindow::status);
    connect(_picker, &Picker::changesMadeToModel, this, &MainWindow::toggleUnsavedChanges);

    connect(_buttonToDesignation, SIGNAL(mapped(int)), _picker, SLOT(setCurrentDesignation(int)));
    connect(_buttonToDrawMode, SIGNAL(mapped(int)), _picker, SLOT(setDrawMode(int)));
    connect(_buttonToBrushType, SIGNAL(mapped(int)), _picker, SLOT(setBrushType(int)));

    connect(ui->toggleGrabButton, &QPushButton::toggled, this, &MainWindow::toggleGrabMode);

    connect(ui->brushSizeSpinBox, SIGNAL(valueChanged(int)), _picker, SLOT(resizeBrush(int)));

    ui->digButton->toggle();
    ui->pencilButton->toggle();
    ui->squareBrushButton->toggle();
    _picker->setBrushType(B_SQUARE);
    _picker->resizeBrush(1);
    _picker->setDrawMode(M_FREEHAND);

    enableUponNew();
}

void MainWindow::openFile()
{
    /* saveUponNewOrOpen might be misleading, it returns
     * true when you want to proceed with making a
     * new project whether you save the old one or not
     * and false when you click cancel*/
    if(!saveUponNewOrOpen())
        return;

    QString filename = QFileDialog::getOpenFileName(this, "Open File", QString(), "CSV Files (*.csv)");
    //std::cout << filename.toStdString() << std::endl;
    if (filename == "")
        return;


    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        errorMsg("Couldn't open the file!");
    }
    QTextStream in(&file);

    QVector<QVector<QString> > contents;
    contents.clear();

    _hasStarter = false;
    Site *oldSite = _site;
    QGraphicsScene *oldScene = _scene;
    try {
        readCSV(contents, in);
        _site = parseCSV(contents);
    } catch (const char* e){
        errorMsg(e);
        return;
    }
    if(oldSite)
        delete oldSite;

    _projectName = filename;
    _width = _site->width();
    _height = _site->height();

    _scene = new QGraphicsScene(0, 0, _width*12, _height*12);
    populateScene();
    ui->digButton->setChecked(true);
    _picker->setCurrentDesignation(D_DIG);
    ui->graphicsView->setScene(_scene);
    if (oldScene)
       delete oldScene;
    _picker->setCurrentFloor(_site->topFloor());
    _picker->setTopFloor(_site->topFloor());
    _site->setCurrFloor(_site->topFloor());

    if (_hasStarter)
        _picker->setStarterTile(_starterTile);
    connectUponNew();

    _picker->sync();


}

//TODO: combine save and save as into one function somehow

void MainWindow::save()
{
    if (_savePath == "")
        _savePath = QFileDialog::getSaveFileName(this, tr("Save File"), _projectName, "CSV Files (*.csv)", nullptr, 0);
    saveProper();
    _title = QString(_projectName);
    _title.append(" - Dwarfchitect");
    setWindowTitle(_title);
    _unsavedChanges = false;
}

void MainWindow::saveAs()
{
    _savePath = QFileDialog::getSaveFileName(this, tr("Save File"), _projectName, "CSV Files (*.csv)", nullptr, 0);
    saveProper();
    _title = QString(_projectName);
    _title.append(" - Dwarfchitect");
    setWindowTitle(_title);
    _unsavedChanges = false;
}

void MainWindow::changeDesignationPreview(int s)
{
    switch (s) {
    case D_DIG:
        ui->designationPreview->setText("d");
        break;
    case D_CHANNEL :
        ui->designationPreview->setText("h");
        break;
    case D_UPDOWN_STAIR :
        ui->designationPreview->setText("i");
        break;
    case D_UP_STAIR :
        ui->designationPreview->setText("u");
        break;
    case D_DOWN_STAIR :
        ui->designationPreview->setText("j");
        break;
    case D_RAMP :
        ui->designationPreview->setText("r");
        break;
    case D_CLEAR :
        ui->designationPreview->setText("c");
        break;
    }
    ui->designationPreview->setAlignment(Qt::AlignJustify | Qt::AlignHCenter |Qt::AlignVCenter);
}

void MainWindow::toggleGrabMode(bool grabbing)
{
    if(grabbing){
        //std::cout << "GRABBING" << std::endl;
        ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        ui->graphicsView->setInteractive(false);
    } else {
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        ui->graphicsView->setInteractive(true);
    }
}

void MainWindow::readCSV(QVector<QVector<QString> > &contents, QTextStream &in)
{
    QVector<QString> row;
    QString s = QString();
    QChar c;
    while (!in.atEnd()){
        in >> c;
        switch (c.toLatin1()) {
        case ',':
            row.push_back(s);
            s = QString();
            break;
        case '\n':
            row.push_back(s);
            s = QString();
            contents.push_back(row);
            row.clear();
            break;
        default:
            s.append(c);
        }
    }
    if (contents.empty())
        throw "The CSV file is empty!";
}

/*kinda lenghty function, I'll see if I can break it down later*/
Site* MainWindow::parseCSV(QVector<QVector<QString> > &contents)
{
    /*checking if every row has the same number of columns*/

    //#>
    /*Calculating the width and height of the site*/
    int width = findCSVWidth(contents);
    int height = findCSVHeight(contents);

    //std::cout << height << ", " << width << std::endl;

    if(height == 0 || width == 0)
        throw ("The file only contains a header!");

    Site *newSite = new Site(width, height);
    Floor *f = newSite->currFloor();
    int floorH = 0;
    int floorW = 0;
    QVector<QString> row;
    QString s;
    /* some extra care while parsing the first row because of the quickfort
     * stadnard header */
    row = contents.takeFirst();
    s = row.takeFirst().trimmed();

    if(s.startsWith("#build") || s.startsWith("\"#build")
            ||s.startsWith("#query") || s.startsWith("\"#query"))
        throw "Can't parse a building/querying csv file!";
    else if (s.startsWith("#dig") || s.startsWith("\"#dig")){
        if (s.contains("start", Qt::CaseInsensitive) )
            extractStartTile(s);
    }
    /*
    QString qs("#dig start(30 45)");

    QRegExp catchStart = QRegExp("start\\((\\d+)\\s+(\\d+)\\)");
    if (catchStart.indexIn(qs) > -1){
        std::cout << "The REGEX works " << catchStart.cap(1).toStdString() << " " << catchStart.cap(2).toStdString() << std::endl;
    }
    */
    else if(s.length() < 2){
        f->tiles()[floorH][floorW].des = _csvMap[s];
        floorW++;
        while(!row.empty()){
            s = row.takeFirst().trimmed();
            f->tiles()[floorH][floorW].des = _csvMap[s];
            floorW++;
        }
    }

    floorW = 0;
    floorH = 0;

    /*main parsing loop*/
    Key k;
    while (!contents.empty()){
        row = contents.takeFirst();
        while (!row.empty()){
            s = row.takeFirst().trimmed();
            if(_csvMap.contains(s))
                k = _csvMap[s];
            else
                throw "Unknow designation in file!";
            switch (k) {
            case DOWN_Z :
                if(floorW !=0 || floorH != height)
                    throw "New floor command out of place in CSV file!";
                if (!contents.empty()){
                    row = contents.takeFirst();
                    floorH = 0;
                    floorW = 0;
                    newSite->addFloorBelowCurr();
                    newSite->moveCurrDown();
                    f = newSite->currFloor();
                    break;
                }
            case D_CLEAR :
                floorW++;
                break;
            default :
                f->tiles()[floorH][floorW].des = k;
                floorW++;
            }
        }
        floorH++;
        floorW = 0;
    }

    return newSite;

}

void MainWindow::extractStartTile(QString s)
{
    QRegExp catchStart = QRegExp("start\\((\\d+)\\s+(\\d+)\\)");

    if (catchStart.indexIn(s) < 0)
        throw ("Something is wrong with the starting tile information!");
    else{
        _starterTile = Coords(catchStart.cap(2).toInt(), catchStart.cap(1).toInt());
        //std::cout << "found a start at" << catchStart.cap(2).toStdString() << " " << catchStart.cap(1).toStdString() << std::endl;
        _hasStarter = true;
    }
}

void MainWindow::errorMsg(const char* s)
{
    QString ss(s);
    QMessageBox msg;
    msg.setIcon(QMessageBox::Warning);
    msg.setText(ss);
    msg.exec();
    return;
}

int MainWindow::findCSVWidth(QVector<QVector<QString> > &contents)
{
int cols = contents[0].size();
for (int i  = 0; i < contents.size(); i++){
    if (contents[i].size() != cols)
        throw "Some rows have unequal number of columns!";
}
return cols;
}

int MainWindow::findCSVHeight(QVector<QVector<QString> > &contents)
{
int height = 0;
bool multiFloor = false;
QString newFloorMarker("#>");

while (height < contents.size()){
    if (contents[height][0].trimmed() == newFloorMarker){
        multiFloor = true;
        break;
    }
    height++;
}


if(multiFloor){ /*gotta check if all floors are of same height now*/
    bool headerRow = false;
    int h2 = height + 1;
    if(contents[0][0].trimmed().startsWith("#dig")
            || contents[0][0].trimmed().startsWith("\"#dig")){
        height--;
        headerRow = true;
    }
    int f = 1;
    while (h2 < contents.size()){
        if(contents[h2][0].trimmed() == newFloorMarker){
            int damnOffset = (height + (headerRow ? 1 : 0)) * f + 1;
            if(height != (h2 - damnOffset))
                throw "Not all floors are of the same height!";
            f++;
        }
        h2++;
    }
}

return height;
}

void MainWindow::enableUponNew()
{
    ui->actionAdd_New_Bottom_Layer->setEnabled(true);
    ui->actionAdd_New_Layer_Above_Current->setEnabled(true);
    ui->actionAdd_New_Layer_Below_Current->setEnabled(true);
    ui->actionAdd_New_Top_Layer->setEnabled(true);
    ui->actionDelete_Current_Layer->setEnabled(true);
    ui->actionExport_Macro->setEnabled(true);
    ui->actionMove_Down->setEnabled(true);
    ui->actionMove_Up->setEnabled(true);
    ui->actionRedo->setEnabled(true);
    ui->actionReset_Zoom->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionSave_As->setEnabled(true);
    ui->actionSquare_Rectangle_render->setEnabled(true);
    ui->actionUndo->setEnabled(true);
    ui->actionZoom_in->setEnabled(true);
    ui->actionZoom_out->setEnabled(true);
}

void MainWindow::saveProper()
{
    Floor *c = _site->topFloor();
    //_savePath.append(".csv");
    QString withExt = QString(_savePath);
    QFile file(withExt.append(".csv"));

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
        QMessageBox *msg = new QMessageBox(this);
        msg->setIcon(QMessageBox::Warning);
        msg->setText("Couldn't Open the File!");
        delete msg;
        return;
    }

    QTextStream out(&file);

    out << "#dig";
    if (_picker->hasStarter()){
        out << " start(" << _picker->starterTile().x << " " << _picker->starterTile().y << ")";
    }
    for (int i = 1; i < _width; i++)
        out << ", ";
    out << '\n';
    while (c != nullptr){
        for (int i = 0; i < _height; i++){
            //out << _csvMap.key(c->tiles()[i][0].des);
            out << (c->tiles()[i][0].des == D_CLEAR ? " " : _csvMap.key(c->tiles()[i][0].des));
            for (int j = 1; j < _width; j++){
                //out << "," <<_csvMap.key(c->tiles()[i][j].des); clean this
                out << "," << (c->tiles()[i][j].des == D_CLEAR ? " " : _csvMap.key(c->tiles()[i][j].des));
            }
            out << '\n';
        }
        if(c->floorBelow()){
            out << "#>";
            for (int i = 1; i < _width; i++)
                out << "," << "#";
            out << '\n';
        }
        c = c->floorBelow();
    }
}

bool MainWindow::saveUponNewOrOpen()
{
if(_unsavedChanges){
    QMessageBox msgBox;
    QFont dorfs("DF Curses 8x12");
    dorfs.setPointSize(12);
    msgBox.setFont(dorfs);
    msgBox.setText("The document has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret){
    case QMessageBox::Save :
        save();
        return true;
    case QMessageBox::Discard :
        return true;
    case QMessageBox::Cancel :
        return false;
    }
}
return true;
}
