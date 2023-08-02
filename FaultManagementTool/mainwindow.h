#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mymodel.h"
#include "save_edits.h"
#include "save_add.h"
#include <QPlainTextEdit>
#include "exporter.h"
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    save_edits *saveEdit;
    save_add *saveAdd;
    QMap<QString, QWidget *> filterDisplays;
    QString tempButtonText;
    int tempButtonWidth;
    void updateViewMenu(QString previousView);

private slots:
    void showDeleteFaultPrompt();

    //filters page
    void on_applyFiltersButton_clicked();
    void on_cancelFiltersButton_clicked();

    //main window buttons
    void on_newFaultButton_clicked();
    void on_editButton_clicked();
    void deleteFault();
    void deleteFailsafe();
    void deleteInterlock();
    void deleteSystemNote();
    void on_createReleaseButton_clicked();
    void on_cancelReleaseButton_clicked();
    void on_addFilterButton_clicked();
    void cancelEdit();
    void on_searchLineEdit_textChanged();
    void on_versionComboBox_activated(const QString &arg1);
    void closeEvent();
    void setHomePage();
    bool updateSaveButtons();
    void setChangesBrowser(QTextBrowser *changes);
    void fillAssociatedWCAsList(QListWidget *list, QString codeType);

    //fault view tab widget buttons
    void on_nextMCMUSVButton_2_clicked();
    void on_nextWCAButton_2_clicked();
    void on_backFailButton_clicked();
    void on_nextFailButton_clicked();
    void on_nextResponseButton_2_clicked();
    void on_nextOtherButton_2_clicked();
    void on_backWCAButton_2_clicked();
    void on_backMCMUSVButton_2_clicked();
    void on_backResponseButton_2_clicked();
    void on_backOtherButton_2_clicked();
    void on_backChangeLogButton_2_clicked();
    void on_nextGeneralFailsafeButton_clicked();
    void on_backResponseFailsafeButton_clicked();
    void on_nextResponseFailsafeButton_clicked();
    void on_backChangelogFailsafeButton_clicked();
    void on_nextGeneralIntButton_clicked();
    void on_backChangeLogIntButton_clicked();
    void on_backOtherFailsafeButton_clicked();
    void on_nextOtherFailsafeButton_clicked();
    void on_nextGeneralSysButton_clicked();
    void on_nextNotesSysButton_clicked();
    void on_nextGeneralFRButton_clicked();
    void on_backChangelogFRButton_clicked();
    void on_backNotesSysButton_clicked();
    void on_backChangeLogSysButton_clicked();
    void on_cancelMCMUSVButton_2_clicked();
    void on_cancelFailButton_clicked();
    void on_cancelWCAButton_2_clicked();
    void on_cancelResponseButton_2_clicked();
    void on_cancelOtherButton_2_clicked();
    void on_cancelChangeLogButton_clicked();
    void on_cancelGeneralFailButton_clicked();
    void on_cancelResponseFailButton_clicked();
    void on_cancelOtherFailButton_clicked();
    void on_cancelIntButton_clicked();
    void on_cancelChangeLogFailsafeButton_clicked();
    void on_cancelChangeLogIntButton_clicked();
    void on_cancelGeneralSysButton_clicked();
    void on_cancelNotesSysButton_clicked();
    void on_cancelChangeLogSysButton_clicked();
    void on_addFaultCodeFailButton_clicked();
    void on_addFaultCodeIntButton_clicked();
    void on_deleteFaultCodeFailButton_clicked();
    void on_deleteFaultCodeIntButton_clicked();
    void addWCAToList(QListWidget *list, QString codeType);
    void deleteWCAFromList(QListWidget *list, QString codeType);

    void on_editMCMUSVButton_clicked();
    void on_editWCAButton_clicked();
    void on_editFailButton_clicked();
    void on_editResponseButton_clicked();
    void on_editOtherButton_clicked();
    void on_editGeneralFailButton_clicked();
    void on_editResponseFailButton_clicked();
    void on_editOtherFailButton_clicked();
    void on_editIntButton_clicked();
    void on_editGeneralSysButton_clicked();
    void on_editNotesSysButton_clicked();
    void createNewFaultPage();
    void createEditPage(bool readonly);
    void createViewPage(bool released);
    void hideEditFaults(bool visible);
    void ghostEditFaults(bool ghost);
    void ghostSaveFaults(bool ghost);
    void makeReadOnlyFields(bool readonly);
    void clearFields();

    //line edit listeners
    void on_groupLE_textEdited(const QString &text);
    void on_enumNameLE_textEdited(const QString &text);
    void on_faultCodeLE_textEdited(const QString &text);
    void on_changeeLE_textEdited(const QString &text);
    void on_faultGroupLE_textEdited(const QString &text);
    void on_failsafeCodeLE_textEdited(const QString &text);
    void on_interlockCodeLE_textEdited(const QString &text);
    QString queryField(QString fieldname);
    void updateHighlightLE(QString text, QString fieldname, QLineEdit *le);
    void on_failsafeIDLE_textEdited(const QString &text);
    void on_interlockIDLE_textEdited(const QString &text);
    void on_changeeIntLE_textEdited(const QString &text);
    void on_racLE_textEdited(const QString &text);
    void on_mitigatedRACLE_textEdited(const QString &text);
    void on_applicableSafetyPreceptLE_textEdited(const QString &text);
    void on_changeeFailLE_textEdited(const QString &text);
    void on_deviceLE_textEdited(const QString &text);
    void on_pcrNumLE_textEdited(const QString &text);
    void on_changeeSysLE_textEdited(const QString &text);
    void on_summarySysLE_textEdited(const QString &text);

    //plain text edit listeners
    void on_wcaMessagePT_textChanged();
    void on_wcaTriggerPT_textChanged();
    void on_failureImpactPT_textChanged();
    void on_failsafeSummaryPT_textChanged();
    void on_notesPT_textChanged();
    void updateHighlightPT(QString text, QString fieldname, QPlainTextEdit *pt);
    void on_failureNamePT_textChanged();
    void on_operationalStatePT_textChanged();
    void on_triggersPT_textChanged();
    //void on_triggeringFaultsPT_textChanged();
    void on_indicationMethodPT_textChanged();
    void on_functionalImpactPT_textChanged();
    void on_firstResponsePT_textChanged();
    void on_secondResponsePT_textChanged();
    void on_thirdResponsePT_textChanged();
    void on_interlockDescriptionPT_textChanged();
    //void on_associatedWcasPT_textChanged();
    void on_internalNotesIntPT_textChanged();
    void on_failsafeNotesPT_textChanged();
    void on_failsafeSummaryFailPT_textChanged();
    void on_internalNotesFailPT_textChanged();
    void on_onboardLCSPT_textChanged();
    void on_offboardLCSPT_textChanged();
    void on_lcsLRPT_textChanged();
    void on_correctiveMaintenanceActionsPT_textChanged();
    void on_wcaNotesPT_textChanged();
    void on_descriptionSysPT_textChanged();
    void on_workaroundsPT_textChanged();
    void on_internalNotesSysPT_textChanged();
    void on_externalNotesSysPT_textChanged();
    void on_wcaClearingPT_textChanged();
    void on_failsafeClearingPT_textChanged();

    //combo box listeners
    void updateHighlightCB(QString text, QString fieldname, QComboBox *cb);
    void on_fmStatusCB_currentIndexChanged(int);
    void on_wcaSeverityCB_currentIndexChanged(int);
    void on_fmStatus2CB_currentIndexChanged(int);
    void on_fmStatus3CB_currentIndexChanged(int);
    void on_componentsCB_currentIndexChanged(int);
    void on_configurationIntCB_currentIndexChanged(int);
    void on_configurationFailCB_currentIndexChanged(int);
    void on_configurationFaultCB_currentIndexChanged(int);
    void on_fmStatus4CB_currentIndexChanged(int);
    void on_configurationSysCB_currentIndexChanged(int);
    void on_operatorCB_currentIndexChanged(int);
    void on_faultGroupSysCB_currentIndexChanged(int);

    //save button listeners
    void on_saveMCMUSVButton_clicked();
    void on_saveWCAButton_clicked();
    void on_saveResponseButton_clicked();
    void on_saveOtherButton_clicked();
    void on_saveFailButton_clicked();
    void on_saveGeneralFailButton_clicked();
    void on_saveResponseFailButton_clicked();
    void on_saveOtherFailButton_clicked();
    void on_saveIntButton_clicked();
    void on_saveGeneralSysButton_clicked();
    void on_saveNotesSysButton_clicked();
    void showInfoPrompt();
    void saveNewFault();
    void saveEditFault();

    void on_actionNew_Release_triggered();
    void markAsLocked();
    void clearCombos();


    //other various slots
    void on_actionDelete_Release_triggered();
    void on_actionRename_triggered();
    QString deleteRelease(QString currentTable, bool isRename);
    void on_actionEdit_Release_triggered();
    void adjustReleaseMenu(bool readonly);
    void on_actionMark_as_Released_triggered();
    void on_actionfaultconstants_h_triggered();
    void on_actionfaultstringlib_h_triggered();
    void on_actionfaultstringlib_cpp_triggered();
    void on_actionFaults_triggered();
    void on_actionFailsafes_triggered();
    void on_actionInterlocks_triggered();
    void on_categoryComboBox_activated(const QString);
    void on_tableView_doubleClicked(const QModelIndex);
    void onFilterClose(QString field);
    bool eventFilter(QObject * obj, QEvent * event);
    void on_lineEdit_8_returnPressed();
    void onSaveViewTriggered();
    void onShowAllColumnsTriggered();
    void onDeleteViewsTriggered();
    void onDeleteViewTriggered();
    void load_view();
    void load_view(QString viewName);
    void onToggleColumnTriggered();
    QMap<QString, bool> *getCustomView();
    void onShowNoColumnsTriggered();
    void onShowCompactTriggered();
    void onShowExpandedTriggered();
    QTabWidget* getTabWidget();

    void fillFaultFields(std::vector<QString> data);
    void fillFailsafeFields(std::vector<QString> data);
    void fillInterlockFields(std::vector<QString> data);
    void fillSystemNotesFields(std::vector<QString> data);

    bool validateData(Exporter::datatype type, QString version, QString originalID);
    void startAddFault(QString version);
    void startEditFault(QString version);
    void startAddFailsafe(QString version);
    void startAddSystemNote(QString version);
    void startEditFailsafe(QString version);
    void startAddInterlock(QString version);
    void startEditInterlock(QString version);
    void startEditSystemNote(QString version);
    QString saveNewFailsafe();
    QString saveEditFailsafe();
    QString saveNewInterlock();
    QString saveEditInterlock();
    void saveNewSystemNote();
    void saveEditSystemNote();

    void showDeleteFailsafePrompt();
    void showDeleteInterlockPrompt();
    void showDeleteSystemNotePrompt();
    void selectDelete();

    void on_actionReset_Database_Info_triggered();

    void on_actionMH_triggered();

    void on_actionUISS_triggered();

    void on_actionOnboard_LCS_triggered();

    void on_actionMine_Warfare_Evaluator_triggered();

    void on_actionLaunch_Recovery_triggered();

    void on_actionOffboard_LCS_triggered();

    void on_actionSystem_Notes_triggered();

    void on_checkBox_stateChanged(int arg1);

    void on_actionOffboard_LCS_2_triggered();

    void on_actionMine_Warfare_Evaluator_2_triggered();

    void on_actionOnboard_LCS_2_triggered();

    void on_actionLaunch_and_Recovery_triggered();

    void on_actionFailsafes_2_triggered();

    void on_actionInterlocks_2_triggered();

    void on_actionSystem_Notes_2_triggered();

    void on_actionFailsafes_3_triggered();

    void on_actionInterlocks_3_triggered();

    void on_actionSystem_Notes_3_triggered();

private:
    Ui::MainWindow *ui;
    MyModel *myModel;
    void updateReleasesDropdown(QString selected);
    const QByteArray passwordhash = QByteArray("\xE0""c'\x99z) \xC8\xD8\xA0\x83>\xA9\xC0WT\x19\x92U\xEA");
    Exporter *exporter;
    QVector<QAction *> viewBar;
    const QString headerStyleSheet = "QHeaderView::section { background-color:lightgray }"
                               "QTableView {gridline-color: #0F0F0F; border: 4px solid #9f9f9f; border-radius: 7px;}";
};
#endif // MAINWINDOW_H
