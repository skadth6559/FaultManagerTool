#include "deletefault.h"
#include "ui_deletefault.h"

deletefault::deletefault(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::deletefault)
{
    ui->setupUi(this);
}

deletefault::~deletefault()
{
    delete ui;
}
