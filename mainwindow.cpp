#include <QFontDatabase>
#include <QFormBuilder>
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
#include <iostream>
#include <QDialog>
#include <QVector>
#include <QString>
#include <string>
#include <QFile>
#include <QMap>

#include "maximalrectangle.h"
#include "ui_mainwindow.h"
#include "makenewdialog.h"
#include "mainwindow.h"
#include "tileface.h"
#include "view.h"

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
    centralWidget()->layout()->addWidget(_graphicView);
    _unsavedChanges = false;
    _title = QString("Dwarfchitect");
    setWindowTitle(_title);

    int id = QFontDatabase::addApplicationFont(":/DF_Curses_8x12.ttf");
    int id2 = QFontDatabase::addApplicationFont(":/Px437_IBM_BIOS.ttf");
    int id3 = QFontDatabase::addApplicationFont(":/Px437_IBM_BIOS-2y.ttf");
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
    connect(ui->actionZoom_in, &QAction::triggered, _graphicView, &GraphicsView::zoomIn);
    connect(ui->actionZoom_out, &QAction::triggered, _graphicView, &GraphicsView::zoomOut);

    connect(ui->actionSquare_Rectangle_render, &QAction::triggered, _graphicView, &GraphicsView::stretchToggle);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveAs);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    QWidget *qdg = new QWidget();
    qdg->setFixedHeight(60);
    qdg->setFixedWidth(175);
    QHBoxLayout *qhb = new QHBoxLayout();
    QGridLayout *qgl = new QGridLayout();

    qgl->setVerticalSpacing(15);
    qgl->setHorizontalSpacing(10);

    QPushButton *digB = new QPushButton();
    QIcon digI(":/icons/dig.png");
    digB->setShortcut(QKeySequence("d"));
    digB->setIcon(digI);
    digB->setCheckable(true);
    digB->setChecked(true);
    digB->setFixedSize(20,20);
    qgl->addWidget(digB, 0, 0);

    QPushButton *channelB = new QPushButton();
    QIcon channelI(":/icons/channel.png");
    channelB->setShortcut(QKeySequence("h"));
    channelB->setCheckable(true);
    channelB->setIcon(channelI);
    channelB->setFixedSize(20,20);
    qgl->addWidget(channelB, 1, 0);

    QPushButton *upDownStairsB = new QPushButton();
    QIcon upDownStairsI(":/icons/stairsupdown.png");
    upDownStairsB->setShortcut(QKeySequence("i"));
    upDownStairsB->setCheckable(true);
    upDownStairsB->setIcon(upDownStairsI);
    upDownStairsB->setFixedSize(20,20);
    qgl->addWidget(upDownStairsB, 0, 1);

    QPushButton *upStairsB = new QPushButton();
    QIcon upStairsI(":/icons/stairsup.png");
    upStairsB->setShortcut(QKeySequence("u"));
    upStairsB->setCheckable(true);
    upStairsB->setIcon(upStairsI);
    upStairsB->setFixedSize(20,20);
    qgl->addWidget(upStairsB, 1, 1);

    QPushButton *downStairsB = new QPushButton();
    QIcon downStairsI(":/icons/stairsdown.png");
    downStairsB->setShortcut(QKeySequence("j"));
    downStairsB->setCheckable(true);
    downStairsB->setIcon(downStairsI);
    downStairsB->setFixedSize(20,20);
    qgl->addWidget(downStairsB, 0, 2);

    QPushButton *rampUpB = new QPushButton();
    QIcon rampUpI(":/icons/rampup.png");
    rampUpB->setShortcut(QKeySequence("r"));
    rampUpB->setCheckable(true);
    rampUpB->setIcon(rampUpI);
    rampUpB->setFixedSize(20,20);
    qgl->addWidget(rampUpB, 1, 2);

    _buttonToDesignation = new QSignalMapper(this);

    _buttonToDesignation->setMapping(digB, D_DIG);
    _buttonToDesignation->setMapping(channelB, D_CHANNEL);
    _buttonToDesignation->setMapping(upDownStairsB, D_UPDOWN_STAIR);
    _buttonToDesignation->setMapping(upStairsB, D_UP_STAIR);
    _buttonToDesignation->setMapping(downStairsB, D_DOWN_STAIR);
    _buttonToDesignation->setMapping(rampUpB, D_RAMP);



    connect(digB, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));
    connect(channelB, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));
    connect(upDownStairsB, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));
    connect(upStairsB, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));
    connect(downStairsB, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));
    connect(rampUpB, SIGNAL(toggled(bool)), _buttonToDesignation, SLOT(map()));

    _designationButtons = new QButtonGroup(this);

    _designationButtons->setExclusive(true);
    _designationButtons->addButton(digB);
    _designationButtons->addButton(channelB);
    _designationButtons->addButton(upDownStairsB);
    _designationButtons->addButton(upStairsB);
    _designationButtons->addButton(downStairsB);
    _designationButtons->addButton(rampUpB);

    qhb->addItem(qgl);

    _designationPreview = new QTextEdit(this);
    _designationPreview->setFixedSize(60, 60);
    _designationPreview->setReadOnly(true);
    QFont font = QFont("DF Curses 8x12");
    font.setPointSize(45);
    _designationPreview->setFont(font);
    _designationPreview->setText("d");
    _designationPreview->setAlignment(Qt::AlignJustify | Qt::AlignHCenter |Qt::AlignVCenter);

    connect(_buttonToDesignation, SIGNAL(mapped(int)), this, SLOT(changeDesignationPreview(int)));

    qhb->addWidget(_designationPreview);


    //QLabel *designationPreview = new QLabel();


    qdg->setLayout(qhb);
    ui->designationsToolBar->addWidget(qdg);

    //ui->desigantionsToolBar->set
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _site;
    delete _picker;
    delete _graphicView;
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

void MainWindow::populateScene()
{
    std::vector<std::vector<TileFace*> > *f = new std::vector<std::vector<TileFace*> >();
    f->resize(_height);
    for(int i = 0; i < _height; i++)
        (*f)[i].resize(_width);
    _picker = new Picker(_width * 12, _height * 12,  f);
    _picker->setPos(0, 0);

    _scene = new QGraphicsScene(this);
    _scene->addItem(_picker);
    for (int i = 0; i < _height; i++){
        for (int j = 0; j < _width; j++){
            QColor qcl(255, 255, 255);
            TileFace *t = new TileFace(qcl, i, j);
            t->setPos(QPointF(j*12,i*12));
            _scene->addItem(t);
            (*f)[i][j] = t;
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (_unsavedChanges){
        QMessageBox msgBox;
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

void MainWindow::initCsvMap()
{
    _csvMap[" "] = CLEAR;
    _csvMap[""] = CLEAR;
    _csvMap["'"] = CLEAR;
    _csvMap["`"] = CLEAR;
    _csvMap["~"] = CLEAR;
    _csvMap["#"] = CLEAR;
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
    /* this function might be misleading, it returns
     * true when you want to proceed with making a
     * new project wheter you save old one or not
     * and false when you click cancel*/
    if(!saveUponNewOrOpen())
        return;

    int returnCode;
    MakeNewDialog *makeNew = new MakeNewDialog();

    returnCode = makeNew->exec();

    if(returnCode){
        _width = makeNew->levelWidth();
        _height = makeNew->levelHeight();
        _projectName = makeNew->acquiredName();
        QGraphicsScene *oldScene = _scene;
        populateScene();
        _graphicView->setScene(_scene);
        if (oldScene)
           delete oldScene;

        if(_site)
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
    MaximalRectangle maxRec(_site->topFloor(), _width, _height, this, _projectName);
    _progressBar->setMaximum(maxRec.amountOfWork()*2);
    _progressBar->setValue(0);

    _progressBar->show();
    connect(&maxRec, &MaximalRectangle::progessed, this, &MainWindow::progressed);
    connect(&maxRec, &MaximalRectangle::syncFaces, _picker, &Picker::sync);

    maxRec.generateMacro();

    std::cout << _progressBar->maximum() << " ";
    std::cout << _progressBar->value() << std::endl;

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

    connect(_picker, &Picker::mousePosition, this, &MainWindow::status);
    connect(_picker, &Picker::changesMadeToModel, this, &MainWindow::toggleUnsavedChanges);

    connect(_buttonToDesignation, SIGNAL(mapped(int)), _picker, SLOT(setCurrentDesignation(int)));
}

void MainWindow::openFile()
{
    /* this function might be misleading, it returns
     * true when you want to proceed with making a
     * new project wheter you save old one or not
     * and false when you click cancel*/
    if(!saveUponNewOrOpen())
        return;

    QString filename = QFileDialog::getOpenFileName(this, "Open File", QString(), "CSV Files (*.csv)");
    std::cout << filename.toStdString() << std::endl;
    if (filename == "")
        return;


    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        errorMsg("Couldn't open the file!");
    }
    QTextStream in(&file);

    QVector<QVector<QString> > contents;
    contents.clear();
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

    populateScene();
    _graphicView->setScene(_scene);
    if (oldScene)
       delete oldScene;
    _picker->setCurrentFloor(_site->topFloor());
    _site->setCurrFloor(_site->topFloor());
    connectUponNew();

    _picker->sync();

}

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
        _designationPreview->setText("d");
        break;
    case D_CHANNEL :
        _designationPreview->setText("h");
        break;
    case D_UPDOWN_STAIR :
        _designationPreview->setText("i");
        break;
    case D_UP_STAIR :
        _designationPreview->setText("u");
        break;
    case D_DOWN_STAIR :
        _designationPreview->setText("j");
        break;
    case D_RAMP :
        _designationPreview->setText("r");
        break;
    }
    _designationPreview->setAlignment(Qt::AlignJustify | Qt::AlignHCenter |Qt::AlignVCenter);
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

    std::cout << height << ", " << width << std::endl;

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
    else if( s.startsWith("#dig") || s.startsWith("\"#dig"))
        ;
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
            case CLEAR :
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

void MainWindow::errorMsg(const char* s)
{
    QString ss(s);
    QMessageBox *msg = new QMessageBox(this);
    msg->setIcon(QMessageBox::Warning);
    msg->setText(ss);
    msg->exec();
    delete msg;
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
    for (int i = 1; i < _width; i++)
        out << ", ";
    out << '\n';
    while (c != nullptr){
        for (int i = 0; i < _height; i++){
            //out << _csvMap.key(c->tiles()[i][0].des);
            out << (c->tiles()[i][0].des == CLEAR ? " " : _csvMap.key(c->tiles()[i][0].des));
            for (int j = 1; j < _width; j++){
                //out << "," <<_csvMap.key(c->tiles()[i][j].des);
                out << "," << (c->tiles()[i][j].des == CLEAR ? " " : _csvMap.key(c->tiles()[i][j].des));
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
