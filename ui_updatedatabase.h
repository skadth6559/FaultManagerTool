/********************************************************************************
** Form generated from reading UI file 'updatedatabase.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UPDATEDATABASE_H
#define UI_UPDATEDATABASE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>

QT_BEGIN_NAMESPACE

class Ui_updatedatabase
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_8;
    QPlainTextEdit *plainTextEdit;

    void setupUi(QDialog *updatedatabase)
    {
        if (updatedatabase->objectName().isEmpty())
            updatedatabase->setObjectName(QString::fromUtf8("updatedatabase"));
        updatedatabase->resize(649, 420);
        buttonBox = new QDialogButtonBox(updatedatabase);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(290, 350, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_8 = new QLabel(updatedatabase);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(20, 20, 271, 21));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label_8->setFont(font);
        plainTextEdit = new QPlainTextEdit(updatedatabase);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(20, 60, 611, 261));
        QFont font1;
        font1.setPointSize(10);
        plainTextEdit->setFont(font1);

        retranslateUi(updatedatabase);
        QObject::connect(buttonBox, SIGNAL(accepted()), updatedatabase, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), updatedatabase, SLOT(reject()));

        QMetaObject::connectSlotsByName(updatedatabase);
    } // setupUi

    void retranslateUi(QDialog *updatedatabase)
    {
        updatedatabase->setWindowTitle(QApplication::translate("updatedatabase", "Dialog", nullptr));
        label_8->setText(QApplication::translate("updatedatabase", "SUBMIT CHANGES?", nullptr));
        plainTextEdit->setPlainText(QApplication::translate("updatedatabase", "[Insert changes made]\n"
"\n"
"Example changes:\n"
"\n"
"ADD -\n"
"FaultID1\n"
"FaultID2\n"
"\n"
"EDIT -\n"
"FaultID3: WCA Severity changed to Caution\n"
"", nullptr));
    } // retranslateUi

};

namespace Ui {
    class updatedatabase: public Ui_updatedatabase {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UPDATEDATABASE_H
