#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QProgressBar>
#include "site.h"
#include "picker.h"
#include "designations.h"
#include "view.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void status(QString status);
private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    void populateScene();
    int _width;
    int _height;
    QString _projectName;
    Picker *_picker;
    Site *_site;
    QProgressBar *progressBar;
    GraphicsView *_graphicView;
private slots:
    void makeNew();
    void exportMacro();
    void progressed(int p);
};

#endif // MAINWINDOW_H
