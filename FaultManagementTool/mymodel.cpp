#include <QtSql>
#include <QMessageBox>
#include <QDebug>
#include <QAbstractItemModel>
#include <QTime>
#include <QCryptographicHash>
#include <algorithm>
#include <QAction>
#include <QInputDialog>
#include <QVector>
#include "mymodel.h"

// Initialize model by creating emtpy/initial values
MyModel::MyModel(QObject *parent) : QAbstractTableModel(parent)
{
    db = QSqlDatabase::addDatabase("QMYSQL");

    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Textron Systems", "Fault Manager");

    if (settings->contains("db/hostname")) {
        db.setHostName(settings->value("db/hostname").toString());
        //db.setHostName("10.10.31.212");
        db.setUserName(settings->value("db/username").toString());
        db.setPassword(settings->value("db/password").toString());
    }
    else {
        resetDbInfo((QWidget *) parent);
    }


    rows = 1;
    fullrows = 1;
    columns = 1;

    celldata = new QString*[rows];
    for (int i = 0; i < rows; ++i) {
        celldata[i] = new QString[columns];
    }

    fullcelldata = new QString*[rows];
    for (int i = 0; i < rows; ++i) {
        fullcelldata[i] = new QString[columns];
    }

    cellbackgrounds = new bool*[rows];
    for (int i = 0; i < rows; ++i) {
        cellbackgrounds[i] = new bool[columns]{false};
    }

    cellused = new bool*[rows];
    for (int i = 0; i < rows; ++i) {
        cellused[i] = new bool[columns]{true};
    }

    headerdata = new QString[columns];
    showingDeleted = false;
}

//returns true if there was a duplicate
bool MyModel::checkDuplicateID(QString version, QString id, QString field, QString original_id) {
    QSqlQuery q;

    q.exec(QStringLiteral("select %1 from %2 where %1 = '%3'").arg(field).arg(version).arg(id));

    if (q.next() == false || (q.value(0).toString() == original_id)) // get the field from database and compare to current text
        return false;
    else
        return true;
}

QString MyModel::getUser() {
    return db.userName();
}

int MyModel::rowCount(const QModelIndex & /*parent*/) const {
    return rows;
}

int MyModel::columnCount(const QModelIndex & /*parent*/) const {
    return columns;
}

QVariant MyModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole && celldata != nullptr) {
        return QString(celldata[index.row()][index.column()]);
    }
    else if (role == Qt::BackgroundRole) {
        // Consult backgrounds array for colors (updated by search feature)
        if (cellbackgrounds[index.row()][index.column()]) {
            return QColor(255, 247, 153);
        }
        else {
            if (!cellused[index.row()][index.column()]) {
                return QColor(240, 240, 240);
            }
            else {
                return QColor(255, 255, 255);
            }
        }
    }
    return QVariant();
}

QVariant MyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return headerdata[section];
    }
    return QVariant();
}

// Determine if the table exists in the database with the given name (version)
bool MyModel::released(const QString &version, QString dbname) {
    QSqlQuery q;
    try {
        db.close();
        db.setDatabaseName(dbname);
        db.open();

        QString query = "Select status from releases where version like \"" + version + "\"";

        q.exec(query);
    }  catch (QException e) {
        qInfo() << "Query Failed";
        return 1;
    }

    if (db.isOpenError()) {
        qInfo() << db.lastError();
        return 1;
    }

    q.next(); //returns false if no matches

    if (q.value(0).toString() == "working")
        return false;
    else return true;
}

bool MyModel::checkDuplicateRelease(const QString &version, QString dbname) {
    QSqlQuery q;
    try {
        db.close();
        db.setDatabaseName(dbname);
        db.open();

        QString query = "Select status from releases where version like \"" + version + "\"";

        q.exec(query);
    }  catch (QException e) {
        qInfo() << "Query Failed";
        return 1;
    }

    if (db.isOpenError()) {
        qInfo() << db.lastError();
        return 1;
    }

    return q.next(); //returns false if no matches
}

// General query used for the view page
std::vector<QString> MyModel::queryViewPage(QString query, int fields, QString dbname) {
    QSqlQuery q;
    try {
        db.close();
        db.setDatabaseName(dbname);
        db.open();

        q.exec(query);
    }  catch (QException e) {
        qInfo() << "Query Failed";
    }

    if (db.isOpenError()) {
        qInfo() << db.lastError();
    }


    std::vector<QString> data;

    while (q.next()) {
        for (int i = 0; i < fields; i++) {
            data.push_back(q.value(i).toString());
        }
    }

    return data;
}

// General query used for the delete fault feature
void MyModel::queryDeleteFault(QString query, QString version, QString dbname) {
    QSqlQuery q;
    try {
        db.close();
        db.setDatabaseName(dbname);
        db.open();

        q.exec(query);

        if (!version.contains("_failsafes") && !version.contains("_interlocks")) {
            //reorder ids if needed
            q.exec("set @n=0");
            q.exec(QStringLiteral("update %1 set id = @n := @n + 1 order by id").arg(version));
        }

        qInfo() << "Fault Deleted";
    }  catch (QException e) {
        qInfo() << "Query Failed";
    }

    if (db.isOpenError()) {
        qInfo() << db.lastError();
    }
}

//helper for general queries below
void MyModel::execFetch(QSqlQuery *q, QString query, QString dbname) {
    try {
        db.close();
        db.setDatabaseName(dbname);
        db.open();

        q->exec(query);
    }  catch (QException e) {
        qInfo() << "Exec query failed";
    }

    if (db.isOpenError()) {
        qInfo() << db.lastError();
    }
}

// used to get the ID of the last row in the table
int MyModel::queryLastID(QString query, QString dbname) {
    QSqlQuery q;

    execFetch(&q, query, dbname);

    q.next();
    return q.value(0).toString().toInt();
}

// General query used for getting a single field
QString MyModel::getField(QString query, QString dbname) {
    QSqlQuery q;

    execFetch(&q, query, dbname);

    q.next();
    return q.value(0).toString();
}




//general function used to execute a query
void MyModel::execQuery(QString query, QString dbname) {
    QSqlQuery q;

    execFetch(&q, query, dbname);
}

//delete later
int MyModel::queryRows(QString query, QString dbname) {
    QSqlQuery q;

    execFetch(&q, query, dbname);

    q.next();
    return q.value(0).toInt();
}

// Universal query that will query the entire given table and populate the application main table.
// This is the only place that the data in the table is directly changed without going through
// the refreshFilters method. This is because when loading new data, all filters and views are reset
// so the data can just be loaded straight in.
void MyModel::queryDatabase(QString query, QString category, QString dbname) {
    QSqlQuery q;
    int fields = columnMap.value("database_name").value(category).size();

    // Try to execute the query
    try {
        db.close();
        db.setDatabaseName(dbname);
        db.open();

        q.exec(query);
    }  catch (QException e) {
        qInfo() << "Query Failed";
        return;
    }

    if (db.isOpenError()) {
        qInfo() << "error found";
        qInfo() << db.lastError();

        adjustTableSize(1, columnMap.value("database_name").value(category).size());
        headerdata[0] = "No Database";

        fullrows = 1;
        fullcolumns = columns;
        QString ** temp = new QString*[1];
        temp[0] = new QString[columns];
        fullcelldata = temp;
    }
    else if (!q.next()) {
        qInfo() << "Printing empty table";

        adjustTableSize(1, columnMap.value("header_name").value(category).size());
        headerdata[0] = "No Data";

        fullrows = 1;
        fullcolumns = columns; // columns is set correctly above by adjustTableSize
        QString ** temp = new QString*[1];
        temp[0] = new QString[columns];
        temp[0][0] = "";
        fullcelldata = temp;

        emit headerDataChanged(Qt::Orientation::Horizontal, 0, 0);
        emit dataChanged(createIndex(0,0), createIndex(rows,columns), {Qt::DisplayRole});
        return;
    }
    else {
        // While the query has more data, continue to push it to back of temporary vector
        std::vector<std::vector<QString>> tempData;
        do {
            tempData.push_back({});
            for (int i = 0; i < fields; i++) {
                tempData.back().push_back(q.value(i).toString());
            }
        } while (q.next());

        adjustTableSize(static_cast<int>(tempData.size()), static_cast<int>(tempData.back().size()));


        // Fill header array with new header information
        for (int i = 0; i < fields; i++) {
            headerdata[i] = columnMap.value("header_name").value(category)[i];
        }

        // Update the full number of rows and declare new full cell data array
        fullrows = static_cast<int>(tempData.size());
        fullcolumns = static_cast<int>(tempData.at(0).size());
        QString **temp = new QString*[fullrows];
        for (int i = 0; i < fullrows; ++i) {
            temp[i] = new QString[columns];
        }
        fullcelldata = temp;

        // Loop through all data and assign proper values to both celldata (display) and fullcelldata (backend)
        for (size_t i = 0; i < tempData.size(); i++) {
            for (size_t j = 0; j < tempData.front().size(); j++) {
                celldata[i][j] = tempData.at(i).at(j);
                fullcelldata[i][j] = tempData.at(i).at(j);
            }
        }        
    }

    updateUsedCells();

    // Notify view that table data has changed
    QModelIndex topLeft = createIndex(0,0);
    QModelIndex bottomRight = createIndex(rows, columns);

    emit headerDataChanged(Qt::Orientation::Horizontal, 0, rows - 1);
    emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole});
    return;
}

// Close the database connection
void MyModel::closeConnection() {
    db.close();
    QSqlDatabase::removeDatabase("client");
}

// Filter the displayed table to match all views, filters, and search that are applied.
// Also updates the backgrounds array to highlight any cell with a global search match.
// This method is called several times in the course of populating the table (always called by refreshFilters()). Each time it is filtering out a different thing:
// The first call has the reset flag = true which restores the table to be unfiltered.
// Then, refreshFilters() calls this method once with each filter and once with field = "__all__" (for the search bar)
// The method is also written to use the supplied currentViewMap to determine if each column should be included.
// Only then is the data ready to display having only the selected columns, filtered by each filter, and filtered by the global search.
void MyModel::filterForPhrase(QString phrase, QString field, bool reset, QMap<QString, bool> *currentViewMap, QString currentCategory){
    // Count up how many columns are supposed to be included
    int newNumCols = 0;
    for (auto e : *currentViewMap) {
        newNumCols += e;
    }

    if (newNumCols == 0) { // No columns set to display in given currentViewMap - display nothing
        adjustTableSize(1, columnMap.value("database_name").value(currentCategory).size());
        headerdata[0] = "No Data";

        QString ** temp = new QString*[1];
        temp[0] = new QString[columns] {""};
        celldata = temp;

        QModelIndex topLeft = createIndex(0,0);
        QModelIndex bottomRight = createIndex(rows, columns);
        updateUsedCells();

        emit headerDataChanged(Qt::Orientation::Horizontal, 0, columns - 1);
        emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole});

        return;
    }

    // If attempting to reset filtered entries, return all data back to the main table view
    else if (reset) {
        // Change table size and reassign cell data (displayed) to match full cell data (backend)
        adjustTableSize(fullrows, newNumCols);
        int numDeleted = 0;
        for (int i = 0; i < fullrows; i++) {
            // Check if row is marked deleted (don't filter yet but just to see how many)
            if (fullcelldata[i][columnMap.value("database_name").value(currentCategory).indexOf("fm_status")].contains("deleted", Qt::CaseInsensitive))
                numDeleted++;

            // Copy proper data over to celldata & headerdata
            int k = 0;
            for (int j = 0; j < fullcolumns; j++) {
                if (currentViewMap->value(columnMap.value("header_name").value(currentCategory).at(j))) {
                    celldata[i][k] = fullcelldata[i][j];
                    headerdata[k] = columnMap.value("header_name").value(currentCategory).at(j);
                    k++;
                }
            }
        }

        // If application is set to not show deleted and at least one deleted entry is present, resize table and refill with the correct rows
        if (!showingDeleted && numDeleted > 0) {
            adjustTableSize(fullrows - numDeleted, newNumCols);
            // index to keep track of the current row to fill in celldata (may be different than i here)
            int l = 0;
            for (int i = 0; i < fullrows; i++) {
                if (fullcelldata[i][columnMap.value("database_name").value(currentCategory).indexOf("fm_status")].contains("deleted", Qt::CaseInsensitive))
                    continue;

                // index to keep track of the current column within the current row to fill (may be different than j here)
                int k = 0;
                for (int j = 0; j < fullcolumns; j++) {
                    if (currentViewMap->value(columnMap.value("header_name").value(currentCategory).at(j))) {
                        celldata[l][k] = fullcelldata[i][j];
                        k++;
                    }
                }
                l++;
            }
        }

        QModelIndex topLeft = createIndex(0,0);
        QModelIndex bottomRight = createIndex(rows, columns);
        updateUsedCells();

        emit headerDataChanged(Qt::Orientation::Horizontal, 0, columns - 1);
        emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole});
        return;
    }
    else if (phrase == NULL || phrase == "" || field == NULL || field == "") { // Empty phrase/field so not going to be filtering out
        // Just reset the yellow highlighted backgrounds to be empty
        twoDimentionDelete(cellbackgrounds, rows);
        bool **tempbackgrounds = new bool*[rows];
        for (int i = 0; i < rows; i++) {
            tempbackgrounds[i] = new bool[columns]{false};

        }
        cellbackgrounds = tempbackgrounds;
        return;
    }

    // Loop through all rows and record pointers to rows containing phrase
    std::vector<QString *> filterHits;
    if (field == "__all__") {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                if(celldata[i][j].toLower().contains(phrase.toLower())) {
                    QString * row = new QString[columns];
                    std::copy(celldata[i], celldata[i] + columns, row);
                    filterHits.push_back(row);
                    break;
                }
            }
        }
    }
    else { // Loop through all rows and record pointers to rows where the given field contains the phrase
        auto itr = std::find(headerdata, headerdata + columns, field);


        if (itr != headerdata + columns) { // column is currently being displayed
            int columnIndex = std::distance(headerdata, itr);
            for (int i = 0; i < rows; i++) {

                if(celldata[i][columnIndex].toLower().contains(phrase.toLower())) {
                    QString * row = new QString[columns];
                    std::copy(celldata[i], celldata[i] + columns, row);
                    filterHits.push_back(row);
                }

            }
        }
        else { // column is not currently being displayed so not going to be filtering anything out
            return;
        }
    }


    adjustTableSize(static_cast<int>(filterHits.size()), columns);

    // Fill new cell backgrounds array with "true" values corresponding to cells with a match
    // (specifically a match for the global search not any of the filters).
    // Size of cell backgrounds array is the same as the cell data array because they are both
    // the displayed data only. The full cell data array continues to hold all (unfiltered) data.
    if (field == "__all__") {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                if(filterHits.at(i)[j].toLower().contains(phrase.toLower())) {
                    cellbackgrounds[i][j] = true;
                }
            }
        }
    }

    // Recreate cell data array with rows out of the filtered hits vector
    for (int i = 0; i < static_cast<int>(filterHits.size()); i++) {
        std::copy(filterHits.at(i), filterHits.at(i) + columns, celldata[i]);
    }

    // Delete new memory used in copying the filtered hits
    for (QString * row : filterHits) {
        delete [] row;
    }

    QModelIndex topLeft = createIndex(0,0);
    QModelIndex bottomRight = createIndex(rows, columns);
    updateUsedCells();

    emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole});

    return;
}

// Insert rows into the table
bool MyModel::insertRows(int row, int count, const QModelIndex &parent) {
    if (count == 0)
        return true;

    beginInsertRows(parent, row, row + count - 1);

    redeclareCellArrays(rows + count, columns);

    rows += count;

    endInsertRows();
    return true;
}

// Insert columns into the table
bool MyModel::insertColumns(int column, int count, const QModelIndex &parent){
    if (count == 0)
        return true;

    QString * tempheaderdata = new QString[columns + count];
    std::copy(headerdata, headerdata + columns, tempheaderdata);
    delete [] headerdata;
    headerdata = tempheaderdata;

    beginInsertColumns(parent, column, column + count - 1);

    redeclareCellArrays(rows, columns + count);

    columns += count;

    endInsertColumns();
    return true;
}

// Remove rows from the table
bool MyModel::removeRows(int row, int count, const QModelIndex &parent){
    if (count == 0)
        return true;

    beginRemoveRows(parent, row, row + count - 1);

    redeclareCellArrays(rows - count, columns);

    rows -= count;

    endRemoveRows();
    return true;
}

// Remove columns from the table
bool MyModel::removeColumns(int column, int count, const QModelIndex &parent){
    if (count == 0)
        return true;

    QString * tempheaderdata = new QString[columns - count];
    std::copy(headerdata, headerdata + columns - count, tempheaderdata);
    delete [] headerdata;
    headerdata = tempheaderdata;

    beginRemoveColumns(parent, column, column + count - 1);

    redeclareCellArrays(rows, columns - count);

    columns -= count;

    endRemoveColumns();
    return true;
}

// Redeclare the arrays that hold cell data/characteristics to be empty
// Use the delete function to prevent mem leaks and initialize to default values
void MyModel::redeclareCellArrays(int numrows, int numcols) {
    twoDimentionDelete(cellbackgrounds, rows);
    twoDimentionDelete(cellused, rows);
    twoDimentionDelete(celldata, rows);

    bool **tempbackgrounds = new bool*[numrows];
    bool **tempcellused = new bool*[numrows];
    QString **tempcelldata = new QString*[numrows];
    for (int i = 0; i < numrows; i++) {
        tempbackgrounds[i] = new bool[numcols]{false};
        tempcellused[i] = new bool[numcols]{true};
        tempcelldata[i] = new QString[numcols];
    }
    cellbackgrounds = tempbackgrounds;
    cellused = tempcellused;
    celldata = tempcelldata;
}

// General resize function that will delegate to the proper functions for resizing.
// Concerning resizing, this should be the only function called by other parts of the program.
bool MyModel::adjustTableSize(int newRows, int newColumns) {
    if (newRows > rows) {
        insertRows(rows, newRows - rows);
    }
    else if (newRows < rows) {
        removeRows(0, rows - newRows);
    }

    if (newColumns > columns) {
        insertColumns(columns, newColumns - columns);
    }
    else if (newColumns < columns) {
        removeColumns(0, columns - newColumns);
    }

    return true;
}

// Specific query to get a list of table names currently stored in the database
std::vector<QString> MyModel::fetchCurrentReleases(QString dbname) {
    QSqlQuery q;
        try {
            db.close();
            db.setDatabaseName(dbname);
            db.open();

            q.exec("select version from releases");
        }  catch (QException e) {
            qInfo() << "Query Failed";
            return std::vector<QString>();
        }

        if (db.isOpenError()) {
            qInfo() << db.lastError();
            return std::vector<QString>();
        }

        std::vector<QString> tempData;
        while (q.next()) {
                tempData.push_back(q.value(0).toString());
        }

        return tempData;
}

// Create a new release based on a previous release and show the new release on the home page
void MyModel::createNewRelease(QString newReleaseName, QString oldReleaseName, bool isRename) {
    std::vector<std::vector<std::vector<QString>>> newData;
    QSqlQuery q;
    QString originalParent;

    convertToUnderscores(&newReleaseName);
    convertToUnderscores(&oldReleaseName);

    if (isRename) {
        q.exec("select parent from releases where version = '" + oldReleaseName + "'");
        q.next();
        originalParent = q.value(0).toString();
    }

    // Create new tables with correct names and number of columns in db
    for (QString type : columnMap.value("database_name").keys()) {
        QString query = "create table " + newReleaseName;

        if (type != "WCAs") {
            query += "_" + returnToUnderscores(type);
        }

        query += " (" + columnMap.value("database_name").value(type).at(0) + " " + columnMap.value("database_type").value(type).at(0);

        for (int i = 1; i < columnMap.value("database_name").value(type).size(); i++) {
            query += "," + columnMap.value("database_name").value(type).at(i) + " " + columnMap.value("database_type").value(type).at(i);
        }

        query += ")";
        q.exec(query);
    }

    // Query for old data and populate vector
    int i = 0;
    for (QString type : columnMap.value("database_name").keys()) {
        QString query = "select * from " + oldReleaseName;

        if (type != "WCAs") {
            query += "_" + returnToUnderscores(type);
        }

        q.exec(query);

        // Replace all "'" characters with tabs so sql doesn't get confused
        newData.push_back({});
        while(q.next()) {
            newData.at(i).push_back({});
            for (int j = 0; j < columnMap.value("database_name").value(type).size(); j++) {
                QString entry = q.value(j).toString();
                std::replace(entry.begin(), entry.end(), '\'', '\t');
                newData.at(i).back().push_back(entry);
            }
        }

        i++;
    }

    // populate new table in db with old data
    i = 0;
    for (QString type : columnMap.value("database_name").keys()) {
        for (int j = 0; j < static_cast<int>(newData.at(i).size()); j++) {
            QString query = "insert into " + newReleaseName;

            if (type != "WCAs") {
                query += "_" + returnToUnderscores(type);
            }

            query += " values ('" + newData.at(i).at(j).at(0);

            for (int k = 1; k < static_cast<int>(newData.at(i).at(j).size()); k++) {
                // Don't copy over the changelog
                if (type == "WCAs" && k == columnMap.value("database_name").value(type).indexOf("changelog") && !isRename) {
                    query += "','";
                }
                else {
                    query += "','" + newData.at(i).at(j).at(k);
                }
            }

            query += "')";
            q.exec(query);
        }
        i++;
    }

    // Switch all tab characters back to ' characters after being inserted into database
    for (QString type: columnMap.value("database_name").keys()) {
        QString query = "update " + newReleaseName;

        if (type != "WCAs") {
            query += "_" + returnToUnderscores(type);
        }

        query += " set " + columnMap.value("database_name").value(type).at(0) + " = replace(" + columnMap.value("database_name").value(type).at(0) + ", char(9), char(39))";

        for (int i = 1; i < columnMap.value("database_name").value(type).size(); i++) {
            query += "," + columnMap.value("database_name").value(type).at(i) + " = replace(" + columnMap.value("database_name").value(type).at(i) + ", char(9), char(39))";
        }
        q.exec(query);
    }

    // Create new entry in release table and set to working
    if (isRename) {
        q.exec("insert into releases (version, status, parent) values ('" + newReleaseName + "','working', '" + originalParent + "')");
    }
    else {
        q.exec("insert into releases (version, status, parent) values ('" + newReleaseName + "','working', '" + oldReleaseName + "')");
    }
}

// Look through the 'parent' column in the releases table and cascade the rename operation
void MyModel::updateParentColumn(QString newReleaseName, QString oldReleaseName) {
    convertToUnderscores(&newReleaseName);
    convertToUnderscores(&oldReleaseName);

    QSqlQuery q;
    QSqlQuery u;
    q.exec("select version from releases where parent = '" + oldReleaseName + "'");

    while(q.next()) {
        QString entryName = q.value(0).toString();

        if (newReleaseName == "NULL") {
            u.exec("update releases set parent = " + newReleaseName + " where version = '" + entryName + "'");
        }
        else {
            u.exec("update releases set parent = '" + newReleaseName + "' where version = '" + entryName + "'");
        }
    }
}

// Update the used cells array based on the contents of the table.
// This function is used in determining which cells to slightly gray out due to not containing any info.
void MyModel::updateUsedCells() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            cellused[i][j] = (celldata[i][j].size() != 0);
        }
    }
}

// Utility function to convert all '.'s in a string to '_'s
void MyModel::convertToUnderscores(QString * string) {
    std::replace(string->begin(), string->end(), '.', '_');
}

// Utility function to convert all '_'s in a string to '.'s
void MyModel::convertToDecimals(QString * string) {
    std::replace(string->begin(), string->end(), '_', '.');
}

QString MyModel::returnToUnderscores(QString string) {
    std::replace(string.begin(), string.end(), ' ', '_');
    return string.toLower();
}

QString MyModel::returnToSpaces(QString string) {
    std::replace(string.begin(), string.end(), '_', ' ');
    return toCamelCase(string);
}

// Delete a 2 dimentional array of strings without mem leaks
void MyModel::twoDimentionDelete(QString ** array, int numrows) {
    for (int i = 0; i < numrows; i++) {
        delete [] array[i];
    }
    delete [] array;
}

// Delete a 2 dimentional array of booleans without mem leaks
void MyModel::twoDimentionDelete(bool ** array, int numrows) {
    for (int i = 0; i < numrows; i++) {
        delete [] array[i];
    }
    delete [] array;
}

void MyModel::updateFaultConstantsTable(QString faultGroup, QString enumName, QString dbname) {
    QSqlQuery q;
    q.exec("select * from faultconstants where enum_name = \"" + enumName + "\" and class = \"" + faultGroup + "\"");

    // If the fault is already in the faultconstants table, don't do anything.
    // else, add a new row
    if (!q.next()) {
        execQuery("insert into faultconstants values (\"" + enumName + "\",\"" + faultGroup + "\")", dbname);
    }
}

// Refresh the display of data on the table taking into account all filtering.
// First, call filterForPhrase with the reset flag set to true to reset.
// Then call filterForPhrase for each filter currently applied.
// Then call filterForPhrase with the current search bar text.
// This is the only place the data in the table gets changed besides queryDatabase().
void MyModel::refreshFilters(QString searchBar, QMap<QString, bool> *currentViewMap, QString currentCategory) {
    filterForPhrase("", "", true, currentViewMap, currentCategory);

    for (QString field : filters.keys()) {
        filterForPhrase(filters.value(field), field, false, currentViewMap, currentCategory);
    }
    filterForPhrase(searchBar, "__all__", false, currentViewMap, currentCategory);
}

void MyModel::addFilter(QString field, QString value) {
    filters.insert(field, value);
}

void MyModel::removeFilter(QString field) {
    filters.remove(field);
}

// Update the data being displayed on the table with the current view name, current view map {field name, bool}, search bar text, and
// currently displayed category of data (fault, failsafe, or interlock)
void MyModel::updateColumnDisplay(QString currentView, QMap<QString, bool> *currentViewMap, QString searchBar, QString currentCategory) {
    // If currently displaying a view, use that as the currentViewMap
    if (currentView.size() > 0) {
        for (auto e : viewMap) {
            if (e.first->text() == currentView) {
                currentViewMap = &e.second;
                break;
            }
        }
    }

    refreshFilters(searchBar, currentViewMap, currentCategory);
    return;
}

void MyModel::resetDbInfo(QWidget *main) {
    QString newHostname;
    QString newUsername;
    QString newPassword;
    bool responded;

    while (true) {
        newHostname = QInputDialog::getText(main, "Set Database Hostname", "Enter the hostname of the database", QLineEdit::Normal, QString(), &responded, Qt::MSWindowsFixedSizeDialogHint);
        if (!responded)
            return;

        if (newHostname.isEmpty()) {
            QMessageBox msg;
            msg.setWindowTitle("Invalid Hostname");
            msg.setText("Hostname cannot be empty");
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setStandardButtons(QMessageBox::Cancel);
            msg.setDefaultButton(QMessageBox::Ok);

            if (msg.exec() == QMessageBox::Cancel)
                return;
        }
        else {
            break;
        }
    }

    while (true) {
        newUsername = QInputDialog::getText(main, "Set Database Username", "Enter the username of your database account (talk to software if you haven't been given one)", QLineEdit::Normal, QString(), &responded, Qt::MSWindowsFixedSizeDialogHint);
        if (!responded)
            return;

        if (newUsername.isEmpty()) {
            QMessageBox msg;
            msg.setWindowTitle("Invalid Username");
            msg.setText("Username cannot be empty");
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setStandardButtons(QMessageBox::Cancel);
            msg.setDefaultButton(QMessageBox::Ok);

            if (msg.exec() == QMessageBox::Cancel)
                return;
        }
        else {
            break;
        }
    }

    while (true) {
        newPassword = QInputDialog::getText(main, "Set Database User Password", "Enter the password of your database account (talk to software if you haven't been given one)", QLineEdit::Password, QString(), &responded, Qt::MSWindowsFixedSizeDialogHint);
        if (!responded)
            return;

        if (newPassword.isEmpty()) {
            QMessageBox msg;
            msg.setWindowTitle("Invalid Password");
            msg.setText("Password cannot be empty");
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setStandardButtons(QMessageBox::Cancel);
            msg.setDefaultButton(QMessageBox::Ok);

            if (msg.exec() == QMessageBox::Cancel)
                return;
        }
        else {
            break;
        }
    }

    // Only change values after user has had a chance to cancel out of both prompts
    settings->setValue("db/hostname", newHostname);
    settings->setValue("db/username", newUsername);
    settings->setValue("db/password", newPassword);

    db.setHostName(newHostname);
    db.setUserName(newUsername);
    db.setPassword(newPassword);
}

QString MyModel::toCamelCase(const QString& s)
{
    QStringList parts = s.split(' ', QString::SkipEmptyParts);
    for (int i = 0; i < parts.size(); ++i)
        parts[i].replace(0, 1, parts[i][0].toUpper());

    return parts.join(" ");
}

void MyModel::setShowingDeleted(bool value) {
    showingDeleted = value;
}

//check if a fault with fault code passed in exists, returns false if nonexistent
bool MyModel::checkFaultCode(QString code, QString version, QString dbname) {
    QSqlQuery q;

    //check existence
    q.exec(QStringLiteral("Select fm_status from %1 where fault_code = '%2'").arg(version).arg(code));

    //if fault exists, make sure its not marked deleted
    if (q.next() == false || (q.value(0).toString() == "DELETED"))
        return false;
    else return true;
}
