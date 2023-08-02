#ifndef SAVE_ADD_H
#define SAVE_ADD_H

#include <QDialog>
#include "qplaintextedit.h"

namespace Ui {
class save_add;
}

class save_add : public QDialog
{
    Q_OBJECT

public:
    explicit save_add(QWidget *parent = nullptr);
    ~save_add();
    QPlainTextEdit * getReasonText();

private:
    Ui::save_add *ui;
};

#endif // SAVE_ADD_H
