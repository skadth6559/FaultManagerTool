#ifndef DELETEFAULT_H
#define DELETEFAULT_H

#include <QDialog>

namespace Ui {
class deletefault;
}

class deletefault : public QDialog
{
    Q_OBJECT

public:
    explicit deletefault(QWidget *parent = nullptr);
    ~deletefault();

private:
    Ui::deletefault *ui;
};

#endif // DELETEFAULT_H
