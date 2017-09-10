#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QProgressBar>
#include <QTextStream>
#include <exception>
#include <QSignalMapper>
#include <QButtonGroup>
#include <QHash>
#include <QTextEdit>
#include "site.h"
#include "picker.h"
#include "designations.h"
#include "graphicsview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Coords starterTile() const;
    void setStarterTile(const Coords &starterTile);

    bool hasStarter() const;
    void setHasStarter(bool hasStarter);

public slots:
    void status(QString status);
    void toggleUnsavedChanges();
private:
    Ui::MainWindow *ui;
    QGraphicsScene *_scene;
    void populateScene();
    int _width;
    int _height;
    QString _projectName;
    Picker *_picker;
    Site *_site;
    QProgressBar *_progressBar;
    GraphicsView *_graphicView;
    QHash<QString, Key> _csvMap;
    QString _savePath;
    QString _title;
    bool _unsavedChanges;
    void closeEvent(QCloseEvent *event);
    QSignalMapper *_buttonToDesignation;
    QSignalMapper *_buttonToDrawMode;
    QSignalMapper *_buttonToBrushType;
    QButtonGroup *_designationButtons;
    QButtonGroup *_drawButtons;
    QButtonGroup *_brushButtons;
    QTextEdit *_designationPreviewold;


    /* This is some very bad design but I can't come up with anything else
     * right now. Both MainWIndow and Picker have the hasStarter bool and
     * the coordinates themselves*/
    Coords _starterTile;
    bool _hasStarter;

    void initCsvMap();
    void readCSV(QVector<QVector<QString> > &contents, QTextStream &in);
    Site *parseCSV(QVector<QVector<QString> > &contents);
    void errorMsg(const char *s);
    int findCSVWidth(QVector<QVector<QString> > &contents);
    int findCSVHeight(QVector<QVector<QString> > &contents);
    void enableUponNew();

    void saveProper();

    bool saveUponNewOrOpen();
private slots:
    void makeNew();
    void exportMacro();
    void progressed(int p);
    void connectUponNew();
    void openFile();
    void save();
    void saveAs();
    void changeDesignationPreview(int s);
    void toggleGrabMode(bool grabbing);
};

#endif // MAINWINDOW_H

