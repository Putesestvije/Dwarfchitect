#include "macrotypedialog.h"
#include "ui_macrotypedialog.h"

macroTypeDialog::macroTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::macroTypeDialog)
{
    ui->setupUi(this);
}

macroTypeDialog::~macroTypeDialog()
{
    delete ui;
}

bool macroTypeDialog::fromTopLeftCorner()
{
    return ui->topLeftCorner->isChecked();
}

bool macroTypeDialog::fromTopmostLeftmost()
{
    return ui->topmostLeftmost->isChecked();
}

bool macroTypeDialog::fromStarter()
{
    return ui->starter->isChecked();
}

void macroTypeDialog::disableStart()
{
    ui->starter->setEnabled(false);
}
