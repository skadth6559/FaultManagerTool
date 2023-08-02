#include "save_edits.h"
#include "ui_save_edits.h"

save_edits::save_edits(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::save_edits)
{
    ui->setupUi(this);
}

save_edits::~save_edits()
{
    delete ui;
}

QPlainTextEdit * save_edits::getReasonText() {
    return ui->reasonPT;
}

QTextBrowser * save_edits::getChangeBrowser() {
    return ui->changesBrowser;
}
