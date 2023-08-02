#include "save_add.h"
#include "ui_save_add.h"

save_add::save_add(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::save_add)
{
    ui->setupUi(this);
}

save_add::~save_add()
{
    delete ui;
}

QPlainTextEdit * save_add::getReasonText() {
    return ui->reasonPT;
}
