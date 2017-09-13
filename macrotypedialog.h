#ifndef MACROTYPEDIALOG_H
#define MACROTYPEDIALOG_H

#include <QDialog>

namespace Ui {
class macroTypeDialog;
}

class macroTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit macroTypeDialog(QWidget *parent = 0);
    ~macroTypeDialog();

    bool fromTopLeftCorner();
    bool fromTopmostLeftmost();
    bool fromStarter();

    void disableStart();

private:
    Ui::macroTypeDialog *ui;
};

#endif // MACROTYPEDIALOG_H
