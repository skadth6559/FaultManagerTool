#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mymodel.h"
#include "qcustompushbutton.h"
#include <iostream>
#include <QApplication>
#include <QMessageBox>
#include <QStringList>
#include <QInputDialog>
#include <QTimer>
#include <QColorDialog>
#include <QVariant>
#include <map>
#include <utility>
#include <regex>

//store total number of columns
const int dbcolumns = 24;
const int failsafedbcolumns = 21;
const int interlockdbcolumns = 9;
const int systemnotesdbcolumns = 13;

const QString database = "fmtooldb"; //used if database from ini file is not used
const QString highlightColor = "#fff799";

//enums reflecting the order of columns in the database
enum faultfields {id, fmStatusCB, groupLE, enumNameLE, faultCodeLE, changeeLE,
            faultGroupLE, configurationCB, wcaSeverityCB, wcaMessagePT, wcaTriggerPT, wcaClearingPT, failureImpactPT, failsafeCodeLE, interlockCodeLE,
                  failsafeSummaryPT, onboardLCSPT, offboardLCSPT, lcsLRPT, correctiveMaintenanceActions,
            wcaNotes, deviceLE, notesPT, changeLogBrowser};

enum failsafefields {fmStatus2CB, failsafeIDLE, configurationFailCB, failureNamePT, operationalStatePT, triggersPT, triggeringFaultsPT, failsafeClearingPT, indicationMethodPT, failsafeNotesPT, functionalImpactPT, racLE, firstResponsePT,
                    secondResponsePT, thirdResponsePT, failsafeSummaryFailPT, applicableSafetyPreceptLE, mitigatedRACLE, internalNotesFailPT, changeeFailLE, changeLog2Browser};

enum interlockfields {fmStatus3CB, interlockIDLE, configurationIntCB, componentsCB, interlockDescriptionPT, associatedWcasPT, internalNotesIntPT, changeeIntLE, changeLog3Browser};

enum systemnotesfields {id2, fmStatus4CB, configurationSysCB, operatorCB, summarySysLE, descriptionSysPT, workaroundsPT, externalNotesSysPT, internalNotesSysPT, faultGroupSysCB, pcrNumLE, changeeSysLE, changeLogSysBrowser };

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), myModel(new MyModel(this))
{
    ui->setupUi(this);
    ui->deleteButton->setVisible(false); //comment out this line for 'Erase Row' button to appear on main screen (fully implemented)
    showMaximized();

    saveEdit = new save_edits;
    saveAdd = new save_add;

    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(selectDelete()));

    exporter = new Exporter(myModel);

    ui->tableView->setModel(myModel);
    ui->tableView->setAlternatingRowColors(false);
    ui->tableView->setStyleSheet(headerStyleSheet);
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    for (auto e : myModel->columnMap.value("header_name").keys()) {
        ui->categoryComboBox->addItem(e);
    }
    ui->categoryComboBox->setCurrentText("WCAs");

    QCoreApplication::instance()->installEventFilter(this);

    // Update dropdown with currently available releases
    updateReleasesDropdown(ui->versionComboBox->currentText());

    updateViewMenu("");

    if (myModel->settings->contains("views/previous_release")) {
        ui->categoryComboBox->setCurrentText(myModel->settings->value("views/previous_category").toString());
        ui->versionComboBox->setCurrentText(myModel->settings->value("views/previous_release").toString());
        on_versionComboBox_activated(ui->versionComboBox->currentText());
    }

    ui->searchLineEdit->setPlaceholderText("Search...");

    ui->tableView->horizontalHeader()->setHighlightSections(false);
}

MainWindow::~MainWindow()
{
    myModel->settings->setValue("views/previous_release", ui->versionComboBox->currentText());
    myModel->settings->setValue("views/previous_category", ui->categoryComboBox->currentText());
    myModel->settings->sync();
    delete ui;
}

//returns the correct tab widget based on the category of data
QTabWidget* MainWindow::getTabWidget() {
    QTabWidget *temp;

    if (ui->categoryComboBox->currentText() == "Failsafes") {
        temp = ui->tabWidget_3;
    } else if (ui->categoryComboBox->currentText() == "Interlocks") {
        temp = ui->tabWidget_4;
    } else if (ui->categoryComboBox->currentText() == "System Notes") {
        temp = ui->tabWidget_5;
    } else {
        temp = ui->tabWidget_2;
    }

    return temp;
}

//ghosts save buttons based on whether information was entered when creating new entry
bool MainWindow::updateSaveButtons() {
    bool result = false;
    QTabWidget *temp = getTabWidget();

    if (ui->faultViewLabel->text() == "ADD FAULT" || ui->failsafeViewLabel->text() == "ADD FAILSAFE" || ui->interlockViewLabel->text() == "ADD INTERLOCK" || ui->systemNoteViewLabel->text() == "ADD SYSTEM NOTE") {
        //figure out whether any fields were changed
        foreach (QLineEdit* le, temp->findChildren<QLineEdit*>()) {
            //there is something to save
            if (le->text().isEmpty() == false && le->accessibleName() != "") result = true;
        }

        foreach (QPlainTextEdit* pt, temp->findChildren<QPlainTextEdit*>()) {
            //there is something to save
            if (pt->toPlainText().isEmpty() == false) result = true;
        }

        //implement the save buttons turning on when combobox is changed, requires comparison
        foreach (QComboBox* cb, temp->findChildren<QComboBox*>()) {
            //query data for each combo box
            //QString stored = myModel->getField(cb->accessibleName(), database);

            //compare query result with current text, result = true if not equal
            if (cb->currentIndex() != -1) result = true;
        }

        ui->saveMCMUSVButton->setEnabled(result);
        ui->saveWCAButton->setEnabled(result);
        ui->saveResponseButton->setEnabled(result);
        ui->saveOtherButton->setEnabled(result);
        ui->saveFailButton->setEnabled(result);
        ui->saveGeneralFailButton->setEnabled(result);
        ui->saveResponseFailButton->setEnabled(result);
        ui->saveOtherFailButton->setEnabled(result);
        ui->saveIntButton->setEnabled(result);
        ui->saveGeneralSysButton->setEnabled(result);
        ui->saveNotesSysButton->setEnabled(result);
    }

    return result;
}

//brings up the main page and refreshes the data
void MainWindow::setHomePage() {
    on_versionComboBox_activated(ui->versionComboBox->currentText());
    ui->stackedWidget->setCurrentIndex(0);
    ui->tabWidget_2->setCurrentIndex(0);
    ui->tabWidget_3->setCurrentIndex(0);
    ui->tabWidget_4->setCurrentIndex(0);
    ui->tabWidget_5->setCurrentIndex(0);
    clearFields();
}

//save button listeners, this function handles all the listeners and determines whether save or add as well as data category
//Also calls the respective function to ask for a changelog reason and begin save/add
void MainWindow::showInfoPrompt() {
    //get version
    QString version = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&version);
    QString text;
    QItemSelectionModel *select = ui->tableView->selectionModel();

    //check that the id follows required format and then start adding item
    if (ui->faultViewLabel->text() == "ADD FAULT" && ui->stackedWidget->currentIndex() == 1) {
        if (!validateData(Exporter::WCAS, version, ""))
            startAddFault(version);
    } else if (ui->faultViewLabel->text().contains("EDIT FAULT") && ui->stackedWidget->currentIndex() == 1) {
        if (!validateData(Exporter::WCAS, version, select->selectedRows(4).value(0).data().toString()))
            startEditFault(version);
    } else if (ui->failsafeViewLabel->text() == "ADD FAILSAFE" && ui->stackedWidget->currentIndex() == 4) {
        if (!validateData(Exporter::FAILSAFES, version + "_failsafes", ""))
            startAddFailsafe(version + "_failsafes");
    } else if (ui->failsafeViewLabel->text().contains("EDIT FAILSAFE") && ui->stackedWidget->currentIndex() == 4) {
        if (!validateData(Exporter::FAILSAFES, version + "_failsafes", select->selectedRows(1).value(0).data().toString()))
            startEditFailsafe(version + "_failsafes");
    } else if (ui->interlockViewLabel->text() == "ADD INTERLOCK" && ui->stackedWidget->currentIndex() == 5) {
        if (!validateData(Exporter::INTERLOCKS, version + "_interlocks", ""))
            startAddInterlock(version + "_interlocks");
    } else if (ui->interlockViewLabel->text().contains("EDIT INTERLOCK") && ui->stackedWidget->currentIndex() == 5) {
        if (!validateData(Exporter::INTERLOCKS, version + "_interlocks", select->selectedRows(1).value(0).data().toString()))
            startEditInterlock(version + "_interlocks");
    } else if (ui->systemNoteViewLabel->text() == "ADD SYSTEM NOTE" && ui->stackedWidget->currentIndex() == 6) {
        startAddSystemNote(version + "_system_notes");
    } else {
        startEditSystemNote(version + "_system_notes");
    }
}

//Used to enforce code formatting for WCAs, failsafes, and interlocks
//Fault Code must follow Alpha### where alpha must refer to a valid subsystem
//Failsafe Codes must have FS- as a prefix
//Interlock Codes must have INT- as a prefix
bool MainWindow::validateData(Exporter::datatype type, QString version, QString originalID) {


    if (type == Exporter::WCAS) {
        //check for empty ID
        if (ui->faultCodeLE->text() == "") {
            QMessageBox::critical(this, "Error", "Fault Code cannot be blank", QMessageBox::Ok);
            return true; // error
        }

        //prevent duplicates
        if (myModel->checkDuplicateID(version, ui->faultCodeLE->text(), "fault_code", originalID)) {
            QMessageBox::critical(this, "Error", "This Fault Code is already in use", QMessageBox::Ok);
            return true; // error
        }

        //enforce formatting using a regex
        std::string regex_format ("[B|D-H|J-K|M-N|P-V|X-Z][0-9][0-9][0-9]");

        if (originalID != ui->faultCodeLE->text() && std::regex_match(ui->faultCodeLE->text().toStdString(), std::regex(regex_format)) == false) {
            QMessageBox::critical(this, "Error", "Invalid Fault Code format", QMessageBox::Ok);
            return true; // error
        }

        return false;
    } else if (type == Exporter::FAILSAFES) {
        //check for empty ID
        if (ui->failsafeIDLE->text() == "") {
            QMessageBox::critical(this, "Error", "Failsafe Code cannot be blank", QMessageBox::Ok);
            return true; // error
        }

        //prevent duplicates
        if (myModel->checkDuplicateID(version, ui->failsafeIDLE->text(), "failsafe_id", originalID)) {
            QMessageBox::critical(this, "Error", "This Failsafe Code is already in use", QMessageBox::Ok);
            return true; // error
        }

        //enforce formatting using a regex
        std::string regex_format ("FS-[a-z|A-Z|0-9]+");

        if (originalID != ui->failsafeIDLE->text() && std::regex_match(ui->failsafeIDLE->text().toStdString(), std::regex(regex_format)) == false) {
            QMessageBox::critical(this, "Error", "Invalid Failsafe Code format", QMessageBox::Ok);
            return true; // error
        }

        return false;
    } else {
        //check for empty ID
        if (ui->interlockIDLE->text() == "") {
            QMessageBox::critical(this, "Error", "Interlock Code cannot be blank", QMessageBox::Ok);
            return true; // error
        }

        //prevent duplicates
        if (myModel->checkDuplicateID(version, ui->interlockIDLE->text(), "interlock_id", originalID)) {
            QMessageBox::critical(this, "Error", "This Interlock Code is already in use", QMessageBox::Ok);
            return true; // error
        }

        //enforce formatting using a regex
        std::string regex_format ("INT-[a-z|A-Z|0-9]+");

        if (originalID != ui->interlockIDLE->text() && std::regex_match(ui->interlockIDLE->text().toStdString(), std::regex(regex_format)) == false) {
            QMessageBox::critical(this, "Error", "Invalid Interlock Code format", QMessageBox::Ok);
            return true; // error
        }

        return false;
    }
}

/******* Add/Edit Changelog Prompt functions *******/
void MainWindow::startAddFault(QString version) {
    QString text;
    QString idquery = "select * from " + version + " order by id desc limit 1";
    int numrows = myModel->queryLastID(idquery, database) + 1;

    //to get text from changelog popup
    QPlainTextEdit *reason = saveAdd->getReasonText();
    int accepted = saveAdd->exec();

    if (accepted == 1) {
        saveNewFault(); //sends query to add new item

        //form qstring with reason and date and send this to changelog
        QString dateReason = "[" + QDateTime::currentDateTime().toString("MM.dd.yyyy hh:mm") + "] " +  myModel->getUser() + " : ";

        text = reason->toPlainText();

        if (text == "")
            dateReason += "Created fault";
        else
            dateReason += text;

        QString query = QStringLiteral("update %1 set changelog = '%2' where id = %3").arg(version).arg(dateReason).arg(numrows);
        myModel->execQuery(query, database);

        setHomePage();
    }

    reason->setPlainText("");
}

void MainWindow::startAddFailsafe(QString version) {
    QString text;

    //to get text from changelog popup
    QPlainTextEdit *reason = saveAdd->getReasonText();
    int accepted = saveAdd->exec();

    if (accepted == 1) {
        //receives failsafe id of new failsafe
        QString failsafeid = saveNewFailsafe();  //sends query to add new item

        //form qstring with reason and date and send this to changelog
        QString dateReason = "[" + QDateTime::currentDateTime().toString("MM.dd.yyyy hh:mm") + "] " +  myModel->getUser() + " : ";

        text = reason->toPlainText();

        if (text == "")
            dateReason += "Created failsafe";
        else
            dateReason += text;

        QString query = QStringLiteral("update %1 set changelog = '%2' where failsafe_id = '%3'").arg(version).arg(dateReason).arg(failsafeid);
        myModel->execQuery(query, database);

        setHomePage();
    }

    reason->setPlainText("");
}

void MainWindow::startAddInterlock(QString version) {
    QString text;

    //to get text from changelog popup
    QPlainTextEdit *reason = saveAdd->getReasonText();
    int accepted = saveAdd->exec();

    if (accepted == 1) {
        //receives failsafe id of new interlock
        QString interlockid = saveNewInterlock();  //sends query to add new item

        //form qstring with reason and date and send this to changelog
        QString dateReason = "[" + QDateTime::currentDateTime().toString("MM.dd.yyyy hh:mm") + "] " +  myModel->getUser() + " : ";

        text = reason->toPlainText();

        if (text == "")
            dateReason += "Created interlock";
        else
            dateReason += text;

        QString query = QStringLiteral("update %1 set changelog = '%2' where interlock_id = '%3'").arg(version).arg(dateReason).arg(interlockid);
        myModel->execQuery(query, database);

        setHomePage();
    }

    reason->setPlainText("");
}

void MainWindow::startAddSystemNote(QString version) {
    QString text;
    QString idquery = "select * from " + version + " order by id desc limit 1";
    int numrows = myModel->queryLastID(idquery, database) + 1;

    //to get text from changelog popup
    QPlainTextEdit *reason = saveAdd->getReasonText();
    int accepted = saveAdd->exec();

    if (accepted == 1) {
        saveNewSystemNote();  //sends query to add new item

        //form qstring with reason and date and send this to changelog
        QString dateReason = "[" + QDateTime::currentDateTime().toString("MM.dd.yyyy hh:mm") + "] " +  myModel->getUser() + " : ";

        text = reason->toPlainText();

        if (text == "")
            dateReason += "Created system note";
        else
            dateReason += text;

        QString query = QStringLiteral("update %1 set changelog = '%2' where id = %3").arg(version).arg(dateReason).arg(numrows);
        myModel->execQuery(query, database);

        setHomePage();
    }

    reason->setPlainText("");
}

void MainWindow::startEditFault(QString version) {
    QString text;
    QItemSelectionModel *select = ui->tableView->selectionModel();
    int row = select->selectedRows(0).value(0).data().toInt();

    QPlainTextEdit *reason = saveEdit->getReasonText();
    QTextBrowser *changes = saveEdit->getChangeBrowser();

    setChangesBrowser(changes);


    //value is 1 if ok clicked, 0 if cancel clicked
    int accepted = saveEdit->exec();

    if (accepted == 1) {
        saveEditFault();

        QString dateReason = "[" + QDateTime::currentDateTime().toString("MM.dd.yyyy hh:mm") + "] " +  myModel->getUser() + " : ";

        text = reason->toPlainText();

        if (text == "")
            dateReason += "Edited fault";
        else
            dateReason += text;

        QString query = QStringLiteral("update %1 set changelog = concat(ifnull(concat(changelog, '\n'), ''), '%2') where id = %3").arg(version).arg(dateReason).arg(row);
        myModel->execQuery(query, database);

        setHomePage();
    }

    reason->setPlainText("");
    changes->setText("");
}

void MainWindow::startEditFailsafe(QString version) {
    QString text;

    QPlainTextEdit *reason = saveEdit->getReasonText();
    QTextBrowser *changes = saveEdit->getChangeBrowser();

    setChangesBrowser(changes);


    //value is 1 if ok clicked, 0 if cancel clicked
    int accepted = saveEdit->exec();

    if (accepted == 1) {
        QString fail_id = saveEditFailsafe();

        QString dateReason = "[" + QDateTime::currentDateTime().toString("MM.dd.yyyy hh:mm") + "] " +  myModel->getUser() + " : ";

        text = reason->toPlainText();

        if (text == "")
            dateReason += "Edited failsafe";
        else
            dateReason += text;

        QString query = QStringLiteral("update %1 set changelog = concat(ifnull(concat(changelog, '\n'), ''), '%2') where failsafe_id = '%3'").arg(version).arg(dateReason).arg(fail_id);
        myModel->execQuery(query, database);

        setHomePage();
    }

    reason->setPlainText("");
    changes->setText("");
}

void MainWindow::startEditInterlock(QString version) {
    QString text;

    QPlainTextEdit *reason = saveEdit->getReasonText();
    QTextBrowser *changes = saveEdit->getChangeBrowser();

    setChangesBrowser(changes);


    //value is 1 if ok clicked, 0 if cancel clicked
    int accepted = saveEdit->exec();

    if (accepted == 1) {
        QString int_id = saveEditInterlock();

        QString dateReason = "[" + QDateTime::currentDateTime().toString("MM.dd.yyyy hh:mm") + "] " +  myModel->getUser() + " : ";

        text = reason->toPlainText();

        if (text == "")
            dateReason += "Edited interlock";
        else
            dateReason += text;

        QString query = QStringLiteral("update %1 set changelog = concat(ifnull(concat(changelog, '\n'), ''), '%2') where interlock_id = '%3'").arg(version).arg(dateReason).arg(int_id);
        myModel->execQuery(query, database);

        setHomePage();
    }

    reason->setPlainText("");
    changes->setText("");
}

void MainWindow::startEditSystemNote(QString version) {
    QString text;
    QItemSelectionModel *select = ui->tableView->selectionModel();
    int note_id = select->selectedRows(0).value(0).data().toInt();

    QPlainTextEdit *reason = saveEdit->getReasonText();
    QTextBrowser *changes = saveEdit->getChangeBrowser();

    setChangesBrowser(changes);

    //value is 1 if ok clicked, 0 if cancel clicked
    int accepted = saveEdit->exec();

    if (accepted == 1) {
        saveEditSystemNote();

        QString dateReason = "[" + QDateTime::currentDateTime().toString("MM.dd.yyyy hh:mm") + "] " +  myModel->getUser() + " : ";

        text = reason->toPlainText();

        if (text == "")
            dateReason += "Edited system note";
        else
            dateReason += text;

        QString query = QStringLiteral("update %1 set changelog = concat(ifnull(concat(changelog, '\n'), ''), '%2') where id = %3").arg(version).arg(dateReason).arg(note_id);
        myModel->execQuery(query, database);

        setHomePage();
    }

    reason->setPlainText("");
    changes->setText("");
}

void MainWindow::setChangesBrowser(QTextBrowser *changes) {
    //get the correct form (tab widget)
    QWidget *temp = getTabWidget();

    //search all fields for highlights and append edits to changes browser
    //use accessible description for the name of each field
    foreach (QLineEdit* le, temp->findChildren<QLineEdit*>()) {
        if (le->backgroundRole() == QPalette::Highlight && le->accessibleDescription() != "") {
            QString change = le->accessibleDescription() + " changed to '" + le->text() + "'\n";
            changes->append(change);
        }
    }

    foreach (QPlainTextEdit* pt, temp->findChildren<QPlainTextEdit*>()) {
        if (pt->backgroundRole() == QPalette::Highlight) {
            QString change = pt->accessibleDescription() + " changed to '" + pt->toPlainText() + "'\n";
            changes->append(change);
        }
    }

    foreach (QComboBox* cb, temp->findChildren<QComboBox*>()) {
        if (cb->lineEdit()->backgroundRole() == QPalette::Highlight) {
            QString change = cb->accessibleDescription() + " changed to '" + cb->currentText() + "'\n";
            changes->append(change);
        }
    }

    //compare contents of associated wcas to database and append to changes if needed
    QString newListValues;
    foreach (QListWidget *lw, temp->findChildren<QListWidget*>()) {//theres only one list widget so this foreach is not necessary for now
        for(int i = 0; i < lw->count(); i++) {
            //construct comma separated list from current contents
            QListWidgetItem *temp = lw->item(i);
            QString faultCode = temp->text().split(']').first().split("[").last();

            QString plainVersion = ui->versionComboBox->currentText();
            MyModel::convertToUnderscores(&plainVersion);

            //append to new comma separated list of values
            if (faultCode != "" && myModel->checkFaultCode(faultCode, plainVersion, database)) {
                if (i < lw->count() - 1)
                    newListValues += faultCode + ",";
                else
                    newListValues += faultCode;
            }
        }

        if (newListValues.at(newListValues.length() - 1) == ',')
            //delete last char
            newListValues.remove(newListValues.length() - 1, 1);

        QString query;
        QString version = ui->versionComboBox->currentText();
        MyModel::convertToUnderscores(&version);
        QItemSelectionModel *select = ui->tableView->selectionModel();
        QString id = select->selectedRows(1).value(0).data().toString();

        //compare to current csv list content in database
        if (ui->categoryComboBox->currentText() == "Failsafes")
            query = QStringLiteral("Select triggering_faults from %1 where %2 = '%3'").arg(version+"_failsafes").arg("failsafe_id").arg(id); //rename triggering faults to associated_wcas in database later
        else
            query = QStringLiteral("Select associated_wcas from %1 where %2 = '%3'").arg(version+"_interlocks").arg("interlock_id").arg(id);

        QString oldListValues = myModel->getField(query, database);

        if (oldListValues != newListValues)
            changes->append("Associated Fault Code(s) changed to " + newListValues + "\n");
    }
}

/******* End Add/Edit Changelog Prompt functions ****/

/******* Save Add/Edit query functions *******/
void MainWindow::saveNewFault() {
    //get last row num from sql table

    QString version = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&version);

    QString idquery = "select * from " + version + " order by id desc limit 1";
    int numrows = myModel->queryLastID(idquery, database) + 1;

    //enter all fields into query with id being the first
    QString query = QStringLiteral("insert into %1 values ('%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', '%11', '%12', '%13', '%14', '%15', "
        "'%16', '%17', '%18', '%19', '%20', '%21', '%22', '%23', '%24', '%25')")
            .arg(version).arg(numrows).arg(ui->fmStatusCB->currentText()).arg(ui->groupLE->text()).arg(ui->enumNameLE->text())
            .arg(ui->faultCodeLE->text()).arg(ui->changeeLE->text()).arg(ui->faultGroupLE->text()).arg(ui->configurationFaultCB->currentText())
            .arg(ui->wcaSeverityCB->currentText()).arg(ui->wcaMessagePT->toPlainText()).arg(ui->wcaTriggerPT->toPlainText()).arg(ui->wcaClearingPT->toPlainText()).arg(ui->failureImpactPT->toPlainText())
            .arg(ui->failsafeCodeLE->text()).arg(ui->interlockCodeLE->text()).arg(ui->failsafeSummaryPT->toPlainText()).arg(ui->onboardLCSPT->toPlainText())
            .arg(ui->offboardLCSPT->toPlainText()).arg(ui->lcsLRPT->toPlainText()).arg(ui->correctiveMaintenanceActionsPT->toPlainText()).arg(ui->wcaNotesPT->toPlainText())
            .arg(ui->deviceLE->text()).arg(ui->notesPT->toPlainText()).arg(ui->changeLogBrowser->toPlainText());

    myModel->execQuery(query, database);
    myModel->updateFaultConstantsTable(ui->groupLE->text(), ui->enumNameLE->text(), database);
}

QString MainWindow::saveNewFailsafe() {
    //get last row num from sql table

    QString version = ui->versionComboBox->currentText() + "_failsafes";
    MyModel::convertToUnderscores(&version);
    QString newListValues;

    //save list widget contents
    foreach (QListWidget *lw, ui->tabWidget_3->findChildren<QListWidget*>()) {//theres only one list widget so this foreach is not necessary for now
        for(int i = 0; i < lw->count(); i++) {
            QListWidgetItem *temp = lw->item(i);
            QString faultCode = temp->text().split(']').first().split("[").last();

            QString plainVersion = ui->versionComboBox->currentText();
            MyModel::convertToUnderscores(&plainVersion);

            //append to new comma separated list of values
            if (faultCode != "" && myModel->checkFaultCode(faultCode, plainVersion, database)) {
                if (i < lw->count() - 1)
                    newListValues += faultCode + ",";
                else
                    newListValues += faultCode;
            }
        }
    }

    if (newListValues != "" && newListValues.at(newListValues.length() - 1) == ',')
        //delete last char
        newListValues.remove(newListValues.length() - 1, 1);

    //enter all fields into the query
    QString query = QStringLiteral("insert into %1 values ('%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', '%11', '%12', '%13', '%14',"
                                   "'%15', '%16', '%17', '%18', '%19', '%20', '%21', '%22')")
            .arg(version).arg(ui->fmStatus2CB->currentText()).arg(ui->failsafeIDLE->text()).arg(ui->configurationFailCB->currentText())
            .arg(ui->failureNamePT->toPlainText())
            .arg(ui->operationalStatePT->toPlainText()).arg(ui->triggersPT->toPlainText()).arg(newListValues).arg(ui->failsafeClearingPT->toPlainText())
            .arg(ui->indicationMethodPT->toPlainText()).arg(ui->failsafeNotesPT->toPlainText()).arg(ui->functionalImpactPT->toPlainText())
            .arg(ui->racLE->text()).arg(ui->firstResponsePT->toPlainText())
            .arg(ui->secondResponsePT->toPlainText()).arg(ui->thirdResponsePT->toPlainText()).arg(ui->failsafeSummaryFailPT->toPlainText())
            .arg(ui->applicableSafetyPreceptLE->text()).arg(ui->mitigatedRACLE->text()).arg(ui->internalNotesFailPT->toPlainText())
            .arg(ui->changeeFailLE->text()).arg(ui->changeLogFailBrowser->toPlainText());

    myModel->execQuery(query, database);

    return ui->failsafeIDLE->text();
}

QString MainWindow::saveNewInterlock() {
    QString version = ui->versionComboBox->currentText() + "_interlocks";
    MyModel::convertToUnderscores(&version);
    QString newListValues;

    //save list widget contents
    foreach (QListWidget *lw, ui->tabWidget_4->findChildren<QListWidget*>()) {//theres only one list widget so this foreach is not necessary for now
        for(int i = 0; i < lw->count(); i++) {
            QListWidgetItem *temp = lw->item(i);
            QString faultCode = temp->text().split(']').first().split("[").last();

            QString plainVersion = ui->versionComboBox->currentText();
            MyModel::convertToUnderscores(&plainVersion);

            //append to new comma separated list of values
            if (faultCode != "" && myModel->checkFaultCode(faultCode, plainVersion, database)) {
                if (i < lw->count() - 1)
                    newListValues += faultCode + ",";
                else
                    newListValues += faultCode;
            }
        }
    }

    if (newListValues != "" && newListValues.at(newListValues.length() - 1) == ',')
        //delete last char
        newListValues.remove(newListValues.length() - 1, 1);

    //enter all fields into the query
    QString query = QStringLiteral("insert into %1 values ('%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10')")
            .arg(version).arg(ui->fmStatus3CB->currentText()).arg(ui->interlockIDLE->text()).arg(ui->configurationIntCB->currentText())
            .arg(ui->componentsCB->currentText())
            .arg(ui->interlockDescriptionPT->toPlainText()).arg(newListValues).arg(ui->internalNotesIntPT->toPlainText())
            .arg(ui->changeeIntLE->text()).arg(ui->changeLogIntBrowser->toPlainText());

    myModel->execQuery(query, database);

    return ui->interlockIDLE->text();
}

void MainWindow::saveNewSystemNote() {
    QString version = ui->versionComboBox->currentText() + "_system_notes";
    MyModel::convertToUnderscores(&version);
    QString idquery = "select * from " + version + " order by id desc limit 1";
    int numrows = myModel->queryLastID(idquery, database) + 1;

    //enter all fields into the query
    QString query = QStringLiteral("insert into %1 values ('%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', '%11', '%12', '%13', '%14')")
            .arg(version).arg(numrows).arg(ui->fmStatus4CB->currentText()).arg(ui->configurationSysCB->currentText())
            .arg(ui->operatorCB->currentText()).arg(ui->summarySysLE->text()).arg(ui->descriptionSysPT->toPlainText())
            .arg(ui->workaroundsPT->toPlainText()).arg(ui->externalNotesSysPT->toPlainText()).arg(ui->internalNotesSysPT->toPlainText())
            .arg(ui->faultGroupSysCB->currentText()).arg(ui->pcrNumLE->text()).arg(ui->changeeSysLE->text()).arg(ui->changeLogSysBrowser->toPlainText());

    myModel->execQuery(query, database);
}

void MainWindow::saveEditFault() {
    //get version and row number
    QString version = ui->versionComboBox->currentText();
    std::replace(version.begin(), version.end(), '.', '_');

    QItemSelectionModel *select = ui->tableView->selectionModel();
    int row = select->selectedRows(0).value(0).data().toInt();

    //Figure out which fields were edited and save them by using the highlight background role for comparison.
    //The accessible name is the name of the field in the database so use it to store new value in database.
    foreach (QLineEdit* le, ui->tabWidget_2->findChildren<QLineEdit*>()) {
        if (le->backgroundRole() == QPalette::Highlight) {
            QString query = QStringLiteral("update %1 set %2 = '%3' where id = %4").arg(version).arg(le->accessibleName()).arg(le->text()).arg(row);
            myModel->execQuery(query, database);

            // If editing a field stored in the faultconstants table, the entry needs to updated
            // in both places. This will only add, not delete, to the fault constants table
            myModel->updateFaultConstantsTable(ui->groupLE->text(), ui->enumNameLE->text(), database);
        }
    }

    foreach (QPlainTextEdit* pt, ui->tabWidget_2->findChildren<QPlainTextEdit*>()) {
        if (pt->backgroundRole() == QPalette::Highlight) {
            QString query = QStringLiteral("update %1 set %2 = '%3' where id = %4").arg(version).arg(pt->accessibleName()).arg(pt->toPlainText()).arg(row);
            myModel->execQuery(query, database);
        }
    }

    foreach (QComboBox* cb, ui->tabWidget_2->findChildren<QComboBox*>()) {
        if (cb->lineEdit()->backgroundRole() == QPalette::Highlight) {
            QString query = QStringLiteral("update %1 set %2 = '%3' where id = %4").arg(version).arg(cb->accessibleName()).arg(cb->currentText()).arg(row);
            myModel->execQuery(query, database);
        }
    }
}

QString MainWindow::saveEditFailsafe() {
    //get version and row number
    QString version = ui->versionComboBox->currentText();
    std::replace(version.begin(), version.end(), '.', '_');
    version += "_failsafes";

    QItemSelectionModel *select = ui->tableView->selectionModel();
    QString fail_id = select->selectedRows(1).value(0).data().toString();

    //Figure out which fields were edited and save them by using the highlight background role for comparison.
    //The accessible name is the name of the field in the database so use it to store new value in database.
    foreach (QLineEdit* le, ui->tabWidget_3->findChildren<QLineEdit*>()) {
        if (le->backgroundRole() == QPalette::Highlight) {
            QString query = QStringLiteral("update %1 set %2 = '%3' where failsafe_id = '%4'").arg(version).arg(le->accessibleName()).arg(le->text()).arg(fail_id);
            myModel->execQuery(query, database);

            if (le->accessibleName() == "failsafe_id") {
                fail_id = le->text(); //return fail_id for changelog
            }
        }
    }

    foreach (QPlainTextEdit* pt, ui->tabWidget_3->findChildren<QPlainTextEdit*>()) {
        if (pt->backgroundRole() == QPalette::Highlight) {
            QString query = QStringLiteral("update %1 set %2 = '%3' where failsafe_id = '%4'").arg(version).arg(pt->accessibleName()).arg(pt->toPlainText()).arg(fail_id);
            myModel->execQuery(query, database);
        }
    }

    foreach (QComboBox* cb, ui->tabWidget_3->findChildren<QComboBox*>()) {
        if (cb->lineEdit()->backgroundRole() == QPalette::Highlight) {
            QString query = QStringLiteral("update %1 set %2 = '%3' where failsafe_id = '%4'").arg(version).arg(cb->accessibleName()).arg(cb->currentText()).arg(fail_id);
            myModel->execQuery(query, database);
        }
    }

    //save list widget contents
    foreach (QListWidget *lw, ui->tabWidget_3->findChildren<QListWidget*>()) {
        QString newListValues;

        for(int i = 0; i < lw->count(); i++) {
            QListWidgetItem *temp = lw->item(i);
            QString faultCode = temp->text().split(']').first().split("[").last();

            QString plainVersion = ui->versionComboBox->currentText();
            std::replace(plainVersion.begin(), plainVersion.end(), '.', '_');

            //append to new comma separated list of values
            if (faultCode != "" && myModel->checkFaultCode(faultCode, plainVersion, database)) {
                if (i < lw->count() - 1)
                    newListValues += faultCode + ",";
                else
                    newListValues += faultCode;
            }
        }

        if (newListValues.at(newListValues.length() - 1) == ',')
            //delete last char
            newListValues.remove(newListValues.length() - 1, 1);

        //save new values to database
        QString query = QStringLiteral("update %1 set %2 = '%3' where failsafe_id = '%4'").arg(version).arg(lw->accessibleName()).arg(newListValues).arg(fail_id);
        myModel->execQuery(query, database);
    }

    return fail_id;
}

QString MainWindow::saveEditInterlock() {
    //get version and row number
    QString version = ui->versionComboBox->currentText();
    std::replace(version.begin(), version.end(), '.', '_');
    version += "_interlocks";

    QItemSelectionModel *select = ui->tableView->selectionModel();
    QString int_id = select->selectedRows(1).value(0).data().toString();

    //Figure out which fields were edited and save them by using the highlight background role for comparison.
    //The accessible name is the name of the field in the database so use it to store new value in database.
    foreach (QLineEdit* le, ui->tabWidget_4->findChildren<QLineEdit*>()) {
        if (le->backgroundRole() == QPalette::Highlight) {
            QString query = QStringLiteral("update %1 set %2 = '%3' where interlock_id = '%4'").arg(version).arg(le->accessibleName()).arg(le->text()).arg(int_id);
            myModel->execQuery(query, database);

            if (le->accessibleName() == "interlock_id") {
                int_id = le->text(); //return int_id for changelog
            }
        }
    }

    foreach (QPlainTextEdit* pt, ui->tabWidget_4->findChildren<QPlainTextEdit*>()) {
        if (pt->backgroundRole() == QPalette::Highlight) {
            QString query = QStringLiteral("update %1 set %2 = '%3' where interlock_id = '%4'").arg(version).arg(pt->accessibleName()).arg(pt->toPlainText()).arg(int_id);
            myModel->execQuery(query, database);
        }
    }

    foreach (QComboBox* cb, ui->tabWidget_4->findChildren<QComboBox*>()) {
        if (cb->lineEdit()->backgroundRole() == QPalette::Highlight) {
            QString query = QStringLiteral("update %1 set %2 = '%3' where interlock_id = '%4'").arg(version).arg(cb->accessibleName()).arg(cb->currentText()).arg(int_id);
            myModel->execQuery(query, database);
        }
    }

    //save list widget contents
    foreach (QListWidget *lw, ui->tabWidget_4->findChildren<QListWidget*>()) {
        QString newListValues;

        for(int i = 0; i < lw->count(); i++) {
            QListWidgetItem *temp = lw->item(i);
            QString faultCode = temp->text().split(']').first().split("[").last();

            QString plainVersion = ui->versionComboBox->currentText();
            std::replace(plainVersion.begin(), plainVersion.end(), '.', '_');

            //append to new comma separated list of values
            if (faultCode != "" && myModel->checkFaultCode(faultCode, plainVersion, database)) {
                if (i < lw->count() - 1)
                    newListValues += faultCode + ",";
                else
                    newListValues += faultCode;
            }
        }

        if (newListValues.at(newListValues.length() - 1) == ',')
            //delete last char
            newListValues.remove(newListValues.length() - 1, 1);

        //save new values to database
        QString query = QStringLiteral("update %1 set %2 = '%3' where interlock_id = '%4'").arg(version).arg(lw->accessibleName()).arg(newListValues).arg(int_id);
        myModel->execQuery(query, database);
    }

    return int_id;
}

void MainWindow::saveEditSystemNote() {
    //get version and row number
    QString version = ui->versionComboBox->currentText();
    std::replace(version.begin(), version.end(), '.', '_');
    version += "_system_notes";

    QItemSelectionModel *select = ui->tableView->selectionModel();
    int note_id = select->selectedRows(0).value(0).data().toInt();

    //Figure out which fields were edited and save them by using the highlight background role for comparison.
    //The accessible name is the name of the field in the database so use it to store new value in database.
    foreach (QLineEdit* le, ui->tabWidget_5->findChildren<QLineEdit*>()) {
        if (le->backgroundRole() == QPalette::Highlight) {
            QString query = QStringLiteral("update %1 set %2 = '%3' where id = %4").arg(version).arg(le->accessibleName()).arg(le->text()).arg(note_id);
            myModel->execQuery(query, database);
        }
    }

    foreach (QPlainTextEdit* pt, ui->tabWidget_5->findChildren<QPlainTextEdit*>()) {
        if (pt->backgroundRole() == QPalette::Highlight) {
            QString query = QStringLiteral("update %1 set %2 = '%3' where id = %4").arg(version).arg(pt->accessibleName()).arg(pt->toPlainText()).arg(note_id);
            myModel->execQuery(query, database);
        }
    }

    foreach (QComboBox* cb, ui->tabWidget_5->findChildren<QComboBox*>()) {
        if (cb->lineEdit()->backgroundRole() == QPalette::Highlight) {
            QString query = QStringLiteral("update %1 set %2 = '%3' where id = %4").arg(version).arg(cb->accessibleName()).arg(cb->currentText()).arg(note_id);
            myModel->execQuery(query, database);
        }
    }
}

/******* End Save Add/Edit query functions ****/

/******* Save button Listeners *******/
void MainWindow::on_saveMCMUSVButton_clicked() {
    showInfoPrompt();
}

void MainWindow::on_saveWCAButton_clicked() {
    showInfoPrompt();
}

void MainWindow::on_saveFailButton_clicked() {
    showInfoPrompt();
}

void MainWindow::on_saveResponseButton_clicked() {
    showInfoPrompt();
}

void MainWindow::on_saveOtherButton_clicked() {
    showInfoPrompt();
}

void MainWindow::on_saveGeneralFailButton_clicked() {
    showInfoPrompt();
}

void MainWindow::on_saveResponseFailButton_clicked() {
    showInfoPrompt();
}

void MainWindow::on_saveOtherFailButton_clicked() {
    showInfoPrompt();
}

void MainWindow::on_saveIntButton_clicked() {
    showInfoPrompt();
}

void MainWindow::on_saveGeneralSysButton_clicked() {
    showInfoPrompt();
}

void MainWindow::on_saveNotesSysButton_clicked() {
    showInfoPrompt();
}

/******* End Save Button Listeners ****/

/******* Start delete popup functions ****/

//popup for deleting a fault, the function for the correct prompt is called
void MainWindow::selectDelete() {
    if (ui->categoryComboBox->currentText() == "Failsafes") {
        showDeleteFailsafePrompt();
    } else if (ui->categoryComboBox->currentText() == "Interlocks") {
        showDeleteInterlockPrompt();
    } else if (ui->categoryComboBox->currentText() == "System Notes") {
        showDeleteSystemNotePrompt();
    } else {
        showDeleteFaultPrompt();
    }
}

//popup for deleting a fault
void MainWindow::showDeleteFaultPrompt() {
    QItemSelectionModel *select = ui->tableView->selectionModel();

    //ensures that a fault to delete was selected
    if (select->selectedRows().count() != 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                          "Confirm Erase", "Are you sure you want to permanently erase from the database?",
                                 QMessageBox::Yes | QMessageBox::No);


        if(reply == QMessageBox::Yes) {
           deleteFault();
           on_versionComboBox_activated(ui->versionComboBox->currentText());
        }
    }
}

//popup for deleting a failsafe
void MainWindow::showDeleteFailsafePrompt() {
    QItemSelectionModel *select = ui->tableView->selectionModel();

    //ensures that a fault to delete was selected
    if (select->selectedRows().count() != 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                          "Confirm Deletion", "Are you sure you want to delete this failsafe?",
                                 QMessageBox::Yes | QMessageBox::No);


        if(reply == QMessageBox::Yes) {
           deleteFailsafe();
           on_versionComboBox_activated(ui->versionComboBox->currentText());
        }
    }
}

//popup for deleting an interlock
void MainWindow::showDeleteInterlockPrompt() {
    QItemSelectionModel *select = ui->tableView->selectionModel();

    //ensures that a fault to delete was selected
    if (select->selectedRows().count() != 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                          "Confirm Deletion", "Are you sure you want to delete this interlock?",
                                 QMessageBox::Yes | QMessageBox::No);


        if(reply == QMessageBox::Yes) {
           deleteInterlock();
           on_versionComboBox_activated(ui->versionComboBox->currentText());
        }
    }
}

//popup for deleting a system note
void MainWindow::showDeleteSystemNotePrompt() {
    QItemSelectionModel *select = ui->tableView->selectionModel();

    //ensures that a fault to delete was selected
    if (select->selectedRows().count() != 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                          "Confirm Deletion", "Are you sure you want to delete this system note?",
                                 QMessageBox::Yes | QMessageBox::No);


        if(reply == QMessageBox::Yes) {
           deleteSystemNote();
           on_versionComboBox_activated(ui->versionComboBox->currentText());
        }
    }
}
/******* End delete popup functions ****/

/******* Start filters functions ****/
//filters page buttons
void MainWindow::on_applyFiltersButton_clicked()
{
   ui->stackedWidget->setCurrentIndex(0);

   QCustomPushButton * b = new QCustomPushButton();
   b->setParent(this);
   b->setText("   \"" + ui->comboBox_6->currentText() + "\"  contains  \"" + ui->lineEdit_8->text().toLower() + "\"   ");
   //QSizePolicy buttonSize = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   b->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
   b->setMaximumHeight(20);
   b->setMinimumHeight(20);
   b->setMaximumWidth(250);
   b->setMinimumWidth(250);
   b->field = ui->comboBox_6->currentText();
   b->resize(b->sizeHint().width(), b->sizeHint().height());
   b->setStyleSheet("border: 1px solid gray; border-radius: 3px; background: lightgray");

   ui->horizontalLayout_12->addWidget(b);

   connect(b, SIGNAL(clicked()), b, SLOT(emitClickedData()));
   connect(b, SIGNAL(clickedData(QString)), this, SLOT(onFilterClose(QString)));

   filterDisplays.insert(ui->comboBox_6->currentText(), b);
   myModel->addFilter(ui->comboBox_6->currentText(), ui->lineEdit_8->text());
   myModel->refreshFilters(ui->searchLineEdit->text(), getCustomView(), ui->categoryComboBox->currentText());
}

void MainWindow::onFilterClose(QString field) {
    QWidget * button = filterDisplays.value(field);

    ui->horizontalLayout_12->removeWidget(button);
    delete button;

    filterDisplays.remove(field);
    myModel->removeFilter(field);
    myModel->refreshFilters(ui->searchLineEdit->text(), getCustomView(), ui->categoryComboBox->currentText());
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    QCustomPushButton * button = (QCustomPushButton *)obj;

  if (filterDisplays.values().contains(button)) {
    if (event->type() == QEvent::Enter)
    {
        tempButtonText = button->text();
        button->setText("Remove?");
        button->setStyleSheet("border: 1px solid gray; border-radius: 3px; background: #d9939d");
    }
    else if (event->type() == QEvent::Leave){
        button->setText(tempButtonText);
        button->setStyleSheet("border: 1px solid gray; border-radius: 3px; background: lightgray");
        tempButtonText = "";
    }
    return QWidget::eventFilter(obj, event);
  }
  else    return QWidget::eventFilter(obj, event);
}

void MainWindow::on_cancelFiltersButton_clicked()
{
   ui->stackedWidget->setCurrentIndex(0);
}
/******* End filters functions ****/

/******* Start New Entry Page functions ****/

//new fault listener
void MainWindow::on_newFaultButton_clicked()
{
    createNewFaultPage();
}

//set buttons and fields for the add page
void MainWindow::createNewFaultPage() {
    QWidget *temp = getTabWidget();
    makeReadOnlyFields(false);
    ghostSaveFaults(true);
    hideEditFaults(false);

    if (ui->categoryComboBox->currentText() == "Failsafes") {
        ui->failsafeViewLabel->setText("ADD FAILSAFE");
        ui->stackedWidget->setCurrentIndex(4);
        ui->tabWidget_3->setTabEnabled(3, false); //enable changelog tab
        ui->nextOtherFailsafeButton->setEnabled(false); //enable next button for changelog
    } else if (ui->categoryComboBox->currentText() == "Interlocks") {
        ui->interlockViewLabel->setText("ADD INTERLOCK");
        ui->stackedWidget->setCurrentIndex(5);
        ui->tabWidget_4->setTabEnabled(1, false); //enable changelog tab
        ui->nextGeneralIntButton->setEnabled(false); //enable next button for changelog
    } else if (ui->categoryComboBox->currentText() == "System Notes") {
        ui->systemNoteViewLabel->setText("ADD SYSTEM NOTE");
        ui->stackedWidget->setCurrentIndex(6);
        ui->tabWidget_5->setTabEnabled(2, false); //enable changelog tab
        ui->nextNotesSysButton->setEnabled(false); //enable next button for changelog
    } else {
        ui->faultViewLabel->setText("ADD FAULT");
        ui->stackedWidget->setCurrentIndex(1);
        ui->tabWidget_2->setTabEnabled(5, false); //enable changelog tab
        ui->nextOtherButton_2->setEnabled(false); //enable next button for changelog
    }


    foreach (QComboBox* cb, temp->findChildren<QComboBox*>()) {
        cb->lineEdit()->setPlaceholderText("Select...");
        cb->setCurrentIndex(-1);
    }

    updateSaveButtons();
}
/******* End New Entry Page functions ****/

/******* Start View Page functions ****/

//determine which view page to display
void MainWindow::on_editButton_clicked()
{
    QItemSelectionModel *select = ui->tableView->selectionModel();

    if (select->selectedRows().count() != 0) {
        //determine selected row and version
        QString row, query, version = ui->versionComboBox->currentText();
        MyModel::convertToUnderscores(&version);

        //determine category and row, query data, and fill fields
        if (ui->categoryComboBox->currentText() == "Failsafes") {
            row = select->selectedRows(1).value(0).data().toString();
            query = QStringLiteral("select * from %1_failsafes where failsafe_id like '%2'").arg(version).arg(row);
            std::vector<QString> data = myModel->queryViewPage(query, failsafedbcolumns, database);
            ui->tabWidget_3->setTabEnabled(3, true); //enable changelog tab
            ui->nextOtherFailsafeButton->setEnabled(true); //enable next button for changelog
            fillFailsafeFields(data);
            ui->stackedWidget->setCurrentIndex(4);
        } else if (ui->categoryComboBox->currentText() == "Interlocks") {
            row = select->selectedRows(1).value(0).data().toString();
            query = QStringLiteral("select * from %1_interlocks where interlock_id like '%2'").arg(version).arg(row);
            std::vector<QString> data = myModel->queryViewPage(query, interlockdbcolumns, database);
            ui->tabWidget_4->setTabEnabled(1, true); //enable changelog tab
            ui->nextGeneralIntButton->setEnabled(true); //enable next button for changelog
            fillInterlockFields(data);
            ui->stackedWidget->setCurrentIndex(5);
        } else if (ui->categoryComboBox->currentText() == "System Notes") {
            row = select->selectedRows(0).value(0).data().toString();
            query = QStringLiteral("select * from %1_system_notes where id like '%2'").arg(version).arg(row);
            std::vector<QString> data = myModel->queryViewPage(query, systemnotesdbcolumns, database);
            ui->tabWidget_5->setTabEnabled(2, true); //enable changelog tab
            ui->nextNotesSysButton->setEnabled(true); //enable next button for changelog
            fillSystemNotesFields(data);
            ui->stackedWidget->setCurrentIndex(6);
        } else {
            row = select->selectedRows(0).value(0).data().toString();
            query = QStringLiteral("select * from %1 where id like %2").arg(version).arg(row);
            std::vector<QString> data = myModel->queryViewPage(query, dbcolumns, database);
            ui->tabWidget_2->setTabEnabled(5, true); //enable changelog tab
            ui->nextOtherButton_2->setEnabled(true); //enable next button for changelog
            fillFaultFields(data);
            ui->stackedWidget->setCurrentIndex(1);
        }

        if (ui->releasedLabel->text() == "(Unlocked)")
            createViewPage(false);
        else
            createViewPage(true);

    } else {
        qInfo() << "No row selected";
    }
}

//fills in the fields for the fault view page
void MainWindow::fillFaultFields(std::vector<QString> data) {
    //set line edits
    ui->groupLE->setText(data.at(groupLE));
    ui->enumNameLE->setText(data.at(enumNameLE));
    ui->faultCodeLE->setText(data.at(faultCodeLE));
    ui->changeeLE->setText(data.at(changeeLE));
    ui->faultGroupLE->setText(data.at(faultGroupLE));
    ui->failsafeCodeLE->setText(data.at(failsafeCodeLE));
    ui->interlockCodeLE->setText(data.at(interlockCodeLE));
    ui->deviceLE->setText(data.at(deviceLE));

    //set plain text edits
    ui->wcaMessagePT->setPlainText(data.at(wcaMessagePT));
    ui->wcaTriggerPT->setPlainText(data.at(wcaTriggerPT));
    ui->wcaClearingPT->setPlainText(data.at(wcaClearingPT));
    ui->failureImpactPT->setPlainText(data.at(failureImpactPT));
    ui->failsafeSummaryPT->setPlainText(data.at(failsafeSummaryPT));
    ui->onboardLCSPT->setPlainText(data.at(onboardLCSPT));
    ui->offboardLCSPT->setPlainText(data.at(offboardLCSPT));
    ui->lcsLRPT->setPlainText(data.at(lcsLRPT));
    ui->correctiveMaintenanceActionsPT->setPlainText(data.at(correctiveMaintenanceActions));
    ui->wcaNotesPT->setPlainText(data.at(wcaNotes));
    ui->notesPT->setPlainText(data.at(notesPT));

    //set combo boxes
    if (data.at(fmStatusCB) == "OBE - Keep b/c ENUM in SW")
        ui->fmStatusCB->setCurrentText("OBE");
    else
        ui->fmStatusCB->setCurrentText(data.at(fmStatusCB));
    ui->wcaSeverityCB->setCurrentText(data.at(wcaSeverityCB));
    ui->configurationFaultCB->setCurrentText(data.at(configurationCB));

    ui->changeLogBrowser->setText(data.at(changeLogBrowser));
}

//fill in fields for failsafe view page
void MainWindow::fillFailsafeFields(std::vector<QString> data) {
    //set line edits
    ui->failsafeIDLE->setText(data.at(failsafeIDLE));
    ui->racLE->setText(data.at(racLE));
    ui->mitigatedRACLE->setText(data.at(mitigatedRACLE));
    ui->applicableSafetyPreceptLE->setText(data.at(applicableSafetyPreceptLE));
    ui->changeeFailLE->setText(data.at(changeeFailLE));

    //set plain text edits
    ui->failureNamePT->setPlainText(data.at(failureNamePT));
    ui->operationalStatePT->setPlainText(data.at(operationalStatePT));
    ui->triggersPT->setPlainText(data.at(triggersPT));
    ui->failsafeClearingPT->setPlainText(data.at(failsafeClearingPT));
    //ui->triggeringFaultsPT->setPlainText(data.at(triggeringFaultsPT));
    fillAssociatedWCAsList(ui->failsafeListWidget, "failsafe_id");
    ui->indicationMethodPT->setPlainText(data.at(indicationMethodPT));
    ui->functionalImpactPT->setPlainText(data.at(functionalImpactPT));
    ui->firstResponsePT->setPlainText(data.at(firstResponsePT));
    ui->secondResponsePT->setPlainText(data.at(secondResponsePT));
    ui->thirdResponsePT->setPlainText(data.at(thirdResponsePT));
    ui->failsafeNotesPT->setPlainText(data.at(failsafeNotesPT));
    ui->failsafeSummaryFailPT->setPlainText(data.at(failsafeSummaryFailPT));
    ui->internalNotesFailPT->setPlainText(data.at(internalNotesFailPT));

    //set comboboxes
    ui->fmStatus2CB->setCurrentText(data.at(fmStatus2CB));
    ui->configurationFailCB->setCurrentText(data.at(configurationFailCB));

    ui->changeLogFailBrowser->setText(data.at(changeLog2Browser));
}

//fill in fields for interlock view page
void MainWindow::fillInterlockFields(std::vector<QString> data) {
    //set line edits
    ui->interlockIDLE->setText(data.at(interlockIDLE));
    ui->changeeIntLE->setText(data.at(changeeIntLE));

    //set plain text edits
    ui->interlockDescriptionPT->setPlainText(data.at(interlockDescriptionPT));
    //ui->associatedWcasPT->setPlainText(data.at(associatedWcasPT));
    fillAssociatedWCAsList(ui->interlockListWidget, "interlock_id");
    ui->internalNotesIntPT->setPlainText(data.at(internalNotesIntPT));

    //set comboboxes
    ui->fmStatus3CB->setCurrentText(data.at(fmStatus3CB));
    ui->componentsCB->setCurrentText(data.at(componentsCB));
    ui->configurationIntCB->setCurrentText(data.at(configurationIntCB));

    ui->changeLogIntBrowser->setText(data.at(changeLog3Browser));
}

//adds entries to the Associated Fault Code lists for both failsafes and interlocks
void MainWindow::fillAssociatedWCAsList(QListWidget *list, QString codeType) {
    QString version = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&version);
    QItemSelectionModel *select = ui->tableView->selectionModel();
    QString id = select->selectedRows(1).value(0).data().toString();
    QString query;

    //get comma separated fault code list and split into QStringList
    if (codeType == "failsafe_id")
        query = QStringLiteral("Select triggering_faults from %1 where %2 = '%3'").arg(version+"_failsafes").arg(codeType).arg(id); //rename triggering faults to associated_wcas in database later
    else
        query = QStringLiteral("Select associated_wcas from %1 where %2 = '%3'").arg(version+"_interlocks").arg(codeType).arg(id);
    QString codeList = myModel->getField(query, database);
    QStringList splitList = codeList.split(u',');

    //iterate through list and check all codes for existence
    for (QString temp : splitList) {
        if (myModel->checkFaultCode(temp, version, database)) {
            //if fault code exists, add it and its enum name to wcas list as new entry
            QString enumQuery = QStringLiteral("Select wca_message from %1 where %2 = '%3'").arg(version).arg("fault_code").arg(temp);
            QString name = myModel->getField(enumQuery, database);
            if (temp != "")
                list->addItem("[" + temp + "] " + name);
        }
    }
}

void MainWindow::on_addFaultCodeFailButton_clicked() {
    addWCAToList(ui->failsafeListWidget, "failsafe_id");
}

void MainWindow::on_addFaultCodeIntButton_clicked() {
    addWCAToList(ui->interlockListWidget, "interlock_id");
}

void MainWindow::addWCAToList(QListWidget *list, QString codeType) {
    QItemSelectionModel *select = ui->tableView->selectionModel();
    QString id = select->selectedRows(1).value(0).data().toString();
    QString version = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&version);
    bool responded;
    QString afcQuery;

    //ask for fault code for new entry into the provided listwidget
    QString newCodeName = QInputDialog::getText(this, "Add Associated Fault", "Enter new fault code to add to the list", QLineEdit::Normal, QString(), &responded, Qt::MSWindowsFixedSizeDialogHint);

    if (newCodeName == "") {
        QMessageBox::critical(this, "Error", "Fault Code cannot be blank", QMessageBox::Ok);
        return;
    }

    if (responded) {
        if (myModel->checkFaultCode(newCodeName, version, database)) {
            //add new fault code to list widget being displayed
            QString enumQuery = QStringLiteral("Select wca_message from %1 where %2 = '%3'").arg(version).arg("fault_code").arg(newCodeName);
            QString name = myModel->getField(enumQuery, database);
            list->addItem("[" + newCodeName + "] " + name);

            //save change in database when save is pressed
        //        if (codeType == "failsafe_id")
        //            afcQuery = QStringLiteral("update %1_failsafes set triggering_faults = concat(ifnull(concat(triggering_faults, ','), ''), '%2') where %3 = '%4'").arg(version).arg(newCodeName).arg(codeType).arg(id);
        //        else
        //            afcQuery = QStringLiteral("update %1_interlocks set associated_wcas = concat(ifnull(concat(associated_wcas, ','), ''), '%2') where %3 = '%4'").arg(version).arg(newCodeName).arg(codeType).arg(id);
        //        myModel->execQuery(afcQuery, database);
        } else {
            list->addItem("[" + newCodeName + "] ERROR - there is no fault with this fault code");
            list->item(list->count()-1)->setTextColor(QColor(255,0,0)); //change color of last item to red to indicate error
        }
    }
}

void MainWindow::on_deleteFaultCodeFailButton_clicked() {
    deleteWCAFromList(ui->failsafeListWidget, "failsafe_id");
}

void MainWindow::on_deleteFaultCodeIntButton_clicked() {
    deleteWCAFromList(ui->interlockListWidget, "interlock_id");
}

void MainWindow::deleteWCAFromList(QListWidget *list, QString codeType) {
    //remove current selection from list widget
    delete list->takeItem(list->row(list->currentItem()));
}

//fill in fields for system notes view page
void MainWindow::fillSystemNotesFields(std::vector<QString> data) {
    //set line edits
    ui->pcrNumLE->setText(data.at(pcrNumLE));
    ui->changeeSysLE->setText(data.at(changeeSysLE));
    ui->summarySysLE->setText(data.at(summarySysLE));

    //set plain text edits
    ui->descriptionSysPT->setPlainText(data.at(descriptionSysPT));
    ui->workaroundsPT->setPlainText(data.at(workaroundsPT));
    ui->internalNotesSysPT->setPlainText(data.at(internalNotesSysPT));
    ui->externalNotesSysPT->setPlainText(data.at(externalNotesSysPT));

    //set comboboxes
    ui->fmStatus4CB->setCurrentText(data.at(fmStatus4CB));
    ui->configurationSysCB->setCurrentText(data.at(configurationSysCB));
    ui->operatorCB->setCurrentText(data.at(operatorCB));
    ui->faultGroupSysCB->setCurrentText(data.at(faultGroupSysCB));

    ui->changeLogSysBrowser->setText(data.at(changeLogSysBrowser));
}

/******* End View Page functions ****/

/******* Start Delete entry functions ****/

// Delete the selected fault from the table
void MainWindow::deleteFault() {
    QItemSelectionModel *select = ui->tableView->selectionModel();

    if (select->selectedRows().count() != 0) {
        //determine selected row and version
        int row = select->selectedRows(0).value(0).data().toInt();
        QString version = ui->versionComboBox->currentText();
        MyModel::convertToUnderscores(&version);

        //delete query
        QString query = QStringLiteral("delete from %1 where id=%2").arg(version).arg(row);
        myModel->queryDeleteFault(query, version, database);
    } else {
        qInfo() << "No row selected";
    }
}

void MainWindow::deleteFailsafe() {
    QItemSelectionModel *select = ui->tableView->selectionModel();

    if (select->selectedRows().count() != 0) {
        //determine selected row and version
        QString version = ui->versionComboBox->currentText();
        MyModel::convertToUnderscores(&version);
        version += "_failsafes";
        QString fail_id = select->selectedRows(1).value(0).data().toString();

        //delete query
        QString query = QStringLiteral("delete from %1 where failsafe_id = '%2'").arg(version).arg(fail_id);
        myModel->queryDeleteFault(query, version, database);
    } else {
        qInfo() << "No row selected";
    }
}

void MainWindow::deleteInterlock() {
    QItemSelectionModel *select = ui->tableView->selectionModel();

    if (select->selectedRows().count() != 0) {
        //determine selected row and version
        QString version = ui->versionComboBox->currentText();
        MyModel::convertToUnderscores(&version);
        version += "_interlocks";
        QString int_id = select->selectedRows(1).value(0).data().toString();

        //delete query
        QString query = QStringLiteral("delete from %1 where interlock_id = '%2'").arg(version).arg(int_id);
        myModel->queryDeleteFault(query, version, database);
    } else {
        qInfo() << "No row selected";
    }
}

void MainWindow::deleteSystemNote() {
    QItemSelectionModel *select = ui->tableView->selectionModel();

    if (select->selectedRows().count() != 0) {
        //determine selected row and version
        int row = select->selectedRows(0).value(0).data().toInt();
        QString version = ui->versionComboBox->currentText();
        MyModel::convertToUnderscores(&version);
        version += "_system_notes";

        //delete query
        QString query = QStringLiteral("delete from %1 where id=%2").arg(version).arg(row);
        myModel->queryDeleteFault(query, version, database);
    } else {
        qInfo() << "No row selected";
    }
}
/******* End Delete entry functions ****/

//handles when cancel is pressed in either add or edit fault modes
void MainWindow::cancelEdit() {
    QString title;
    QTabWidget *temp = getTabWidget();
    QLabel *label;

    //determine which category of data is being used
    if (ui->categoryComboBox->currentText() == "Failsafes") {
        label = ui->failsafeViewLabel;
        title = "FAILSAFE";
    } else if (ui->categoryComboBox->currentText() == "Interlocks") {
        label = ui->interlockViewLabel;
        title = "INTERLOCK";
    } else if (ui->categoryComboBox->currentText() == "System Notes") {
        label = ui->systemNoteViewLabel;
        title = "SYSTEM NOTE";
    } else {
        label = ui->faultViewLabel;
        title = "FAULT";
    }

    //if user entered any information then prompt for confirmation before cancel, else go straight to home page
    if (!(label->text().contains("VIEW " + title))) {
        bool clear = true, highlight = false;

        //for cancelling on add page
        if (label->text() == "ADD " + title) {
            foreach (QLineEdit* le, temp->findChildren<QLineEdit*>())
                if (le->accessibleDescription() != "" && le->text() != "") clear = false;

            foreach (QPlainTextEdit* pt, temp->findChildren<QPlainTextEdit*>())
                if (pt->toPlainText() != "") clear = false;

            foreach (QComboBox* cb, temp->findChildren<QComboBox*>())
                if (cb->currentIndex() != -1) clear = false;

            if (clear == true) {
                ui->stackedWidget->setCurrentIndex(0);
                temp->setCurrentIndex(0);
                clearCombos();
                return;
            } else {
                QMessageBox::StandardButton reply = QMessageBox::question(this,
                          "Confirm Cancellation", "Are you sure you want to discard this new entry?",
                          QMessageBox::Yes | QMessageBox::No);


                if(reply == QMessageBox::Yes) {
                    //go to main page + reset form and fields
                    ui->stackedWidget->setCurrentIndex(0);
                    temp->setCurrentIndex(0);
                    clearFields();
                }
            }
        }

        //for cancelling on edit page
        if (label->text().contains("EDIT " + title)) {
            //check for highlighted fields
            foreach (QLineEdit* le, temp->findChildren<QLineEdit*>()) {
                if (le->backgroundRole() == QPalette::Highlight) highlight = true;
            }

            foreach (QPlainTextEdit* pt, temp->findChildren<QPlainTextEdit*>()) {
                if (pt->backgroundRole() == QPalette::Highlight) highlight = true;
            }

            foreach (QComboBox* cb, temp->findChildren<QComboBox*>()) {
                if (cb->lineEdit()->backgroundRole() == QPalette::Highlight) highlight = true;
            }

            if (highlight == true) {
                QMessageBox::StandardButton reply = QMessageBox::question(this,
                          "Confirm Cancellation", "Are you sure you want to discard any potential edits?",
                          QMessageBox::Yes | QMessageBox::No);

                if(reply == QMessageBox::Yes) {
                    //go to main page + reset form and fields
                    ui->stackedWidget->setCurrentIndex(0);
                    temp->setCurrentIndex(0);
                    clearFields();
                }
            } else {
                ui->stackedWidget->setCurrentIndex(0);
                temp->setCurrentIndex(0);
                clearFields();
            }
        }
    } else {
        //for cancelling on view page
        ui->stackedWidget->setCurrentIndex(0);
        temp->setCurrentIndex(0);
        clearFields();
    }
}

void MainWindow::clearCombos() {
    QTabWidget *temp = getTabWidget();

    foreach (QComboBox* cb, temp->findChildren<QComboBox*>()) {
        cb->lineEdit()->setPlaceholderText("");
    }
}

// Clear text and highlights from all fields
void MainWindow::clearFields() {
    QTabWidget *temp = getTabWidget();

    //all fields can be traversed with these for loops
    foreach (QLineEdit* le, temp->findChildren<QLineEdit*>()) {
        le->clear();
        QPalette palette;
        QColor color = QColor("white");
        palette.setColor(QPalette::Base,color);
        le->setPalette(palette);
        le->setBackgroundRole(QPalette::Base);
    }

    foreach (QPlainTextEdit* pt, temp->findChildren<QPlainTextEdit*>()) {
        pt->clear();
        QPalette palette;
        QColor color = QColor("white");
        palette.setColor(QPalette::Base,color);
        pt->setPalette(palette);
        pt->setBackgroundRole(QPalette::Base);
    }

    foreach (QComboBox* cb, temp->findChildren<QComboBox*>()) {
        cb->setCurrentIndex(0);
        QPalette palette;
        cb->setEditable(true);
        cb->lineEdit()->setReadOnly(true);

        QColor color = QColor("white");
        palette.setColor(QPalette::Base,color);
        cb->lineEdit()->setPalette(palette);
        cb->setBackgroundRole(QPalette::Base);
    }

    foreach (QTextBrowser* tb, temp->findChildren<QTextBrowser*>()) {
        tb->clear();
    }

    foreach (QListWidget* lw, temp->findChildren<QListWidget*>()) {
        lw->clear();
    }
}

// Create a new release and then display it on the main page
void MainWindow::on_createReleaseButton_clicked()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    QString oldRelease = ui->comboBox_15->itemText(ui->comboBox_15->currentIndex());
    QString newRelease = ui->lineEdit_3->text();

    if (newRelease == "") {
        QMessageBox::critical(this, "Error", "New release name is required", QMessageBox::Ok);
        return;
    }

    if (myModel->checkDuplicateRelease(newRelease, database)) {
        QMessageBox::critical(this, "Error", "This release name is in use", QMessageBox::Ok);
        return;
    }

    myModel->createNewRelease(newRelease, oldRelease, false);
    ui->stackedWidget->setCurrentIndex(0);

    for (int i = 0; i < ui->versionComboBox->count(); i++) {
        QString itemText = ui->versionComboBox->itemText(i);
        MyModel::convertToUnderscores(&itemText);
        if (itemText == newRelease) {
            ui->versionComboBox->setCurrentIndex(i);
        }
    }

    on_versionComboBox_activated(newRelease);

//    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(Qt::ArrowCursor); //removes infinite loading cursor
    QApplication::processEvents();
}

void MainWindow::on_cancelReleaseButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_addFilterButton_clicked() {
    if (filterDisplays.size() >= 4) {
        QMessageBox msg;
        msg.setWindowTitle("Too Many Filters");
        msg.setText("Cannot add another filter. The max of 4 filters has already been reached");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    }
    else {
        ui->comboBox_6->clear();
        ui->lineEdit_8->clear();

        for (QString fieldName : myModel->columnMap.value("header_name").value(ui->categoryComboBox->currentText())) {
            if (!filterDisplays.keys().contains(fieldName)) {
                ui->comboBox_6->addItem(fieldName);
            }
        }

        ui->stackedWidget->setCurrentIndex(3);
    }
}

//queries data and fills up table with fault data
void MainWindow::on_versionComboBox_activated(const QString &arg1)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    QString s = ui->versionComboBox->currentText();
    if (arg1.size() > 0) {
        s = arg1;
    }
    MyModel::convertToUnderscores(&s);

    QString select = "select * from " + s;
    if (ui->categoryComboBox->currentText() != "WCAs") {
        select += "_";
        select += MyModel::returnToUnderscores(ui->categoryComboBox->currentText());
    }


    myModel->queryDatabase(select, ui->categoryComboBox->currentText(), database);

    ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();

    if (myModel->released(s, database)) {
        ui->releasedLabel->setText("(Locked)");
        ui->releasedLabel->setStyleSheet("QLabel { color: #ff0000 }");

        ui->newFaultButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);

        createViewPage(true);
        adjustReleaseMenu(true);
    } else {
        ui->releasedLabel->setText("(Unlocked)");
        ui->releasedLabel->setStyleSheet("QLabel { color: #00aa00 }");

        ui->newFaultButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);

        createViewPage(false);
        adjustReleaseMenu(false);
    }

    updateReleasesDropdown(arg1);

    ui->versionComboBox->setCurrentText(arg1);
    updateViewMenu("");

    onShowAllColumnsTriggered();

//    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(Qt::ArrowCursor); //removes infinite loading cursor at startup
    QApplication::processEvents();
}

//sets up default fault view page buttons and fields
void MainWindow::createViewPage(bool released) {
    if (released == true) {
        //ghosts both edit and save buttons to prevent edits
        ghostEditFaults(false);
        ghostSaveFaults(false);
        //make edit buttons appear
        hideEditFaults(true);
    } else {
        //ghosts save buttons only, allowing user to edit
        ghostEditFaults(true);
        ghostSaveFaults(false);
        //make edit buttons appear
        hideEditFaults(true);
    }
    makeReadOnlyFields(true);

    QItemSelectionModel *select = ui->tableView->selectionModel();

    //update title
    if (ui->categoryComboBox->currentText() == "Failsafes") {
        QString fail_id = select->selectedRows(1).value(0).data().toString();
        ui->failsafeViewLabel->setText("VIEW FAILSAFE " + fail_id);
    } else if (ui->categoryComboBox->currentText() == "Interlocks") {
        QString int_id = select->selectedRows(1).value(0).data().toString();
        ui->interlockViewLabel->setText("VIEW INTERLOCK " + int_id);
    } else if (ui->categoryComboBox->currentText() == "System Notes") {
        ui->systemNoteViewLabel->setText("VIEW SYSTEM NOTE");
    } else {
        QString fault_id = select->selectedRows(4).value(0).data().toString();
        ui->faultViewLabel->setText("VIEW FAULT " + fault_id);
    }
}

//sets up edit fault mode
void MainWindow::createEditPage(bool readonly) {
    QItemSelectionModel *select = ui->tableView->selectionModel();


    //update title
    if (ui->categoryComboBox->currentText() == "Failsafes") {
        QString fail_id = select->selectedRows(1).value(0).data().toString();
        ui->failsafeViewLabel->setText("EDIT FAILSAFE " + fail_id);
    } else if (ui->categoryComboBox->currentText() == "Interlocks") {
        QString int_id = select->selectedRows(1).value(0).data().toString();
        ui->interlockViewLabel->setText("EDIT INTERLOCK " + int_id);
    } else if (ui->categoryComboBox->currentText() == "System Notes") {
        ui->systemNoteViewLabel->setText("EDIT SYSTEM NOTE");
    } else {
        QString fault_id = select->selectedRows(4).value(0).data().toString();
        ui->faultViewLabel->setText("EDIT FAULT " + fault_id);
    }

    //make fields editable and display save buttons
    makeReadOnlyFields(false);
    ghostEditFaults(readonly);
    ghostSaveFaults(!readonly);
}

// Toggles read only for all of the form fields based on argument
void MainWindow::makeReadOnlyFields(bool readonly) {
    QTabWidget *temp = getTabWidget();

    foreach (QLineEdit* le, temp->findChildren<QLineEdit*>())
        le->setReadOnly(readonly);

    foreach (QPlainTextEdit* pt, temp->findChildren<QPlainTextEdit*>())
        pt->setReadOnly(readonly);

    foreach (QComboBox* cb, temp->findChildren<QComboBox*>()) {
        cb->setEnabled(!readonly);
        cb->lineEdit()->setReadOnly(!readonly);
    }
}

void MainWindow::ghostEditFaults(bool visible) {
    //hide all edit buttons in detailed fault view
    ui->editMCMUSVButton->setEnabled(visible);
    ui->editWCAButton->setEnabled(visible);
    ui->editResponseButton->setEnabled(visible);
    ui->editOtherButton->setEnabled(visible);
    ui->editFailButton->setEnabled(visible);
    ui->editGeneralFailButton->setEnabled(visible);
    ui->editResponseFailButton->setEnabled(visible);
    ui->editOtherFailButton->setEnabled(visible);
    ui->editIntButton->setEnabled(visible);
    ui->editGeneralSysButton->setEnabled(visible);
    ui->editNotesSysButton->setEnabled(visible);
}

void MainWindow::hideEditFaults(bool ghost) {
    //ghost all edit buttons in detailed fault view
    ui->editMCMUSVButton->setVisible(ghost);
    ui->editWCAButton->setVisible(ghost);
    ui->editResponseButton->setVisible(ghost);
    ui->editOtherButton->setVisible(ghost);
    ui->editFailButton->setVisible(ghost);
    ui->editGeneralFailButton->setVisible(ghost);
    ui->editResponseFailButton->setVisible(ghost);
    ui->editOtherFailButton->setVisible(ghost);
    ui->editIntButton->setVisible(ghost);
    ui->editGeneralSysButton->setVisible(ghost);
    ui->editNotesSysButton->setVisible(ghost);
}

void MainWindow::ghostSaveFaults(bool ghost) {
    //ghost all save buttons in detailed fault view
    ui->saveMCMUSVButton->setEnabled(ghost);
    ui->saveWCAButton->setEnabled(ghost);
    ui->saveResponseButton->setEnabled(ghost);
    ui->saveOtherButton->setEnabled(ghost);
    ui->saveFailButton->setEnabled(ghost);
    ui->saveGeneralFailButton->setEnabled(ghost);
    ui->saveResponseFailButton->setEnabled(ghost);
    ui->saveOtherFailButton->setEnabled(ghost);
    ui->saveIntButton->setEnabled(ghost);
    ui->saveGeneralSysButton->setEnabled(ghost);
    ui->saveNotesSysButton->setEnabled(ghost);
    ui->addFaultCodeFailButton->setEnabled(ghost);
    ui->addFaultCodeIntButton->setEnabled(ghost);
    ui->deleteFaultCodeFailButton->setEnabled(ghost);
    ui->deleteFaultCodeIntButton->setEnabled(ghost);
};

void MainWindow::closeEvent() {
    myModel->closeConnection();
    qInfo() << "Connection closed";
}

void MainWindow::on_searchLineEdit_textChanged()
{
    myModel->refreshFilters(ui->searchLineEdit->text(), getCustomView(), ui->categoryComboBox->currentText());
    ui->searchLineEdit->setFocus();
}

// Send query to get all releases in database and update the dropdown
void MainWindow::updateReleasesDropdown(QString selectedText) {
    ui->versionComboBox->clear();
    std::vector<QString> releases = myModel->fetchCurrentReleases(database);
    int i = 0;
    int newSelectedIndex = 0;
    for (QString release : releases) {
        MyModel::convertToDecimals(&release);
        ui->versionComboBox->addItem(release);
        if (release == selectedText) {
            newSelectedIndex = i;
        }
        i++;
    }

    ui->versionComboBox->setCurrentIndex(newSelectedIndex);
}

void MainWindow::on_actionNew_Release_triggered()
{
    ui->stackedWidget->setCurrentIndex(2);
    updateReleasesDropdown(ui->versionComboBox->currentText());
    ui->comboBox_15->clear();
    ui->lineEdit_3->clear();

    for (int i = 0; i < ui->versionComboBox->count(); i++) {
        ui->comboBox_15->addItem(ui->versionComboBox->itemText(i));
    }

    for (int i = 0; i < ui->comboBox_15->count(); i++) {
        if (ui->comboBox_15->itemText(i) == ui->versionComboBox->currentText()) {
            ui->comboBox_15->setCurrentIndex(i);
            break;
        }
    }
}

// Mark the currently selected release as locked
void MainWindow::markAsLocked() {
    QString currentRelease = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&currentRelease);
    QSqlQuery q;


    q.exec("update releases set status = 'released' where version = '" + currentRelease + "'");

    MyModel::convertToDecimals(&currentRelease);
    on_versionComboBox_activated(currentRelease);
}

// Delete the currently selected release (delegates to deleteRelease())
void MainWindow::on_actionDelete_Release_triggered()
{
    QMessageBox msg;
    msg.setWindowTitle("Delete Release");
    msg.setText("Are you sure you want to delete this release? It will remove all data from this release from the database");
    msg.setStandardButtons(QMessageBox::Ok);
    msg.addButton(QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Ok);

    if (msg.exec() == QMessageBox::Ok) {
        on_versionComboBox_activated(deleteRelease(ui->versionComboBox->currentText(), false));
    }
}

// Rename the currently selected release
void MainWindow::on_actionRename_triggered()
{
    bool responded;
    QString newReleaseName = QInputDialog::getText(this, "Rename Release", "Enter new name for release", QLineEdit::Normal, QString(), &responded, Qt::MSWindowsFixedSizeDialogHint);
    if (!responded)
        return;

    if (newReleaseName.size() > 0) {
        QString oldReleaseName = ui->versionComboBox->currentText();

        myModel->createNewRelease(newReleaseName, oldReleaseName, true);
        deleteRelease(ui->versionComboBox->currentText(), true);
        myModel->updateParentColumn(newReleaseName, oldReleaseName);

        on_versionComboBox_activated(newReleaseName);
    }
}

// Delete the given release and return the name of a valid table to display afterwards (or "" if none exists)
QString MainWindow::deleteRelease(QString currentTable, bool isRename) {
    QSqlQuery q;
    QSqlQuery u;
    MyModel::convertToUnderscores(&currentTable);

    // Update the parent column in the releases table to remove the deleted release
    if (!isRename) {
        myModel->updateParentColumn("NULL", currentTable);
    }

    // Attempt to find the parent table to display after deletion
    // TODO: currently not working - still displays the first entry in the combo box
    q.exec("drop table " + currentTable);
    q.exec("drop table " + currentTable + "_failsafes");
    q.exec("drop table " + currentTable + "_interlocks");
    q.exec("drop table " + currentTable + "_system_notes");
    q.exec("select parent from releases where version = '" + currentTable + "'");
    QString parentTable;
    q.next();
    parentTable = q.value(0).toString();
    q.exec("delete from releases where version = '" + currentTable + "'");
    if (parentTable.size() > 0) {
        q.exec("select version from releases where version = '" + parentTable + "'");
        if (q.next() && q.value(0).toString().size() > 0) {
            qInfo() << "displaying parent table named " + parentTable;
            return parentTable;
        }
    }

    q.exec("select version from releases limit 1");
    QString newDisplayedRelease = "";

    if (q.next()) {
        newDisplayedRelease = q.value(0).toString();
    }
    else {
        qInfo() << "No tables left to display";
    }

    return newDisplayedRelease;
}

// Change the currently selected release from locked to unlocked
void MainWindow::on_actionEdit_Release_triggered()
{
    QSqlQuery q;
    bool responded;

    while (true) {
        QString entry = QInputDialog::getText(this, "Unlock Release", "Enter password:", QLineEdit::Normal, QString(), &responded, Qt::MSWindowsFixedSizeDialogHint);
        if (!responded)
            return;

        QString currentTable = ui->versionComboBox->currentText();
        MyModel::convertToUnderscores(&currentTable);

        QByteArray hashedEntry = QCryptographicHash::hash(entry.toLocal8Bit(), QCryptographicHash::Sha1);

        if (hashedEntry == passwordhash) {
            q.exec("update releases set status = 'working' where version = '" + currentTable + "'");
            on_versionComboBox_activated(ui->versionComboBox->currentText());
            return;
        }
        else {
            QMessageBox msg;
            msg.setWindowTitle("Incorrect");
            msg.setIcon(QMessageBox::Icon::Warning);
            msg.addButton(QMessageBox::Ok);
            msg.setText("Incorrect Password");
            msg.exec();
        }
    }
}

// Update visibilty of release menu options depending on the given read only (locked) status
void MainWindow::adjustReleaseMenu(bool readonly) {
    ui->actionMark_as_Released->setEnabled(!readonly);
    ui->actionEdit_Release->setEnabled(readonly);
    ui->actionDelete_Release->setEnabled(!readonly);
    ui->actionRename->setEnabled(!readonly);
}

void MainWindow::on_actionMark_as_Released_triggered()
{
    QMessageBox msg;
    msg.setWindowTitle("Lock Release");
    msg.setText("Are you sure you want to lock this release? It will prevent further changes to any data in this release.");
    msg.setStandardButtons(QMessageBox::Ok);
    msg.addButton(QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Ok);

    if (msg.exec() == QMessageBox::Ok) {
        markAsLocked();
    }
}


//used for highlight listeners
QString MainWindow::queryField(QString fieldname) {
    //determine row + version to make the query and pass query, row, testdb to mymodel query field function
    QItemSelectionModel *select = ui->tableView->selectionModel();

    //determine selected row and version
    QString row, query, version = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&version);

    if (ui->categoryComboBox->currentText() == "Failsafes") {
        row = select->selectedRows(1).value(0).data().toString();
        query = QStringLiteral("select %1 from %2_failsafes where failsafe_id like '%3'").arg(fieldname).arg(version).arg(row);
    } else if (ui->categoryComboBox->currentText() == "Interlocks") {
        row = select->selectedRows(1).value(0).data().toString();
        query = QStringLiteral("select %1 from %2_interlocks where interlock_id like '%3'").arg(fieldname).arg(version).arg(row);
    } else if (ui->categoryComboBox->currentText() == "System Notes") {
        row = select->selectedRows(0).value(0).data().toString();
        query = QStringLiteral("select %1 from %2_system_notes where id like '%3'").arg(fieldname).arg(version).arg(row);
    } else {
        row = select->selectedRows(0).value(0).data().toString();
        query = QStringLiteral("select %1 from %2 where id like '%3'").arg(fieldname).arg(version).arg(row);
    }

    QString result = myModel->getField(query, database);
    return result;
}


/******line edit listeners******/
void MainWindow::updateHighlightLE(QString text, QString fieldname, QLineEdit *le) {
    if (ui->faultViewLabel->text().contains("EDIT FAULT") || ui->failsafeViewLabel->text().contains("EDIT FAILSAFE")  || ui->interlockViewLabel->text().contains("EDIT INTERLOCK") || ui->systemNoteViewLabel->text().contains("EDIT SYSTEM NOTE")) {
        QString field = queryField(fieldname);
        QPalette palette;
        QColor color;

        if (field != text) {
            color = QColor(highlightColor);
            palette.setColor(QPalette::Base,color);
            le->setPalette(palette);
            le->setBackgroundRole(QPalette::Highlight); //IP
        } else {
            color = QColor("white");
            palette.setColor(QPalette::Base,color);
            le->setPalette(palette);
            le->setBackgroundRole(QPalette::Base);
        }
    }

    updateSaveButtons();
}

void MainWindow::on_failsafeIDLE_textEdited(const QString &text) {
    updateHighlightLE(text, ui->failsafeIDLE->accessibleName(), ui->failsafeIDLE);
}
void MainWindow::on_interlockIDLE_textEdited(const QString &text) {
    updateHighlightLE(text, ui->interlockIDLE->accessibleName(), ui->interlockIDLE);
}

void MainWindow::on_groupLE_textEdited(const QString &text) {
    updateHighlightLE(text, "group_name", ui->groupLE);
}

void MainWindow::on_enumNameLE_textEdited(const QString &text) {
    updateHighlightLE(text, "enum_name", ui->enumNameLE);
}

void MainWindow::on_faultCodeLE_textEdited(const QString &text) {
    updateHighlightLE(text, "fault_code", ui->faultCodeLE);
}

void MainWindow::on_changeeLE_textEdited(const QString &text) {
    updateHighlightLE(text, "changee", ui->changeeLE);
}

void MainWindow::on_faultGroupLE_textEdited(const QString &text) {
    updateHighlightLE(text, "fault_group", ui->faultGroupLE);
}

void MainWindow::on_failsafeCodeLE_textEdited(const QString &text) {
    updateHighlightLE(text, "failsafe_code", ui->failsafeCodeLE);
}

void MainWindow::on_interlockCodeLE_textEdited(const QString &text) {
    updateHighlightLE(text, "interlock_code", ui->interlockCodeLE);
}

void MainWindow::on_changeeIntLE_textEdited(const QString &text) {
    updateHighlightLE(text, "changee", ui->changeeIntLE);
}

void MainWindow::on_racLE_textEdited(const QString &text) {
    updateHighlightLE(text, "rac", ui->racLE);
}

void MainWindow::on_mitigatedRACLE_textEdited(const QString &text) {
    updateHighlightLE(text, "mitigated_rac", ui->mitigatedRACLE);
}

void MainWindow::on_applicableSafetyPreceptLE_textEdited(const QString &text) {
    updateHighlightLE(text, "applicable_safety_precept", ui->applicableSafetyPreceptLE);
}

void MainWindow::on_changeeFailLE_textEdited(const QString &text) {
    updateHighlightLE(text, "changee", ui->changeeFailLE);
}

void MainWindow::on_deviceLE_textEdited(const QString &text) {
    updateHighlightLE(text, "device", ui->deviceLE);
}

void MainWindow::on_pcrNumLE_textEdited(const QString &text) {
    updateHighlightLE(text, "pcr_num", ui->pcrNumLE);
}

void MainWindow::on_changeeSysLE_textEdited(const QString &text) {
    updateHighlightLE(text, "changee", ui->changeeSysLE);
}

void MainWindow::on_summarySysLE_textEdited(const QString &text) {
    updateHighlightLE(text, "summary", ui->summarySysLE);
}

/***** End Line Edit Listeners *******/

/******Plain text edit listeners******/
void MainWindow::updateHighlightPT(QString text, QString fieldname, QPlainTextEdit *pt) {
    if (ui->faultViewLabel->text().contains("EDIT FAULT") || ui->failsafeViewLabel->text().contains("EDIT FAILSAFE") || ui->interlockViewLabel->text().contains("EDIT INTERLOCK") || ui->systemNoteViewLabel->text().contains("EDIT SYSTEM NOTE")) {
        QString field = queryField(fieldname);
        QPalette palette;
        QColor color;

        if (field != text) {
            color = QColor(highlightColor);
            palette.setColor(QPalette::Base,color);
            pt->setPalette(palette);
            pt->setBackgroundRole(QPalette::Highlight);
        } else {
            color = QColor("white");
            palette.setColor(QPalette::Base,color);
            pt->setPalette(palette);
            pt->setBackgroundRole(QPalette::Base);
        }
    }

    updateSaveButtons();
}

void MainWindow::on_failureNamePT_textChanged() {
    updateHighlightPT(ui->failureNamePT->toPlainText(), ui->failureNamePT->accessibleName(), ui->failureNamePT);
}

void MainWindow::on_operationalStatePT_textChanged() {
    updateHighlightPT(ui->operationalStatePT->toPlainText(), ui->operationalStatePT->accessibleName(), ui->operationalStatePT);
}

void MainWindow::on_triggersPT_textChanged() {
    updateHighlightPT(ui->triggersPT->toPlainText(), ui->triggersPT->accessibleName(), ui->triggersPT);
}

//void MainWindow::on_triggeringFaultsPT_textChanged() {
//    //updateHighlightPT(ui->triggeringFaultsPT->toPlainText(), ui->triggeringFaultsPT->accessibleName(), ui->triggeringFaultsPT);
//}
void MainWindow::on_indicationMethodPT_textChanged() {
    updateHighlightPT(ui->indicationMethodPT->toPlainText(), ui->indicationMethodPT->accessibleName(), ui->indicationMethodPT);
}

void MainWindow::on_functionalImpactPT_textChanged() {
    updateHighlightPT(ui->functionalImpactPT->toPlainText(), ui->functionalImpactPT->accessibleName(), ui->functionalImpactPT);
}

void MainWindow::on_firstResponsePT_textChanged() {
    updateHighlightPT(ui->firstResponsePT->toPlainText(), ui->firstResponsePT->accessibleName(), ui->firstResponsePT);
}

void MainWindow::on_secondResponsePT_textChanged() {
    updateHighlightPT(ui->secondResponsePT->toPlainText(), ui->secondResponsePT->accessibleName(), ui->secondResponsePT);
}

void MainWindow::on_thirdResponsePT_textChanged(){
    updateHighlightPT(ui->thirdResponsePT->toPlainText(), ui->thirdResponsePT->accessibleName(), ui->thirdResponsePT);
}

void MainWindow::on_interlockDescriptionPT_textChanged(){
    updateHighlightPT(ui->interlockDescriptionPT->toPlainText(), ui->interlockDescriptionPT->accessibleName(), ui->interlockDescriptionPT);
}

//void MainWindow::on_associatedWcasPT_textChanged() {
//    //updateHighlightPT(ui->associatedWcasPT->toPlainText(), ui->associatedWcasPT->accessibleName(), ui->associatedWcasPT);
//}

void MainWindow::on_wcaMessagePT_textChanged() {
    updateHighlightPT(ui->wcaMessagePT->toPlainText(), "wca_message", ui->wcaMessagePT);
}

void MainWindow::on_wcaTriggerPT_textChanged() {
    updateHighlightPT(ui->wcaTriggerPT->toPlainText(), "wca_trigger", ui->wcaTriggerPT);
}

void MainWindow::on_failureImpactPT_textChanged() {
    updateHighlightPT(ui->failureImpactPT->toPlainText(), "failure_impact", ui->failureImpactPT);
}

void MainWindow::on_failsafeSummaryPT_textChanged() {
    updateHighlightPT(ui->failsafeSummaryPT->toPlainText(), "failsafe_summary", ui->failsafeSummaryPT);
}

void MainWindow::on_onboardLCSPT_textChanged() {
    updateHighlightPT(ui->onboardLCSPT->toPlainText(), "onboard_lcs", ui->onboardLCSPT);
}

void MainWindow::on_offboardLCSPT_textChanged() {
    updateHighlightPT(ui->offboardLCSPT->toPlainText(), "offboard_lcs", ui->offboardLCSPT);
}

void MainWindow::on_lcsLRPT_textChanged() {
    updateHighlightPT(ui->lcsLRPT->toPlainText(), "lcs_lr", ui->lcsLRPT);
}

void MainWindow::on_correctiveMaintenanceActionsPT_textChanged() {
    updateHighlightPT(ui->correctiveMaintenanceActionsPT->toPlainText(), "corrective_maintenance_actions", ui->correctiveMaintenanceActionsPT);
}

void MainWindow::on_wcaNotesPT_textChanged() {
    updateHighlightPT(ui->wcaNotesPT->toPlainText(), "wca_notes", ui->wcaNotesPT);
}

void MainWindow::on_notesPT_textChanged() {
    updateHighlightPT(ui->notesPT->toPlainText(), "notes", ui->notesPT);
}

void MainWindow::on_internalNotesIntPT_textChanged() {
    updateHighlightPT(ui->internalNotesIntPT->toPlainText(), "internal_notes", ui->internalNotesIntPT);
}

void MainWindow::on_failsafeNotesPT_textChanged() {
    updateHighlightPT(ui->failsafeNotesPT->toPlainText(), "failsafe_notes", ui->failsafeNotesPT);
}

void MainWindow::on_failsafeSummaryFailPT_textChanged() {
    updateHighlightPT(ui->failsafeSummaryFailPT->toPlainText(), "failsafe_summary", ui->failsafeSummaryFailPT);
}

void MainWindow::on_internalNotesFailPT_textChanged() {
    updateHighlightPT(ui->internalNotesFailPT->toPlainText(), "internal_notes", ui->internalNotesFailPT);
}

void MainWindow::on_descriptionSysPT_textChanged() {
    updateHighlightPT(ui->descriptionSysPT->toPlainText(), "description", ui->descriptionSysPT);
}

void MainWindow::on_workaroundsPT_textChanged() {
    updateHighlightPT(ui->workaroundsPT->toPlainText(), "workarounds", ui->workaroundsPT);
}

void MainWindow::on_internalNotesSysPT_textChanged() {
    updateHighlightPT(ui->internalNotesSysPT->toPlainText(), "internal_notes", ui->internalNotesSysPT);
}

void MainWindow::on_externalNotesSysPT_textChanged() {
    updateHighlightPT(ui->externalNotesSysPT->toPlainText(), "external_notes", ui->externalNotesSysPT);
}

void MainWindow::on_wcaClearingPT_textChanged() {
    updateHighlightPT(ui->wcaClearingPT->toPlainText(), "wca_clearing", ui->wcaClearingPT);
}

void MainWindow::on_failsafeClearingPT_textChanged() {
    updateHighlightPT(ui->failsafeClearingPT->toPlainText(), "failsafe_clearing", ui->failsafeClearingPT);
}
/******* End Plain Text Listeners ******/

/******* Combobox Listeners ************/
void MainWindow::updateHighlightCB(QString text, QString fieldname, QComboBox *cb) {
    if (ui->faultViewLabel->text().contains("EDIT FAULT") || ui->failsafeViewLabel->text().contains("EDIT FAILSAFE") || ui->interlockViewLabel->text().contains("EDIT INTERLOCK") || ui->systemNoteViewLabel->text().contains("EDIT SYSTEM NOTE")) {
        QString field = queryField(fieldname);
        QPalette palette;
        QColor color;

        cb->setEditable(true);
        QLineEdit *lineEditor = cb->lineEdit();
        lineEditor->setReadOnly(true);




        if (field != text) {
            color = QColor(highlightColor);
            palette.setColor(QPalette::Base,color);
            cb->lineEdit()->setPalette(palette);
            cb->lineEdit()->setBackgroundRole(QPalette::Highlight);
        } else {
            color = QColor("white");
            palette.setColor(QPalette::Base,color);
            cb->lineEdit()->setPalette(palette);
            cb->lineEdit()->setBackgroundRole(QPalette::Base);
        }
    }

    updateSaveButtons();
}

void MainWindow::on_fmStatus2CB_currentIndexChanged(int) {
    updateHighlightCB(ui->fmStatus2CB->currentText(), ui->fmStatus2CB->accessibleName(), ui->fmStatus2CB);
}
void MainWindow::on_fmStatus3CB_currentIndexChanged(int) {
    updateHighlightCB(ui->fmStatus3CB->currentText(), ui->fmStatus3CB->accessibleName(), ui->fmStatus3CB);
}

void MainWindow::on_componentsCB_currentIndexChanged(int) {
    updateHighlightCB(ui->componentsCB->currentText(), ui->componentsCB->accessibleName(), ui->componentsCB);
}

void MainWindow::on_fmStatusCB_currentIndexChanged(int) {
    QString text = ui->fmStatusCB->currentText();

    if (text == "OBE")
        text = "OBE - Keep b/c ENUM in SW";

    updateHighlightCB(text, "fm_status", ui->fmStatusCB);
}

void MainWindow::on_wcaSeverityCB_currentIndexChanged(int) {
    updateHighlightCB(ui->wcaSeverityCB->currentText(), "wca_severity", ui->wcaSeverityCB);
}

void MainWindow::on_configurationFaultCB_currentIndexChanged(int) {
    updateHighlightCB(ui->configurationFaultCB->currentText(), "configuration", ui->configurationFaultCB);
}

void MainWindow::on_configurationIntCB_currentIndexChanged(int) {
    updateHighlightCB(ui->configurationIntCB->currentText(), "configuration", ui->configurationIntCB);
}

void MainWindow::on_configurationFailCB_currentIndexChanged(int) {
    updateHighlightCB(ui->configurationFailCB->currentText(), "configuration", ui->configurationFailCB);
}

void MainWindow::on_fmStatus4CB_currentIndexChanged(int) {
    updateHighlightCB(ui->fmStatus4CB->currentText(), "status", ui->fmStatus4CB);
}

void MainWindow::on_configurationSysCB_currentIndexChanged(int) {
    updateHighlightCB(ui->configurationSysCB->currentText(), "configuration", ui->configurationSysCB);
}

void MainWindow::on_operatorCB_currentIndexChanged(int) {
    updateHighlightCB(ui->operatorCB->currentText(), "operator", ui->operatorCB);
}

void MainWindow::on_faultGroupSysCB_currentIndexChanged(int) {
    updateHighlightCB(ui->faultGroupSysCB->currentText(), "fault_group", ui->faultGroupSysCB);
}

/******* End Combobox Listeners ******/


/******* Start Tab Widget Button Listeners ******/
//listeners for buttons in each page in the default fault view tab widget

void MainWindow::on_nextMCMUSVButton_2_clicked()
{
    ui->tabWidget_2->setCurrentIndex(1);
}

//delete later
void MainWindow::on_backMCMUSVButton_2_clicked()
{
    ui->tabWidget_2->setCurrentIndex(0);
}

void MainWindow::on_nextWCAButton_2_clicked()
{
    ui->tabWidget_2->setCurrentIndex(2);
}

void MainWindow::on_backWCAButton_2_clicked()
{
    ui->tabWidget_2->setCurrentIndex(0);
}

void MainWindow::on_backFailButton_clicked()
{
    ui->tabWidget_2->setCurrentIndex(1);
}

void MainWindow::on_backResponseFailsafeButton_clicked() {
    ui->tabWidget_3->setCurrentIndex(0);
}

void MainWindow::on_nextGeneralFailsafeButton_clicked() {
    ui->tabWidget_3->setCurrentIndex(1);
}

void MainWindow::on_nextResponseFailsafeButton_clicked() {
    ui->tabWidget_3->setCurrentIndex(2);
}
void MainWindow::on_backChangelogFailsafeButton_clicked() {
    ui->tabWidget_3->setCurrentIndex(2);
}
void MainWindow::on_backOtherFailsafeButton_clicked() {
    ui->tabWidget_3->setCurrentIndex(1);
}
void MainWindow::on_nextOtherFailsafeButton_clicked() {
    ui->tabWidget_3->setCurrentIndex(3);
}
void MainWindow::on_nextGeneralIntButton_clicked() {
    ui->tabWidget_4->setCurrentIndex(1);
}
void MainWindow::on_backChangeLogIntButton_clicked() {
    ui->tabWidget_4->setCurrentIndex(0);
}

void MainWindow::on_nextGeneralSysButton_clicked() {
    ui->tabWidget_5->setCurrentIndex(1);
}

void MainWindow::on_nextNotesSysButton_clicked() {
    ui->tabWidget_5->setCurrentIndex(2);
}

void MainWindow::on_backNotesSysButton_clicked() {
    ui->tabWidget_5->setCurrentIndex(0);
}

void MainWindow::on_backChangeLogSysButton_clicked() {
    ui->tabWidget_5->setCurrentIndex(1);
}

void MainWindow::on_nextGeneralFRButton_clicked() {
    ui->tabWidget_6->setCurrentIndex(1);
}

void MainWindow::on_backChangelogFRButton_clicked() {
    ui->tabWidget_6->setCurrentIndex(0);
}

void MainWindow::on_nextFailButton_clicked()
{
    ui->tabWidget_2->setCurrentIndex(3);
}

void MainWindow::on_nextResponseButton_2_clicked()
{
    ui->tabWidget_2->setCurrentIndex(4);
}

void MainWindow::on_nextOtherButton_2_clicked() {
    ui->tabWidget_2->setCurrentIndex(5);
}

void MainWindow::on_backResponseButton_2_clicked()
{
    ui->tabWidget_2->setCurrentIndex(2);
}

void MainWindow::on_backOtherButton_2_clicked()
{
    ui->tabWidget_2->setCurrentIndex(3);
}

void MainWindow::on_backChangeLogButton_2_clicked() {
    ui->tabWidget_2->setCurrentIndex(4);
}

void MainWindow::on_cancelMCMUSVButton_2_clicked()
{
    cancelEdit();

}

void MainWindow::on_cancelFailButton_clicked()
{
    cancelEdit();

}

void MainWindow::on_cancelWCAButton_2_clicked()
{
    cancelEdit();
}

void MainWindow::on_cancelResponseButton_2_clicked()
{
    cancelEdit();
}

void MainWindow::on_cancelOtherButton_2_clicked()
{
    cancelEdit();
}

void MainWindow::on_cancelChangeLogButton_clicked() {
    cancelEdit();
}

void MainWindow::on_cancelGeneralFailButton_clicked() {
    cancelEdit();
}

void MainWindow::on_cancelResponseFailButton_clicked() {
    cancelEdit();
}

void MainWindow::on_cancelOtherFailButton_clicked() {
    cancelEdit();
}

void MainWindow::on_cancelIntButton_clicked() {
    cancelEdit();
}

void MainWindow::on_cancelChangeLogFailsafeButton_clicked() {
    cancelEdit();
}

void MainWindow::on_cancelChangeLogIntButton_clicked() {
    cancelEdit();
}

void MainWindow::on_cancelGeneralSysButton_clicked() {
    cancelEdit();
}

void MainWindow::on_cancelNotesSysButton_clicked() {
    cancelEdit();
}

void MainWindow::on_cancelChangeLogSysButton_clicked() {
    cancelEdit();
}

void MainWindow::on_editMCMUSVButton_clicked() {
    createEditPage(false);
}

void MainWindow::on_editWCAButton_clicked() {
    createEditPage(false);
}

void MainWindow::on_editFailButton_clicked()
{
    createEditPage(false);
}

void MainWindow::on_editResponseButton_clicked() {
    createEditPage(false);
}

void MainWindow::on_editOtherButton_clicked() {
    createEditPage(false);
}

void MainWindow::on_editGeneralFailButton_clicked() {
    createEditPage(false);
}

void MainWindow::on_editResponseFailButton_clicked() {
    createEditPage(false);
}

void MainWindow::on_editOtherFailButton_clicked() {
    createEditPage(false);
}

void MainWindow::on_editIntButton_clicked() {
    createEditPage(false);
}

void MainWindow::on_editGeneralSysButton_clicked() {
    createEditPage(false);
}

void MainWindow::on_editNotesSysButton_clicked() {
    createEditPage(false);
}

/******* End Tab Widget Button Listeners ******/

/******* Start Export Menu Option Listeners ******/

void MainWindow::on_actionfaultconstants_h_triggered()
{
    // Use -1 for the payload field to ignore payload in export
    exporter->exportFaultConstants(ui->versionComboBox->currentText(), this, -1);
}

void MainWindow::on_actionfaultstringlib_h_triggered()
{
    // Use -1 for the payload field to ignore payload in export
    exporter->exportFaultStringLibh(ui->versionComboBox->currentText(), this, -1);
}

void MainWindow::on_actionfaultstringlib_cpp_triggered()
{
    // Use -1 for the payload field to ignore payload in export
    exporter->exportFaultStringLibcpp(ui->versionComboBox->currentText(), this, -1);
}

void MainWindow::on_categoryComboBox_activated(const QString)
{
    while (filterDisplays.size() > 0) {
        onFilterClose(filterDisplays.firstKey());
    }
    on_versionComboBox_activated(ui->versionComboBox->currentText());
}

void MainWindow::on_actionFaults_triggered()
{
    exporter->exportToCSV(ui->versionComboBox->currentText(), this, "WCAs");
}

void MainWindow::on_actionFailsafes_triggered()
{
    exporter->exportToCSV(ui->versionComboBox->currentText(), this, "Failsafes");
}

void MainWindow::on_actionInterlocks_triggered()
{
    exporter->exportToCSV(ui->versionComboBox->currentText(), this, "Interlocks");
}

void MainWindow::on_actionSystem_Notes_triggered()
{
    exporter->exportToCSV(ui->versionComboBox->currentText(), this, "System Notes");
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex)
{
    on_editButton_clicked();
}

void MainWindow::on_lineEdit_8_returnPressed()
{
    on_applyFiltersButton_clicked();
}

// Update the "View" menu bar option's actions. Populating it programatically like this
// instead of setting it up in the visual editor is necessary because the contents will change
// with both what type of data is displayed (faults, interlocks, or failsafes) and what views
// the user has locally created.
void MainWindow::updateViewMenu(QString previousView) {
    ui->menuView->clear();
    viewBar.clear();
    myModel->viewMap.clear();

    // Add and connect actions for showing all columns and showing no columns
    QAction * allAction = ui->menuView->addAction("Show All Columns");
    allAction->setCheckable(false);
    connect(allAction, SIGNAL(triggered()), this, SLOT(onShowAllColumnsTriggered()));

    QAction * noneAction = ui->menuView->addAction("Show No Columns");
    noneAction->setCheckable(false);
    connect(noneAction, SIGNAL(triggered()), this, SLOT(onShowNoColumnsTriggered()));

    // Create and fill sub-menu with all column names and set to currently checked (displayed)
    QMenu * submenu = ui->menuView->addMenu("Select Columns");
    for (QString item : myModel->columnMap.value("header_name").value(ui->categoryComboBox->currentText())) {
        QAction * newAction = submenu->addAction(item);
        viewBar.append(newAction);
        newAction->setCheckable(true);
        newAction->setChecked(true);

        if (item == "ID" || ((item == "Failsafe Code" || item == "Interlock Code") && ui->categoryComboBox->currentText() != "WCAs")) {
            newAction->setEnabled(false);
        }
        connect(newAction, SIGNAL(triggered()), this, SLOT(onToggleColumnTriggered()));
    }

    // Create and fill a section with the names of the user's previously created views
    ui->menuView->addSeparator();
    if (myModel->settings->childGroups().contains("views")) { // if at least one custom view has been created and saved previously
        QString path = "views/" + MyModel::returnToUnderscores(ui->categoryComboBox->currentText());
        myModel->settings->beginGroup(path);
        QStringList viewNames = myModel->settings->childGroups();
        myModel->settings->endGroup();

        // Populate the myModel->viewMap map with a new entry:
        // Key: pointer to the menu bar option for this view
        // Value: map correlating the header name of each column to whether it is displayed
        for (QString viewName : viewNames) {
            myModel->settings->beginGroup(path + "/" + viewName);
            QStringList keys = myModel->settings->childKeys();
            QAction * newAction = ui->menuView->addAction(viewName);
            myModel->viewMap.insert({newAction, QMap<QString, bool>()});
            newAction->setCheckable(true);
            newAction->setChecked(false);
            connect(newAction, SIGNAL(triggered()), this, SLOT(load_view()));

            // Populate the single myModel->viewMap entry with all the individual fields
            // and whether they each are displayed or not based on the saved view data
            foreach (QString key, keys) {
                QMap<QString, bool> *tempMap = &myModel->viewMap[newAction];
                tempMap->insert(key, myModel->settings->value(key).toBool());
            }
            myModel->settings->endGroup();
        }
    }

    // Create and fill a section with buttons to save and delete views
    ui->menuView->addSeparator();
    connect(ui->menuView->addAction("Save Current View As..."), SIGNAL(triggered()), this, SLOT(onSaveViewTriggered()));
    connect(ui->menuView->addAction("Delete Current View"), SIGNAL(triggered()), this, SLOT(onDeleteViewTriggered()));
    connect(ui->menuView->addAction("Delete All \"" + ui->categoryComboBox->currentText() + "\" Views"), SIGNAL(triggered()), this, SLOT(onDeleteViewsTriggered()));

    // Create and fill a section with button to resize all rows in table
    ui->menuView->addSeparator();
    connect(ui->menuView->addAction("Show Rows Compact"), SIGNAL(triggered()), this, SLOT(onShowCompactTriggered()));
    connect(ui->menuView->addAction("Show Rows Expanded"), SIGNAL(triggered()), this, SLOT(onShowExpandedTriggered()));

    // If the function was called with a parameter, finish by loading the given view
    if (previousView.size() > 0) {
        load_view(previousView);
    }
}

// Save the view that is currently configured in the application
void MainWindow::onSaveViewTriggered() {
    bool responded;
    QString viewName = QInputDialog::getText(this, "Save Current View", "Enter new name for view", QLineEdit::Normal, QString(), &responded, Qt::MSWindowsFixedSizeDialogHint);
    if (!responded)
        return;

    QMessageBox msg;

    if (viewName.size() > 0) {
        QString path = "views/" + MyModel::returnToUnderscores(ui->categoryComboBox->currentText());
        QMap<QString, bool> * tempMap = getCustomView();

        // Check if the settings already contains a view with the given name
        if (myModel->settings->contains(path + "/" + viewName + "/" + myModel->columnMap.value("header_name").value(ui->categoryComboBox->currentText()).at(0))) {
            msg.setWindowTitle("Overwrite Existing View?");
            msg.setText("There is already a saved view with the same name. Do you wish to continue and overwrite it?");
            msg.setStandardButtons(QMessageBox::Ok);
            msg.addButton(QMessageBox::Cancel);
            msg.setDefaultButton(QMessageBox::Ok);

            if (msg.exec() != QMessageBox::Ok) {
                return;
            }
        }

        myModel->settings->beginGroup(path + "/" + viewName);
        QMap<QString, bool>::const_iterator iter = tempMap->constBegin();
        int i = 0;
        while (iter != tempMap->constEnd()) {
            myModel->settings->setValue(iter.key(), iter.value());
            ++iter;
            i++;
        }
        myModel->settings->endGroup();
        myModel->settings->sync();
        // Update the view menu with the newly saved view.
        // Providing the name tells the function to go ahead and select this view afterwards
        updateViewMenu(viewName);
    }
}

// Show all the columns available for the current category of data
void MainWindow::onShowAllColumnsTriggered() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // Make sure all individual fields are shown enabled
    for (QAction * action : viewBar) {
        action->setChecked(true);
    }

    // Make sure menu is not still showing a specific view is selected
    for (auto e : myModel->viewMap) {
        e.first->setChecked(false);
    }

    myModel->updateColumnDisplay("", getCustomView(), ui->searchLineEdit->text(), ui->categoryComboBox->currentText());
    ui->tableView->resizeColumnsToContents();

    // Set the stylesheet of the header because it reverts sometimes for some reason
    ui->tableView->setStyleSheet(headerStyleSheet);

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
}

// Delete all the views currently stored on the user's machine
void MainWindow::onDeleteViewsTriggered() {

    QMessageBox msg;
    msg.setWindowTitle("Delete All Views");
    msg.setText("Are you sure you want to delete all views?");
    msg.setStandardButtons(QMessageBox::Ok);
    msg.addButton(QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Ok);

    if (msg.exec() == QMessageBox::Ok) {
        QString path = "views/" + MyModel::returnToUnderscores(ui->categoryComboBox->currentText());
        myModel->settings->beginGroup(path);
        QStringList viewNames = myModel->settings->childGroups();
        myModel->settings->endGroup();

        for (QString viewName : viewNames) {
            myModel->settings->beginGroup(path + "/" + viewName);
            myModel->settings->remove("");
            myModel->settings->endGroup();
        }

        myModel->settings->sync();
        updateViewMenu("");
        onShowAllColumnsTriggered();
    }
}

// Delete only a single view stored on the user's machiine
void MainWindow::onDeleteViewTriggered() {
    QString viewName;
    bool inView = false;

    for (auto e : myModel->viewMap) {
        if (e.first->isChecked()) {
            viewName = e.first->text();
            inView = true;
            break;
        }
    }

    if (inView) {
        QMessageBox msg;
        msg.setWindowTitle("Delete View");
        msg.setText("Are you sure you want to delete this view?");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.addButton(QMessageBox::Cancel);
        msg.setDefaultButton(QMessageBox::Ok);

        if (msg.exec() == QMessageBox::Ok) { // Allow user to cancel out without changes
            QString path = "views/" + MyModel::returnToUnderscores(ui->categoryComboBox->currentText());

            if (myModel->settings->contains(path + "/" + viewName + "/" + myModel->columnMap.value("header_name").value(ui->categoryComboBox->currentText()).at(0))) {
                myModel->settings->beginGroup(path + "/" + viewName);
                myModel->settings->remove("");
                myModel->settings->endGroup();

                myModel->settings->sync();
                updateViewMenu("");
                onShowAllColumnsTriggered();
            }
            else {
                QMessageBox::information(this, "View Not Found", "No view with the name \"" + viewName + "\" found. Please try again.", QMessageBox::Ok);
            }
        }
    }
    else { // No view currently selected
        QMessageBox::information(this, "View Not Selected", "No view is currently selected. Please select a view and then try again", QMessageBox::Ok);
    }
}

// Load a previously created view.
// The view to be loaded is determined by the sender of the signal which will
// be the menu option corresponding to this view. This function then just delegates
// to the other overloaded load_view(QString) function passing the name of the view
void MainWindow::load_view() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    QAction * obj = qobject_cast<QAction *>(sender());

    load_view(obj->text());

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
}

// Load a previously created view using the given name to find it.
// This function only loads the view and sets the correct checkbox configuration.
// It then calls myModel->updateColumnDisplay to actually update the table.
// Note: the views are already loaded into memeory in myModel->viewMap so there is
// no reading of QSettings here.
void MainWindow::load_view(QString viewName) {

    if (viewName.size() != 0) {
        QMap<QString, bool> *entry;

        // Loop through all saved views in myModel->viewMap looking for the one given as a parameter
        for (std::map<QAction *const, QMap<QString, bool>>::iterator iter = myModel->viewMap.begin(); iter != myModel->viewMap.end(); ++iter) {
            if (iter->first->text() == viewName) { // found the correct entry in the viewMap
                entry = &iter->second;
                iter->first->setChecked(true);
            }
            else {
                iter->first->setChecked(false);
            }
        }

        // Loop through all actions and set checked status based off of the viewMap entry
        for (auto a : viewBar) {
            a->setChecked(entry->value(a->text()));
        }
    }

    myModel->updateColumnDisplay(viewName, getCustomView(), ui->searchLineEdit->text(), ui->categoryComboBox->currentText());
    ui->tableView->resizeColumnsToContents();
}

// Show or unshow a single column after a click of a column checkbox
void MainWindow::onToggleColumnTriggered() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // Make sure menu is not still showing a specific view is selected
    for (auto e : myModel->viewMap) {
        e.first->setChecked(false);
    }
    // Call myModel->updateColumnDisplay but give "" as the view name communicates that no view is currently selected
    myModel->updateColumnDisplay("", getCustomView(), ui->searchLineEdit->text(), ui->categoryComboBox->currentText());
    ui->tableView->resizeColumnsToContents();

    ui->tableView->setStyleSheet(headerStyleSheet);

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
}

// Build a map to describe the current view.
// The map consists of one entry per field that is possible to display for the current category of data:
// Key: column header name
// Value: if the column is currently being displayed
QMap<QString, bool>* MainWindow::getCustomView() {
    QMap<QString, bool> *currentMap = new QMap<QString, bool>;

    for (auto e : viewBar) {
        currentMap->insert(e->text(), e->isChecked());
    }

    return currentMap;
}

// Show no columns in the view. Not actually useful to see no columns but it may
// be helpful so you don't have to uncheck every single one if you only want to see a single column.
void MainWindow::onShowNoColumnsTriggered() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // Set all individual columns to be not selected
    for (QAction * action : viewBar) {
        if (action->isEnabled())
            action->setChecked(false);
    }

    // Make sure menu is not still showing a specific view is selected
    for (auto e : myModel->viewMap) {
        e.first->setChecked(false);
    }

    myModel->updateColumnDisplay("", getCustomView(), ui->searchLineEdit->text(), ui->categoryComboBox->currentText());
    ui->tableView->resizeColumnsToContents();

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
}

// Change all displayed rows to be compact (single line).
// Because no function is available, I instead do it by removing all rows from the
// table and then adding them back which makes them single line unless expressly resized to contents
void MainWindow::onShowCompactTriggered() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // Update display with ________ as the search bar text which should result in no matches
    myModel->updateColumnDisplay("", getCustomView(), "________", ui->categoryComboBox->currentText());

    // Update display to restore what was previously being displayed
    for (auto e : myModel->viewMap) {
        if (e.first->isChecked()) { // If a view is selected refresh with that view
            updateViewMenu(e.first->text());

            QApplication::restoreOverrideCursor();
            QApplication::processEvents();

            return;
        }
    }

    // If no view is selected refresh with no view
    myModel->updateColumnDisplay("", getCustomView(), ui->searchLineEdit->text(), ui->categoryComboBox->currentText());

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
}

// Change all displayed rows to be expanded/fitted to row contents
void MainWindow::onShowExpandedTriggered() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    ui->tableView->resizeRowsToContents();

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
}

void MainWindow::on_actionReset_Database_Info_triggered()
{
    myModel->resetDbInfo(this);
    updateReleasesDropdown(ui->versionComboBox->currentText());
}

// Call export wca guidebook xml function for minehunt
void MainWindow::on_actionMH_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    // Pass in 1 for minehunt
    exporter->exportWcaGuidebookIndex(this, &q, releaseName, 1);
}

// Call export wca guidebook xml function for UISS
void MainWindow::on_actionUISS_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    // Pass in 0 fro UISS
    exporter->exportWcaGuidebookIndex(this, &q, releaseName, 0);
}

void MainWindow::on_actionOnboard_LCS_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookOnboardLCS(this, &q, releaseName, 0);
}

void MainWindow::on_actionMine_Warfare_Evaluator_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookMineWarfareEval(this, &q, releaseName, 0);
}

void MainWindow::on_actionLaunch_Recovery_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookLR(this, &q, releaseName, 0);
}

void MainWindow::on_actionOffboard_LCS_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookOffboardLCS(this, &q, releaseName, 0);
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if (arg1 > 0) { // Checked
        myModel->setShowingDeleted(true);
    }
    else { // Unchecked
        myModel->setShowingDeleted(false);
    }
    on_versionComboBox_activated(ui->versionComboBox->currentText());
}

void MainWindow::on_actionOffboard_LCS_2_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookOffboardLCS(this, &q, releaseName, 1);
}

void MainWindow::on_actionMine_Warfare_Evaluator_2_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookMineWarfareEval(this, &q, releaseName, 1);
}

void MainWindow::on_actionOnboard_LCS_2_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookOnboardLCS(this, &q, releaseName, 1);
}

void MainWindow::on_actionLaunch_and_Recovery_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookLR(this, &q, releaseName, 1);
}

void MainWindow::on_actionFailsafes_2_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookFailsafes(this, &q, releaseName, 0);
}

void MainWindow::on_actionInterlocks_2_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookInterlocks(this, &q, releaseName, 0);
}

void MainWindow::on_actionSystem_Notes_2_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookSystemNotes(this, &q, releaseName, 0);
}

void MainWindow::on_actionFailsafes_3_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookFailsafes(this, &q, releaseName, 1);
}

void MainWindow::on_actionInterlocks_3_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookInterlocks(this, &q, releaseName, 1);
}

void MainWindow::on_actionSystem_Notes_3_triggered()
{
    QSqlQuery q;
    QString releaseName = ui->versionComboBox->currentText();
    MyModel::convertToUnderscores(&releaseName);
    exporter->exportWcaGuidebookSystemNotes(this, &q, releaseName, 1);
}
