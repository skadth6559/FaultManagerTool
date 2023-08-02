#ifndef QCUSTOMPUSHBUTTON_H
#define QCUSTOMPUSHBUTTON_H

#include <QPushButton>

class QCustomPushButton : public QPushButton
{
    Q_OBJECT
public:
    QCustomPushButton();
    QString field;
public slots:
    void emitClickedData();
Q_SIGNALS:
    void clickedData(QString field);
};

#endif // QCUSTOMPUSHBUTTON_H
