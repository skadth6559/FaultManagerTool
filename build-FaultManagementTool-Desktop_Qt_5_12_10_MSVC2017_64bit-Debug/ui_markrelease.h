/********************************************************************************
** Form generated from reading UI file 'markrelease.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MARKRELEASE_H
#define UI_MARKRELEASE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_markrelease
{
public:
    QDialogButtonBox *buttonBox;
    QTextBrowser *textBrowser;

    void setupUi(QDialog *markrelease)
    {
        if (markrelease->objectName().isEmpty())
            markrelease->setObjectName(QString::fromUtf8("markrelease"));
        markrelease->resize(465, 185);
        buttonBox = new QDialogButtonBox(markrelease);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(70, 130, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        textBrowser = new QTextBrowser(markrelease);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(40, 40, 371, 61));
        QFont font;
        font.setPointSize(12);
        textBrowser->setFont(font);

        retranslateUi(markrelease);
        QObject::connect(buttonBox, SIGNAL(accepted()), markrelease, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), markrelease, SLOT(reject()));

        QMetaObject::connectSlotsByName(markrelease);
    } // setupUi

    void retranslateUi(QDialog *markrelease)
    {
        markrelease->setWindowTitle(QApplication::translate("markrelease", "Dialog", nullptr));
        textBrowser->setHtml(QApplication::translate("markrelease", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:12pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Confirm that you would like to mark this release as released and update the database.</p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class markrelease: public Ui_markrelease {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MARKRELEASE_H
