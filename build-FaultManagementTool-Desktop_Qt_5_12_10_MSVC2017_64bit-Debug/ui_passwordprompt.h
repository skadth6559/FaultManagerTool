/********************************************************************************
** Form generated from reading UI file 'passwordprompt.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PASSWORDPROMPT_H
#define UI_PASSWORDPROMPT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_passwordprompt
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QLineEdit *lineEdit;

    void setupUi(QDialog *passwordprompt)
    {
        if (passwordprompt->objectName().isEmpty())
            passwordprompt->setObjectName(QString::fromUtf8("passwordprompt"));
        passwordprompt->resize(400, 168);
        buttonBox = new QDialogButtonBox(passwordprompt);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 120, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(passwordprompt);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 321, 31));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        lineEdit = new QLineEdit(passwordprompt);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(20, 60, 351, 20));

        retranslateUi(passwordprompt);
        QObject::connect(buttonBox, SIGNAL(accepted()), passwordprompt, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), passwordprompt, SLOT(reject()));

        QMetaObject::connectSlotsByName(passwordprompt);
    } // setupUi

    void retranslateUi(QDialog *passwordprompt)
    {
        passwordprompt->setWindowTitle(QApplication::translate("passwordprompt", "Dialog", nullptr));
        label->setText(QApplication::translate("passwordprompt", "Please enter the password:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class passwordprompt: public Ui_passwordprompt {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PASSWORDPROMPT_H
