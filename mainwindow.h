#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "site.h"
#include "picker.h"
#include "designations.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    void populateScene();
    int tilesX;
    int tilesY;
    Picker *_picker;
    Site *_site;
private slots:
    void makeNew();
};

#endif // MAINWINDOW_H
