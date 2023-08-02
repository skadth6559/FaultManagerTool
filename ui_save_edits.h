/********************************************************************************
** Form generated from reading UI file 'save_edits.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAVE_EDITS_H
#define UI_SAVE_EDITS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_save_edits
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_3;
    QSpacerItem *verticalSpacer;
    QSpacerItem *verticalSpacer_5;
    QLabel *label;
    QTextBrowser *changesBrowser;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_2;
    QPlainTextEdit *reasonPT;
    QSpacerItem *verticalSpacer_4;
    QSpacerItem *verticalSpacer_3;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *save_edits)
    {
        if (save_edits->objectName().isEmpty())
            save_edits->setObjectName(QString::fromUtf8("save_edits"));
        save_edits->resize(497, 522);
        verticalLayout = new QVBoxLayout(save_edits);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_3 = new QLabel(save_edits);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label_3->setFont(font);

        verticalLayout->addWidget(label_3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        verticalSpacer_5 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_5);

        label = new QLabel(save_edits);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setPointSize(10);
        label->setFont(font1);

        verticalLayout->addWidget(label);

        changesBrowser = new QTextBrowser(save_edits);
        changesBrowser->setObjectName(QString::fromUtf8("changesBrowser"));
        changesBrowser->setMinimumSize(QSize(0, 200));
        changesBrowser->setFont(font1);

        verticalLayout->addWidget(changesBrowser);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_2);

        label_2 = new QLabel(save_edits);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font1);

        verticalLayout->addWidget(label_2);

        reasonPT = new QPlainTextEdit(save_edits);
        reasonPT->setObjectName(QString::fromUtf8("reasonPT"));
        reasonPT->setFont(font1);

        verticalLayout->addWidget(reasonPT);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_4);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        buttonBox = new QDialogButtonBox(save_edits);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(save_edits);
        QObject::connect(buttonBox, SIGNAL(accepted()), save_edits, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), save_edits, SLOT(reject()));

        QMetaObject::connectSlotsByName(save_edits);
    } // setupUi

    void retranslateUi(QDialog *save_edits)
    {
        save_edits->setWindowTitle(QApplication::translate("save_edits", "Dialog", nullptr));
        label_3->setText(QApplication::translate("save_edits", "Confirm Save", nullptr));
        label->setText(QApplication::translate("save_edits", "Changes made to this fault:", nullptr));
        label_2->setText(QApplication::translate("save_edits", "Reason for making these edits (optional):", nullptr));
    } // retranslateUi

};

namespace Ui {
    class save_edits: public Ui_save_edits {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAVE_EDITS_H
