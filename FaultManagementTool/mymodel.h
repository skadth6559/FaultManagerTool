#ifndef MYMODEL_H
#define MYMODEL_H

#include <QAbstractTableModel>
#include <QSettings>
#include <QString>
#include <QtSql>

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    //Ui::MainWindow *ui; //remove later
    explicit MyModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool released(const QString &version, QString dbname);
    void queryDatabase(QString query, QString category, QString dbname);
    void filterForPhrase(QString phrase, QString field, bool reset,QMap<QString, bool> *currentViewMap, QString currentCategory);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    void closeConnection();
    std::vector<QString> queryViewPage(QString query, int fields, QString dbname);
    void queryDeleteFault(QString query, QString version, QString dbname);
    QString getField(QString query, QString dbname);
    int queryRows(QString query, QString dbname);
    void execQuery(QString query, QString dbname);
    int queryLastID(QString query, QString dbname);
    bool checkFaultCode(QString code, QString version, QString dbname);

    void execFetch(QSqlQuery *q, QString query, QString dbname);

    std::vector<QString> fetchCurrentReleases(QString dbname);

    void createNewRelease(QString newReleaseName, QString oldReleaseName, bool isRename);
    static void convertToUnderscores(QString * string);
    static void convertToDecimals(QString * string);
    static QString returnToUnderscores(QString string);
    static QString returnToSpaces(QString string);
    void updateParentColumn(QString newReleaseName, QString oldReleaseName);
    void updateFaultConstantsTable(QString faultGroup, QString enumName, QString dbname);
    void refreshFilters(QString searchBar, QMap<QString, bool> *currentViewMap, QString currentCategory);
    void addFilter(QString field, QString value);
    void removeFilter(QString field);
    void updateColumnDisplay(QString currentView, QMap<QString, bool> *currentViewMap, QString searchBar, QString currentCategory);

    // Holds the displayed header names, the database names, and the database types of all the columns
    // in the tables. Each category is further divided into WCAs, Failsafes, and Interlocks to differentiate
    // between columns for the different types of data that can be displayed.
    // All columns are listed in the order in which they appear in the database (for easy insert queries)
    const QMap<QString, QMap<QString, QStringList>> columnMap {{"header_name", {{"WCAs", QStringList{"ID", "FM STATUS", "SW Group", "Enum Name", "Fault Code", "Changee", "Fault Group", "Configuration",
                                                                             "WCA Severity", "WCA Message", "WCA Trigger", "WCA Clearing", "Failure Impact", "Failsafe Code", "Interlock Code", "Failsafe Summary",
                                                                             "Onboard LCS", "Offboard LCS", "LCS L&R", "Corrective Maintenance Actions", "WCA Notes", "Device", "Internal Notes", "Changelog"}},
                                                                            {"Failsafes", QStringList{"FM STATUS", "Failsafe Code", "Configuration", "Failsafe Name", "Operational State(s)", "Trigger(s)", "Associated Fault Code(s)", "Failsafe Clearing", "Indication Method", "Failsafe Notes",
                                                                             "Effect + Functional Impact", "RAC", "First Automated Response", "Second Automated Response", "Third Automated Response", "Failsafe Summary", "Applicable Safety Precept", "Mitigated RAC", "Internal Notes", "Changee", "Changelog"}},
                                                                            {"Interlocks", QStringList{"FM STATUS", "Interlock Code", "Configuration", "Components", "Interlock Description", "Associated Fault Code(s)", "Internal Notes", "Changee", "Changelog"}},
                                                                            {"System Notes", QStringList{"ID", "Status", "Configuration", "Operator", "Summary", "Description", "Workarounds + Actions", "Notes [External]", "Internal Notes", "Fault Group", "PCR Number", "Changee", "Changelog"}}}},
                                                         {"database_name", {{"WCAs", QStringList{"id", "fm_status", "group_name", "enum_name", "fault_code", "changee",
                                                                             "fault_group", "configuration", "wca_severity", "wca_message", "wca_trigger", "wca_clearing", "failure_impact", "failsafe_code", "interlock_code", "failsafe_summary", "onboard_lcs", "offboard_lcs", "lcs_lr",
                                                                             "corrective_maintenance_actions", "wca_notes", "device", "notes", "changelog"}},
                                                                            {"Failsafes", QStringList{"fm_status", "failsafe_id", "configuration", "failure_name", "operational_state", "triggers", "triggering_faults", "failsafe_clearing",
                                                                             "indication_method", "failsafe_notes", "functional_impact", "rac", "first_auto_response", "second_auto_response", "third_auto_response", "failsafe_summary", "applicable_safety_precept", "mitigated_rac", "internal_notes", "changee", "changelog"}},
                                                                            {"Interlocks", QStringList{"fm_status", "interlock_id", "configuration", "components", "interlock_description", "associated_wcas", "internal_notes", "changee", "changelog"}},
                                                                            {"System Notes", QStringList{"id", "fm_status", "configuration", "operator", "summary", "description", "workarounds", "external_notes", "internal_notes", "fault_groups", "pcr_num", "changee", "changelog"}}}},
                                                         {"database_type", {{"WCAs", QStringList{"int auto_increment, primary key(id)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)",
                                                                                                 "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)",
                                                                                                 "varchar(1000)", "varchar(1000)", "varchar(1000)", "varchar(1000)", "varchar(200)", "varchar(200)", "varchar(1000)", "varchar(1000)"}},
                                                                            {"Failsafes", QStringList{"varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(1000)", "varchar(1000)", "varchar(200)", "varchar(1000)",
                                                                                                      "varchar(1000)", "varchar(1000)", "varchar(1000)", "varchar(200)", "varchar(200)", "varchar(1000)", "varchar(200)", "varchar(1000)"}},
                                                                            {"Interlocks", QStringList{"varchar(200)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(1000)", "varchar(1000)", "varchar(1000)", "varchar(200)", "varchar(1000)"}},
                                                                            {"System Notes", QStringList{"int auto_increment, primary key(id)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(1000)", "varchar(1000)", "varchar(1000)", "varchar(1000)", "varchar(1000)", "varchar(200)", "varchar(200)", "varchar(200)", "varchar(1000)"}}}}};
    const QString defaultHostname = "localhost";
    const QString defaultUsername = "root";
    const QString defaultPassword = "test12";
    QSettings * settings;
    std::map<QAction *, QMap<QString, bool>> viewMap;
    QMap<QString, QString> filters;
    void resetDbInfo(QWidget *main);

    QString getUser();

    bool checkDuplicateID(QString version, QString id, QString field, QString original_id);
    void setShowingDeleted(bool value);
    bool checkDuplicateRelease(const QString &version, QString dbname);

private:
    QString ** fullcelldata;
    QString ** celldata;
    bool ** cellbackgrounds;
    bool ** cellused;
    QString * headerdata;
    int fullrows;
    int fullcolumns;
    int rows;
    int columns;
    QSqlDatabase db;
    bool showingDeleted;
    bool adjustTableSize(int newRows, int newColumns);
    void updateUsedCells();
    void twoDimentionDelete(QString ** array, int numrows);
    void twoDimentionDelete(bool ** array, int numrows);
    void redeclareCellArrays(int numrows, int numcols);
    QString currentView;
    static QString toCamelCase(const QString& s);

};

#endif // MYMODEL_H

