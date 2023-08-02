#ifndef SAVE_EDITS_H
#define SAVE_EDITS_H

#include <QDialog>
#include "qplaintextedit.h"
#include "qtextbrowser.h"

namespace Ui {
class save_edits;
}

class save_edits : public QDialog
{
    Q_OBJECT

public:
    explicit save_edits(QWidget *parent = nullptr);
    ~save_edits();
    QPlainTextEdit * getReasonText();
    QTextBrowser * getChangeBrowser();

private:
    Ui::save_edits *ui;
};

#endif // SAVE_EDITS_H
