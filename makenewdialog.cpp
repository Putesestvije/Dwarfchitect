#include "makenewdialog.h"
#include <iostream>
#include <QLineEdit>
#include <QComboBox>
#include <QIntValidator>
#include <QErrorMessage>
#include <QMessageBox>

MakeNewDialog::MakeNewDialog(QWidget *parent) : ui(new Ui::Dialog)
{
    setParent(parent);
    ui->setupUi(this);
    QIntValidator *validator = new QIntValidator();
    validator->setBottom(1);
    ui->widthEdit->setValidator(validator);
    ui->heightEdit->setValidator(validator);
    ui->widthEdit->setText("144");
    ui->heightEdit->setText("144");
}

int MakeNewDialog::levelWidth() const
{
    return _levelWidth;
}

int MakeNewDialog::levelHeight() const
{
    return _levelHeight;
}

void MakeNewDialog::somethin()
{
    std::cout << "ha ha ha" << std::endl;
}

void MakeNewDialog::accept()
{
    if(!isValidInput())
        return;

    done(QDialog::Accepted);
}

bool MakeNewDialog::isValidInput()
{
    /*Warning: bunch of dumb boilerplate*/

    QMessageBox *msg = new QMessageBox(this);
    msg->setIcon(QMessageBox::Warning);
    QString emptyFieldMsg("Please enter a value in the %1 field%2");
    QString outOfBoundsMsg("%1 is out of bounds. "
                           "A number between 1 and %2 is required. "
                           "Closest value inserted.");

    /*Checking if there is anything in the nameEdits*/
    if(ui->widthEdit->text().length() == 0){
        msg->setText(emptyFieldMsg.arg("width").arg(""));
        if(ui->heightEdit->text().length() == 0){
            msg->setText(emptyFieldMsg.arg("width and height").arg("s"));
        }
        msg->exec();
        delete msg;
        return false;
    }
    if(ui->heightEdit->text().length() == 0){
        msg->setText(emptyFieldMsg.arg("height").arg(""));
        msg->exec();
        delete msg;
        return false;
    }
    if(ui->nameEdit->text().length() == 0){
        msg->setText("Please enter a name for your project");
        msg->exec();
        delete msg;
        return false;
    }

    int tempWidth = ui->widthEdit->text().toInt();
    int tempHeight = ui->heightEdit->text().toInt();

    int isEmbarkWidth = ui->widthType->currentIndex();
    int isEmbarkHeight = ui->heightType->currentIndex();

    /*checking if the amount of tiles is out of bounds*/
    if((isEmbarkWidth && (tempWidth > 16)) || (tempWidth > 16 * EMBARK_TILES) || (tempWidth == 0)){
        msg->setText(outOfBoundsMsg.arg("Width").arg(isEmbarkWidth ? "16" : "768"));
        msg->exec();
        ui->widthEdit->setText((tempWidth == 0) ? "1" :
                                                  isEmbarkWidth ? "16" : "768");
        delete msg;
        return false;
    }
    if((isEmbarkHeight && (tempHeight > 16)) || (tempHeight > 16 * EMBARK_TILES) || (tempHeight == 0)){
        msg->setText(outOfBoundsMsg.arg("Height").arg(isEmbarkHeight ? "16" : "768"));
        msg->exec();
        ui->heightEdit->setText((tempHeight == 0) ? "1" :
                                                  isEmbarkHeight ? "16" : "768");
        delete msg;
        return false;
    }
    _levelWidth = (isEmbarkWidth ? tempWidth*EMBARK_TILES : tempWidth);
    _levelHeight = (isEmbarkHeight ? tempHeight*EMBARK_TILES : tempHeight);
    _acquiredName = ui->nameEdit->text();


    delete msg;
    return true;
}

QString MakeNewDialog::acquiredName() const
{
    return _acquiredName;
}

MakeNewDialog::~MakeNewDialog(){
    delete ui;
}
