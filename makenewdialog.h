#ifndef MAKENEWDIALOG_H
#define MAKENEWDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QString>
#include "ui_newprojectdialog_1.h"
#define EMBARK_TILES 48
class MakeNewDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MakeNewDialog(QWidget *parent = 0);
    ~MakeNewDialog();

    int levelWidth() const;

    int levelHeight() const;

    QString acquiredName() const;

signals:

public slots:
    void somethin();
    void accept();

private:
    bool isValidInput();
    QString _acquiredName;
    Ui::Dialog *ui;
    int _levelWidth;
    int _levelHeight;
};

#endif // MAKENEWDIALOG_H
