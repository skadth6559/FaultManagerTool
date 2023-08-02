/********************************************************************************
** Form generated from reading UI file 'save_add.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAVE_ADD_H
#define UI_SAVE_ADD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_save_add
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_3;
    QSpacerItem *verticalSpacer;
    QLabel *label_2;
    QPlainTextEdit *reasonPT;
    QSpacerItem *verticalSpacer_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *save_add)
    {
        if (save_add->objectName().isEmpty())
            save_add->setObjectName(QString::fromUtf8("save_add"));
        save_add->resize(497, 236);
        save_add->setModal(false);
        verticalLayout = new QVBoxLayout(save_add);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_3 = new QLabel(save_add);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label_3->setFont(font);

        verticalLayout->addWidget(label_3);

        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer);

        label_2 = new QLabel(save_add);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QFont font1;
        font1.setPointSize(10);
        label_2->setFont(font1);

        verticalLayout->addWidget(label_2);

        reasonPT = new QPlainTextEdit(save_add);
        reasonPT->setObjectName(QString::fromUtf8("reasonPT"));
        reasonPT->setFont(font1);

        verticalLayout->addWidget(reasonPT);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_2);

        buttonBox = new QDialogButtonBox(save_add);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(save_add);
        QObject::connect(buttonBox, SIGNAL(accepted()), save_add, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), save_add, SLOT(reject()));

        QMetaObject::connectSlotsByName(save_add);
    } // setupUi

    void retranslateUi(QDialog *save_add)
    {
        save_add->setWindowTitle(QApplication::translate("save_add", "Dialog", nullptr));
        label_3->setText(QApplication::translate("save_add", "Confirm Save", nullptr));
        label_2->setText(QApplication::translate("save_add", "Reason for adding (optional):", nullptr));
    } // retranslateUi

};

namespace Ui {
    class save_add: public Ui_save_add {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAVE_ADD_H
