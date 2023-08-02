#include <QFileDialog>
#include <string>
#include "exporter.h"



// Comparator to order faults for export.
// Orders alphabetically by group name first, then by enum_name
struct compareGroupNames {
    inline bool operator() (const QVector<QString> first, const QVector<QString> second) {
        if (first.at(0) == second.at(0)) {
            return (first.at(1) < second.at(1));
        }
        return (first.at(0) < second.at(0));
    }
};

Exporter::Exporter(MyModel *model){
    currentIndent = 0;
    myModel = model;
}

// Exports all data from the table being currently displayed
// This fucntion does not recognize any filters or views; The entire table will be exported
// no matter what filters or views are currently applied to the table when this is called
void Exporter::exportToCSV(QString version, QWidget * parent, QString category) {
    MyModel::convertToUnderscores(&version);
    std::replace(category.begin(), category.end(), ' ', '_'); //cannot have spaces in filename
    QString filename = QFileDialog::getSaveFileName(parent, "Save Fault CSV", version + "_" + category, "CSV Files (*.csv)");
    std::replace(category.begin(), category.end(), '_', ' '); //remove underscores after getting filename
    QString outfile;

    if (!filename.isEmpty() && !filename.isNull()) {
        QSqlQuery q;
        if (category != "WCAs") {
            version += "_" + myModel->returnToUnderscores(category);
        }
        q.exec("select * from " + version);

        // Populate first row of output csv with header names
        for (int i = 0; i < myModel->columnMap.value("header_name").value(category).size(); i++) {
            outfile += "\"" + myModel->columnMap.value("header_name").value(category).at(i) + "\",";
        }
        outfile += "\n";

        // Populate each row of the csv with the data from each cell in the query
        while (q.next()) {
            for (int i = 0; i < myModel->columnMap.value("database_name").value(category).size(); i++) {
                outfile += "\"" + q.value(i).toString() + "\",";
            }
            outfile += "\r\n";
        }

        QFile f(filename);
        f.open(QIODevice::WriteOnly);
        f.write(outfile.toUtf8());
        f.close();
    }
}

// Export the faultconstants.h file to the location chosen by the user.
// Export will function as follows (same as other source files):
// The program will attempt to export every fault listed in the faultconstants
// table in the database. Before it includes a fault, though, it checks
// the fault table currently being viewed and makes sure the given fault is
// present and marked with an "IN_USE" or "ACTIVE" fm_status. Also, if the value of "payload" passed in
// is positive, it will only include faults of the payload corresponding to that value.
// If all of those conditions are met, the fault is included in the export
void Exporter::exportFaultConstants(QString version, QWidget * parent, int payload) {
    QString filename = QFileDialog::getSaveFileName(parent, "Save Fault Constants Header File", "FaultConstants.h", "C++ Header Files (*.h)");
    currentIndent = 0;

    if (!filename.isEmpty() && !filename.isNull()) { // If the user did not cancel out of the file save dialog
        QString outfile;
        MyModel::convertToUnderscores(&version);

        addLine("#ifndef FAULT_CONSTANTS_H", &outfile);
        addLine("#define FAULT_CONSTANTS_H\n", &outfile);
        addLine("#include <QtGlobal>\n", &outfile);
        addLine("class FConst", &outfile);
        addLine("{", &outfile);
        addLine("public:\n", &outfile);
        increaseIndent();
        addLine("static const quint8 EMPTY_FAULT_ID = 0;", &outfile);
        addLine("static const quint8 INACTIVE = 0;", &outfile);
        addLine("static const quint8 ACTIVE   = 1;\n", &outfile);

        QSqlQuery q;
        QVector<QVector<QString>> enumData;
        q.exec("select * from faultconstants");

        // For every fault in the faultconstants table, check the fm_status
        // in the correct release table
        while (q.next()) {
            QString enumName = q.value(0).toString();
            QString enumGroup = q.value(1).toString();

            QSqlQuery u;

            u.exec("select fm_status, configuration from " + version + " where group_name = '" + enumGroup + "' and enum_name = '" + enumName + "'");

            if (u.next()) { // If fault can be found in the main table
                if (payload >= 0) { // If the value of payload passed in is positive - determines if it will filter for a specific payload
                    if ((u.value(0).toString().toLower() == "in use" || u.value(0).toString().toLower() == "active") && (u.value(1).toString().contains(payloads[payload], Qt::CaseInsensitive) || u.value(1).toString().contains("Common Craft", Qt::CaseInsensitive))) {
                        enumData.append(QVector<QString>{enumGroup, enumName});
                    }
                }
                else { // payload < 0, Ignore the payload assigned to the faults
                    if (u.value(0).toString().toLower() == "in use" || u.value(0).toString().toLower() == "active") {
                        enumData.append(QVector<QString>{enumGroup, enumName});
                    }
                }
            }
        }

        // Sort by group name first, so the export can add faults in correct groups
        std::sort(enumData.begin(), enumData.end(), compareGroupNames());

        QString currentGroupName;

        // Add all faults to exported file
        for (QVector<QString> fault : enumData) {
            if (currentGroupName != fault.at(0)) { // If current fault should start a new class in the header file
                if (currentGroupName != "") { // If current fault is not the first in the file
                    decreaseIndent();
                    addLine("};", &outfile);
                    decreaseIndent();
                    addLine("};\n", &outfile);
                }

                currentGroupName = fault.at(0);
                addLine("class " + currentGroupName, &outfile);
                addLine("{", &outfile);
                increaseIndent();
                addLine("public:", &outfile);
                addLine("enum Faults : quint16", &outfile);
                addLine("{", &outfile);
                increaseIndent();
                addLine("NA = EMPTY_FAULT_ID,", &outfile);
                if (currentGroupName == "ControlSoftware") {
                    addLine("CS_FaultCount, // not a fault, just used as an index in VSMGUI", &outfile);
                }
            }
            addLine(fault.at(1) + ",", &outfile);
        }

        decreaseIndent();
        addLine("};", &outfile);
        decreaseIndent();
        addLine("};\n", &outfile);

        /****** Begin placeholder code *******/

        // This code is for the "Autopilot" and "STRAIN" classes
        // to be present in the file. Neither of the classes have any
        // active faults so they normally wouldn't be exported. If any faults
        // get added in the future that are in groups "Autopilot" or "STRAIN",
        // part of this will need to be removed

        addLine("class Autopilot", &outfile);
        addLine("{", &outfile);
        addLine("public:", &outfile);
        increaseIndent();
        addLine("enum Faults : quint16", &outfile);
        addLine("{", &outfile);
        increaseIndent();
        addLine("NA = EMPTY_FAULT_ID", &outfile);
        decreaseIndent();
        addLine("};", &outfile);
        decreaseIndent();
        addLine("};\n", &outfile);

        addLine("class STRAIN", &outfile);
        addLine("{", &outfile);
        addLine("public:", &outfile);
        increaseIndent();
        addLine("enum Faults : quint16", &outfile);
        addLine("{", &outfile);
        increaseIndent();
        addLine("NA = EMPTY_FAULT_ID", &outfile);
        decreaseIndent();
        addLine("};", &outfile);
        decreaseIndent();
        addLine("};\n", &outfile);

        /****** End placeholder code *******/

        decreaseIndent();
        addLine("private:", &outfile);
        increaseIndent();
        addLine("FConst();", &outfile);
        decreaseIndent();
        addLine("};\n", &outfile);
        addLine("#endif // FAULT_CONSTANTS_H", &outfile);

        QFile f(filename);
        f.open(QIODevice::WriteOnly);
        f.write(outfile.toUtf8());
        f.close();
    }
}

// Export the faultstringlib.h file to the location chosen by the user.
// Export will function as follows (same as other source files):
// The program will attempt to export every fault listed in the faultconstants
// table in the database. Before it includes a fault, though, it checks
// the fault table currently being viewed and makes sure the given fault is
// present and marked with an "IN_USE" or "ACTIVE" fm_status. Also, if the value of "payload" passed in
// is positive, it will only include faults of the payload corresponding to that value.
// If all of those conditions are met, the fault is included in the export
void Exporter::exportFaultStringLibh(QString version, QWidget * parent, int payload) {
    QString filename = QFileDialog::getSaveFileName(parent, "Save Fault Lib Header File", "faultstringlib.h", "C++ Header Files (*.h)");
    currentIndent = 0;

    if (!filename.isEmpty() && !filename.isNull()) { // If the user did not cancel out of the file save dialog
        QString outfile;
        MyModel::convertToUnderscores(&version);

        // Add boilerplate code. If any more groups of faults are added in the future,
        // this will need to be updated
        addLine("#ifndef FAULTSTRINGLIB_H", &outfile);
        addLine("#define FAULTSTRINGLIB_H\n", &outfile);
        addLine("#include <QObject>", &outfile);
        addLine("#include <QMap>", &outfile);
        addLine("#include \"FaultConstants.h\"\n", &outfile);
        addLine("/*", &outfile);
        addLine("Group ID Lookup", &outfile);
        addLine("0  : Autopilot", &outfile);
        addLine("1  : Central", &outfile);
        addLine("2  : MVCS", &outfile);
        addLine("3  : Position", &outfile);
        addLine("4  : Power", &outfile);
        addLine("5  : Propulsion", &outfile);
        addLine("6  : Secondary", &outfile);
        addLine("7  : DDC", &outfile);
        addLine("8  : SPC", &outfile);
        addLine("9  : WC", &outfile);
        addLine("10 : SCC", &outfile);
        addLine("11 : DSS", &outfile);
        addLine("12 : Payload", &outfile);
        addLine("*/\n", &outfile);
        addLine("enum WcaLevel {", &outfile);
        increaseIndent();
        addLine("ADVISORY, CAUTION, WARNING, UNKNOWN, UNUSED", &outfile);
        decreaseIndent();
        addLine("};\n", &outfile);
        addLine("struct wcaMsg{", &outfile);
        increaseIndent();
        addLine("int uniqueFaltId;", &outfile);
        addLine("WcaLevel lvl;", &outfile);
        addLine("QString message;", &outfile);
        addLine("QString trigger;", &outfile);
        addLine("QString subSystem;", &outfile);
        decreaseIndent();
        addLine("};\n", &outfile);
        addLine("class faultStringLib : public QObject", &outfile);
        addLine("{", &outfile);
        increaseIndent();
        addLine("Q_OBJECT", &outfile);
        decreaseIndent();
        addLine("public:", &outfile);
        increaseIndent();
        addLine("explicit faultStringLib(QObject *parent = 0);", &outfile);
        addLine("~faultStringLib();", &outfile);
        addLine("QString getWcaMsg(int groupId, int faultId);", &outfile);
        addLine("QString getTrigger(int groupId, int faultId);", &outfile);
        addLine("WcaLevel getLevel(int groupId, int faultId);", &outfile);
        addLine("QString getSubSys(int groupId, int faultId);", &outfile);
        addLine("static const int CONTROL_STATION_GROUP = -1;\n", &outfile);
        decreaseIndent();
        addLine("signals:\n", &outfile);
        addLine("public slots:\n", &outfile);
        addLine("private:", &outfile);
        increaseIndent();
        addLine("QMap<int, QMap<int, wcaMsg>> wcaIdMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaAutopilotMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaCentralMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaMVCSMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaPositionMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaPropulsionMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaPowerMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaSecondaryMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaDDCMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaSPCMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaWCMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaSACMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaStrainMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaDSSMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaVPLCMap;", &outfile);
        addLine("QMap<int, wcaMsg> wcaControlSoftwareMap;", &outfile);
        addLine("QMap<int wcaMsg> getGroupMap(int group)", &outfile);
        decreaseIndent();
        addLine("};\n", &outfile);

        QSqlQuery q;
        QVector<QVector<QString>> enumData;
        q.exec("select * from faultconstants");

        // For every fault in the faultconstants table, check the fm_status from
        // the correct release table
        while (q.next()) {
            QString enumName = q.value(0).toString();
            QString enumGroup = q.value(1).toString();

            QSqlQuery u;

            u.exec("select fm_status, wca_severity, wca_message, wca_trigger, fault_group, configuration from " + version + " where group_name = '" + enumGroup + "' and enum_name = '" + enumName + "'");

            if (u.next()) {// If fault can be found in the main table
                if (payload >= 0) {// If the value of payload passed in is positive - determines if it will filter for a specific payload
                    if ((u.value(0).toString().toLower() == "in use" || u.value(0).toString().toLower() == "active") && (u.value(5).toString().contains(payloads[payload], Qt::CaseInsensitive) || u.value(5).toString().contains("Common Craft", Qt::CaseInsensitive))) {
                        enumData.append(QVector<QString>{enumGroup, enumName, u.value(1).toString(), u.value(2).toString(), u.value(3).toString(), u.value(4).toString()});
                    }
                }
                else { // payload < 0, Ignore the payload assigned to the faults
                    if (u.value(0).toString().toLower() == "in use" || u.value(0).toString().toLower() == "active") {
                        enumData.append(QVector<QString>{enumGroup, enumName, u.value(1).toString(), u.value(2).toString(), u.value(3).toString(), u.value(4).toString()});
                    }
                }
            }
        }

        // Sort faults by group name first so they appear next to each other in the export
        std::sort(enumData.begin(), enumData.end(), compareGroupNames());

        for (QVector<QString> fault : enumData) {
            QString cleanedMessage = fault.at(3);
            QString cleanedTrigger = fault.at(4);
            cleanedMessage.remove(QChar('"'), Qt::CaseSensitivity::CaseInsensitive);
            cleanedTrigger.remove(QChar('"'), Qt::CaseSensitivity::CaseInsensitive);


            addLine("const wcaMsg " + fault.at(0) + "_" + fault.at(1) + " = {FConst::" +
                    fault.at(0) + "::" + fault.at(1) + ", WcaLevel::" + fault.at(2) + ", \"" +
                    cleanedMessage + "\",\"" + cleanedTrigger + "\",\"" + fault.at(5) + "\"};", &outfile);
        }

        addLine("\n#endif // FAULTSTRINGLIB_H", &outfile);

        QFile f(filename);
        f.open(QIODevice::WriteOnly);
        f.write(outfile.toUtf8());
        f.close();
    }
}

// Export the faultstringlib.cpp file to the location chosen by the user.
// Export will function as follows (same as other source files):
// The program will attempt to export every fault listed in the faultconstants
// table in the database. Before it includes a fault, though, it checks
// the fault table currently being viewed and makes sure the given fault is
// present and marked with an "IN_USE" or "ACTIVE" fm_status. Also, if the value of "payload" passed in
// is positive, it will only include faults of the payload corresponding to that value.
// If all of those conditions are met, the fault is included in the export
void Exporter::exportFaultStringLibcpp(QString version, QWidget *parent, int payload) {
    QString filename = QFileDialog::getSaveFileName(parent, "Save Fault Lib Source File", "faultstringlib.cpp", "C++ Source Files (*.cpp)");
    currentIndent = 0;

    if (!filename.isEmpty() && !filename.isNull()) { // If the user did not cancel out of the file save dialog
        QString outfile;
        MyModel::convertToUnderscores(&version);

        addLine("#include \"faultstringlib.h\"", &outfile);
        addLine("#include \"MessageConstants.h\"", &outfile);
        addLine("#include \"Data/FaultGroupData.h\"\n", &outfile);
        addLine("#include <QDebug>\n", &outfile);
        addLine("using namespace UsvCommon::Data;\n", &outfile);
        addLine("faultStringLib::faultStringLib(QObject * parent) : QObject(parent)", &outfile);
        addLine("{", &outfile);
        increaseIndent();

        QSqlQuery q;
        QVector<QVector<QString>> enumData;
        q.exec("select * from faultconstants");

        // For every fault in the faultconstants table, check the fm_status
        // in the correct release table
        while (q.next()) {
            QString enumName = q.value(0).toString();
            QString enumGroup = q.value(1).toString();

            QSqlQuery u;

            u.exec("select fm_status, configuration from " + version + " where group_name = '" + enumGroup + "' and enum_name = '" + enumName + "'");

            if (u.next()) {// If fault can be found in the main table
                if (payload >= 0) { // If the value of payload passed in is positive - determines if it will filter for a specific payload
                    if ((u.value(0).toString().toLower() == "in use" || u.value(0).toString().toLower() == "active") && (u.value(1).toString().contains(payloads[payload], Qt::CaseInsensitive) || u.value(1).toString().contains("Common Craft", Qt::CaseInsensitive))) {
                        enumData.append(QVector<QString>{enumGroup, enumName});
                    }
                }
                else { // payload < 0, Ignore the payload assigned to the faults
                    if (u.value(0).toString().toLower() == "in use" || u.value(0).toString().toLower() == "active") {
                        enumData.append(QVector<QString>{enumGroup, enumName});
                    }
                }
            }
        }

        // Sort by group name first so they appear next to each other in the final export
        std::sort(enumData.begin(), enumData.end(), compareGroupNames());

        for(QVector<QString> fault : enumData) {
            addLine("wca" + fault.at(0) + "Map.insert(FConst::" + fault.at(0) + "::" + fault.at(1) +
                    ", " + fault.at(0) + "_" + fault.at(1) + ");", &outfile);
        }

        // Add boilerplate code. If a new fault group is added in the future this will
        // need to be updated
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::AUTOPILOT),  wcaAutopilotMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::CENTRAL),  wcaCentralMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::MVCS),  wcaMVCSMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::POSITION),  wcaPositionMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::POWER),  wcaPowerMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::PROPULSION),  wcaPropulsionMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::SECONDARY),  wcaSecondaryMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::DDC),  wcaDDCMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::SPC),  wcaSPCMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::WC),  wcaWCMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::SCC),  wcaSACtMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::STRAIN),  wcaStrainMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::DSS),  wcaDSSMap);", &outfile);
        addLine("wcaIdMap.insert(static_cast<int>(FaultGroupData::groupIdEnum::VPLC),  wcaVPLCtMap);", &outfile);
        decreaseIndent();
        addLine("}\n", &outfile);

        addLine("faultStringLib::~faultStringLib()", &outfile);
        addLine("{\n}\n", &outfile);

        addLine("QString faultStringLib::getWcaMsg(int groupId, int faultId)", &outfile);
        addLine("{", &outfile);
        increaseIndent();
        addLine("auto tempMap = getGroupMap(groupId);//get the map of faults from the group = groupId", &outfile);
        addLine("auto tempMsg = tempMap.value(faultId);//get the wcaMsg struct from the map of faults from above", &outfile);
        addLine("if(tempMsg.message.isNull()){", &outfile);
        increaseIndent();
        addLine("return \"WCA text not found. Group ID: \" + QString::number(groupId) + \" Fault ID: \" + QString::number(faultId);", &outfile);
        decreaseIndent();
        addLine("return tempMsg.message;//return the text", &outfile);
        decreaseIndent();
        addLine("}\n", &outfile);

        addLine("QString faultStringLib::getTrigger(int groupId, int faultId)", &outfile);
        addLine("{", &outfile);
        increaseIndent();
        addLine("auto tempMap = getGroupMap(groupId);", &outfile);
        addLine("auto tempMsg = tempMap.value(faultId);", &outfile);
        addLine("if(tempMsg.trigger.isNull()){", &outfile);
        increaseIndent();
        addLine("return \"Unknown\";", &outfile);
        decreaseIndent();
        addLine("}", &outfile);
        addLine("return tempMsg.trigger;", &outfile);
        decreaseIndent();
        addLine("}\n", &outfile);

        addLine("WcaLevel faultStringLib::getLevel(int groupId, int faultId)", &outfile);
        addLine("{", &outfile);
        increaseIndent();
        addLine("wcaMsg defaultMsg = {0, UNKNOWN, "", "", ""};", &outfile);
        addLine("auto tempMap = getGroupMap(groupId);", &outfile);
        addLine("auto tempMsg = tempMap.value(faultId, defaultMsg);", &outfile);
        addLine("return tempMsg.lvl;", &outfile);
        decreaseIndent();
        addLine("}\n", &outfile);

        addLine("QString faultStringLib::getSubSys(int groupId, int faultId)", &outfile);
        addLine("{", &outfile);
        increaseIndent();
        addLine("auto tempMap = getGroupMap(groupId);", &outfile);
        addLine("auto tempMsg = tempMap.value(faultId);", &outfile);
        addLine("if(tempMsg.subSystem.isNull()){", &outfile);
        increaseIndent();
        addLine("return \"Unknown\";", &outfile);
        decreaseIndent();
        addLine("}", &outfile);
        addLine("return tempMsg.subSystem", &outfile);
        decreaseIndent();
        addLine("}\n", &outfile);

        addLine("QMap<int, wcaMsg> faultStringLib::getGroupMap(int group)", &outfile);
        addLine("{", &outfile);
        increaseIndent();
        addLine("if (group == CONTROL_STATION_GROUP){", &outfile);
        increaseIndent();
        addLine("return wcaControlSoftwareMap;", &outfile);
        decreaseIndent();
        addLine("}", &outfile);
        addLine("return wcaIdMap.value(group);", &outfile);
        decreaseIndent();
        addLine("}\n", &outfile);

        QFile f(filename);
        f.open(QIODevice::WriteOnly);
        f.write(outfile.toUtf8());
        f.close();
    }

}

// Export the WCA Guidebook XML Index file to the location chosen by the user
// Function takes a pointer to a query, the name of the release in the database, and a number
// representing what payload the export is for:
// 0 - UISS
// 1 - MH
// A fault is only included in an export if it's fm_status is "IN USE" or "ACTIVE" and it has a valid wca_severity.
// This function makes use of the "tags" nested map declared in export.h. It contains all the xml content that
// is pieced together to build each row in the output
void Exporter::exportWcaGuidebookIndex(QWidget *parent, QSqlQuery *q, QString releaseName, int payload) {
    QString filename = QFileDialog::getSaveFileName(parent, "Save WCA Guidebook XML - Index", "wca_guidebook_index.txt", "Text File (*.txt)");

    if (!filename.isEmpty()) {    // Allows user to close out and not save a file
        qInfo() << "Exporting File";

        int index = filename.lastIndexOf('.');
        QString sub_filename = filename;
        sub_filename.insert(index, "_" + payloads[payload]);

        QFile f(sub_filename);

        if (!f.open(QFile::WriteOnly | QFile::Text)) {
            qInfo() << "Error Writing File";
            f.close();
        }

        QTextStream outfile(&f);
        if (payload == 0) {
            q->exec("select * from " + releaseName + " where configuration like '%UISS%' or configuration like '%Common Craft%' order by fault_code");
        }
        else if (payload == 1) {
            q->exec("select * from " + releaseName + " where configuration like '%MH%' or configuration like '%Common Craft%' order by fault_code");
        }

        if (payload == 0) {
            while (q->next()) { // Output a row for every fault
                QString severity = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_severity")).toString().replace("&", "&amp;");
                QString faultCode = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fault_code")).toString().replace("&", "&amp;");

                if ((q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fm_status")).toString().contains("in use", Qt::CaseSensitivity::CaseInsensitive) ||
                     q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fm_status")).toString().contains("active", Qt::CaseSensitivity::CaseInsensitive))
                     && (severity.toLower() == "warning" | severity.toLower() == "caution" | severity.toLower() == "advisory")
                        ) { // Check the fm_status and wca_severity for export

                    // ----- Start of Row -----
                    outfile << tags.value("row").value("start");
                    // ------------------------
                    outfile << tags.value("bold_entry_with_id").value("start");
                    outfile << faultCode;
                    outfile << tags.value("bold_entry_with_id").value("middle");
                    outfile << faultCode;
                    outfile << tags.value("bold_entry_with_id").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << severity;
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("bold_entry").value("start");
                    outfile << q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_message")).toString().replace("&", "&amp;");
                    outfile << tags.value("bold_entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("ref").value("start");
                    outfile << faultCode;
                    outfile << tags.value("offboard_lcs_iss").value(severity.toLower());
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("ref").value("start");
                    outfile << faultCode;
                    outfile << tags.value("lcs_lr_iss").value(severity.toLower());
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("ref").value("start");
                    outfile << faultCode;
                    outfile << tags.value("onboard_lcs_iss").value(severity.toLower());
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("ref").value("start");
                    outfile << faultCode;
                    outfile << tags.value("mine_warfare_evaluator_iss").value(severity.toLower());
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("row").value("end") + '\n';
                    // ------ End of Row ------
                }
            }
        }
        else if (payload == 1) {
            while (q->next()) { // Output a row for every fault
                QString severity = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_severity")).toString().replace("&", "&amp;");
                QString faultCode = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fault_code")).toString().replace("&", "&amp;");

                if ((q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fm_status")).toString().contains("in use", Qt::CaseSensitivity::CaseInsensitive) ||
                        q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fm_status")).toString().contains("active", Qt::CaseSensitivity::CaseInsensitive))
                        && !severity.isEmpty()
                        ) {// Check the fm_status and wca_severity for export
                    // ----- Start of Row -----
                    outfile << tags.value("row").value("start");
                    // ------------------------
                    outfile << tags.value("bold_entry_with_id").value("start");
                    outfile << faultCode;
                    outfile << tags.value("bold_entry_with_id").value("middle");
                    outfile << faultCode;
                    outfile << tags.value("bold_entry_with_id").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << severity;
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("bold_entry").value("start");
                    outfile << q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_message")).toString().replace("&", "&amp;");
                    outfile << tags.value("bold_entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("ref").value("start");
                    outfile << faultCode;
                    outfile << tags.value("offboard_lcs_mhp_20").value(severity.toLower());
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("ref").value("start");
                    outfile << faultCode;
                    outfile << tags.value("lcs_lr_mhp_20").value(severity.toLower());
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("ref").value("start");
                    outfile << faultCode;
                    outfile << tags.value("onboard_lcs_mhp_20").value(severity.toLower());
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("ref").value("start");
                    outfile << faultCode;
                    outfile << tags.value("mine_warfare_evaluator_mhp_20").value(severity.toLower());
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("row").value("end") + '\n';
                    // ------ End of Row ------
                }
            }
        }
        f.close();
    }
}

// Export the WCA Guidebook XML Onboard LCS file to the location chosen by the user.
// Like the other WCA Guidebook functions, this function takes a pointer to the query to use,
// the name of the release to use, and an int representing the payload to export for:
// 0 - UISS
// 1 - MH
// The function will automatically complete 3 separate exports for the different fault severities
// and append the appropriate ending to the filenames ("_warning", "_caution", and "_advisory").
void Exporter::exportWcaGuidebookOnboardLCS(QWidget *parent, QSqlQuery *q, QString releaseName, int payload) {
    QString filename = QFileDialog::getSaveFileName(parent, "Save WCA Guidebook XML - Onboard LCS", "onboard_lcs.txt", "Text File (*.txt)");

    for (int i = 0; i < 3; i++) {
        int index = filename.lastIndexOf('.');
        QString sub_filename = filename;
        sub_filename.insert(index, "_" + severities[i]);

        if (!filename.isEmpty()) {    // Allows user to close out and not save a file
            qInfo() << "Exporting File";
            QFile f(sub_filename);

            if (!f.open(QFile::WriteOnly | QFile::Text)) {
                qInfo() << "Error Writing File";
                f.close();
            }

            QTextStream outfile(&f);

            if (payload == 0) {
                q->exec("select * from " + releaseName + " where configuration like '%UISS%' or configuration like '%Common Craft%' order by fault_code");
            }
            else if (payload == 1) {
                q->exec("select * from " + releaseName + " where configuration like '%MH%' or configuration like '%Common Craft%' order by fault_code");
            }


            while (q->next()) { // Output a row for every fault
                QString severity = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_severity")).toString().replace("&", "&amp;");
                QString faultCode = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fault_code")).toString().replace("&", "&amp;");

                if ((q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fm_status")).toString().contains("in use", Qt::CaseSensitivity::CaseInsensitive) ||
                     q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fm_status")).toString().contains("active", Qt::CaseSensitivity::CaseInsensitive))
                        && (severity.toLower() == severities[i])) { // Check the fm_status and wca_severity for export
                    // ----- Start of Row -----
                    outfile << tags.value("row").value("start");
                    // ------------------------
                    outfile << tags.value("bold_entry_with_id").value("start");
                    outfile << faultCode;
                    outfile << tags.value("bold_entry_with_id").value("middle");
                    outfile << faultCode;
                    outfile << tags.value("bold_entry_with_id").value("end");
                    // ------------------------
                    outfile << tags.value("bold_entry").value("start");
                    outfile << q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_message")).toString().replace("&", "&amp;");
                    outfile << tags.value("bold_entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("random_list").value("start");
                    for (auto e : q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_trigger")).toString().replace("&", "&amp;").split(".", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive)) {
                        outfile << tags.value("list_item").value("start");
                        outfile << e;
                        outfile << tags.value("list_item").value("end");
                    }
                    outfile << tags.value("random_list").value("end");
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("random_list").value("start");
                    for (auto e : q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("failure_impact")).toString().replace("&", "&amp;").split(".", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive)) {
                        outfile << tags.value("list_item").value("start");
                        outfile << e;
                        outfile << tags.value("list_item").value("end");
                    }
                    outfile << tags.value("random_list").value("end");
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry_with_numbered_list").value("start");
                    for (auto e : q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("corrective_maintenance_actions")).toString().replace("&", "&amp;").split(".", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive)) {
                        outfile << tags.value("list_item").value("start");
                        outfile << e;
                        outfile << tags.value("list_item").value("end");
                    }
                    outfile << tags.value("entry_with_numbered_list").value("end");
                    // ------------------------
                    outfile << tags.value("row").value("end") + '\n';
                }
            }

            f.close();
        }
    }
}

// Export the WCA Guidebook XML Mine Warfare Evaluator file to the location chosen by the user.
// Like the other WCA Guidebook functions, this function takes a pointer to the query to use,
// the name of the release to use, and an int representing the payload to export for:
// 0 - UISS
// 1 - MH
// The function will automatically complete 3 separate exports for the different fault severities
// and append the appropriate ending to the filenames ("_warning", "_caution", and "_advisory").
void Exporter::exportWcaGuidebookMineWarfareEval(QWidget *parent, QSqlQuery *q, QString releaseName, int payload) {
    QString filename = QFileDialog::getSaveFileName(parent, "Save WCA Guidebook XML - Onboard LCS", "mwe.txt", "Text File (*.txt)");

    for (int i = 0; i < 3; i++) {
        int index = filename.lastIndexOf('.');
        QString sub_filename = filename;
        sub_filename.insert(index, "_" + severities[i]);

        if (!filename.isEmpty()) {    // Allows user to close out and not save a file
            qInfo() << "Exporting File";
            QFile f(sub_filename);

            if (!f.open(QFile::WriteOnly | QFile::Text)) {
                qInfo() << "Error Writing File";
                f.close();
            }

            QTextStream outfile(&f);
            if (payload == 0) {
                q->exec("select * from " + releaseName + " where configuration like '%UISS%' or configuration like '%Common Craft%' order by fault_code");
            }
            else if (payload == 1) {
                q->exec("select * from " + releaseName + " where configuration like '%MH%' or configuration like '%Common Craft%' order by fault_code");
            }


            while (q->next()) { // Output a row for every fault
                QString severity = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_severity")).toString().replace("&", "&amp;");
                QString faultCode = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fault_code")).toString().replace("&", "&amp;");
                QString onboardLcs = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("onboard_lcs")).toString().replace("&", "&amp;");
                QString offboardLcs = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("offboard_lcs")).toString().replace("&", "&amp;");
                QString landr = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("lcs_lr")).toString().replace("&", "&amp;");
                QString offboard_rvo = "No RVO Action";
                QString offboard_rso = "No RSO Action";
                QString launch = "No Launch Action";
                QString recovery = "No Recovery Action";

                // String parsing to try and find the RVO and RSO instructions through the labels
                if (offboardLcs.contains("RVO:", Qt::CaseInsensitive)) {
                    if (offboardLcs.contains("RSO:", Qt::CaseInsensitive)) {
                        QStringList actions = offboardLcs.split("RSO:", QString::SkipEmptyParts, Qt::CaseInsensitive);
                        offboard_rvo = actions[0].remove("RVO:", Qt::CaseInsensitive).trimmed();
                        offboard_rso = actions[1].trimmed();
                    }
                    else {
                        offboard_rvo = offboardLcs.remove("RVO:", Qt::CaseInsensitive).trimmed();
                    }
                }
                else if (offboardLcs.contains("RSO:", Qt::CaseInsensitive)) {
                    offboard_rso = offboardLcs.remove("RSO:", Qt::CaseInsensitive).trimmed();
                }
                else {
                    offboard_rso = offboard_rvo = offboardLcs.trimmed();
                }

                // String parsing to try and find the Launch and Recovery instructions through the labels
                if (landr.contains("Launch:", Qt::CaseInsensitive)) {
                    if (landr.contains("Recovery:", Qt::CaseInsensitive)) {
                        QStringList actions = landr.split("Recovery:", QString::SkipEmptyParts, Qt::CaseInsensitive);
                        launch = actions[0].remove("Launch:", Qt::CaseInsensitive).trimmed();
                        recovery = actions[1].trimmed();
                    }
                    else {
                        launch = landr.remove("Launch:", Qt::CaseInsensitive).trimmed();
                    }
                }
                else if (landr.contains("Recovery:", Qt::CaseInsensitive)) {
                    recovery = landr.remove("Recovery:", Qt::CaseInsensitive).trimmed();
                }
                else {
                    launch = recovery = landr.trimmed();
                }


                if ((q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fm_status")).toString().contains("in use", Qt::CaseSensitivity::CaseInsensitive) ||
                        q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fm_status")).toString().contains("active", Qt::CaseSensitivity::CaseInsensitive))
                        && (severity.toLower() == severities[i])) { // Check the fm_status and wca_severity for export
                    // ----- Start of Row -----
                    outfile << tags.value("row").value("start");
                    // ------------------------
                    outfile << tags.value("bold_entry_with_id").value("start");
                    outfile << faultCode;
                    outfile << tags.value("bold_entry_with_id").value("middle");
                    outfile << faultCode;
                    outfile << tags.value("bold_entry_with_id").value("end");
                    // ------------------------
                    outfile << tags.value("bold_entry").value("start");
                    outfile << q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_message")).toString().replace("&", "&amp;");
                    outfile << tags.value("bold_entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("random_list").value("start");
                    for (auto e : q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("failure_impact")).toString().replace("&", "&amp;").split(".", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive)) {
                        outfile << tags.value("list_item").value("start");
                        outfile << e;
                        outfile << tags.value("list_item").value("end");
                    }
                    outfile << tags.value("random_list").value("end");
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry_with_numbered_list").value("start");
                    // ---------
                    outfile << tags.value("list_item").value("start");
                    outfile << tags.value("bold_text").value("start");
                    outfile << "Onboard LCS: ";
                    outfile << tags.value("bold_text").value("end");
                    outfile << q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("onboard_lcs")).toString().replace("&", "&amp;");
                    outfile << tags.value("list_item").value("end");
                    // ---------
                    outfile << tags.value("list_item").value("start");
                    outfile << tags.value("bold_text").value("start");
                    outfile << "Offboard LCS: ";
                    outfile << tags.value("bold_text").value("end");
                    outfile << tags.value("numbered_list").value("start");
                    // ---
                    outfile << tags.value("list_item").value("start");
                    outfile << tags.value("bold_text").value("start");
                    outfile << "RVO: ";
                    outfile << tags.value("bold_text").value("end");
                    outfile << offboard_rvo.replace("&", "&amp;");
                    outfile << tags.value("list_item").value("end");
                    // ---
                    outfile << tags.value("list_item").value("start");
                    outfile << tags.value("bold_text").value("start");
                    outfile << "RSO: ";
                    outfile << tags.value("bold_text").value("end");
                    outfile << offboard_rso.replace("&", "&amp;");
                    outfile << tags.value("list_item").value("end");
                    // ---
                    outfile << tags.value("numbered_list").value("end");
                    outfile << tags.value("list_item").value("end");
                    // ---------
                    outfile << tags.value("list_item").value("start");
                    outfile << tags.value("bold_text").value("start");
                    outfile << "LCS L&amp;R: ";
                    outfile << tags.value("bold_text").value("end");
                    outfile << tags.value("numbered_list").value("start");
                    // ---
                    outfile << tags.value("list_item").value("start");
                    outfile << tags.value("bold_text").value("start");
                    outfile << "Launch: " + launch.replace("&", "&amp;");
                    outfile << tags.value("bold_text").value("end");
                    outfile << tags.value("list_item").value("end");
                    // ---
                    outfile << tags.value("list_item").value("start");
                    outfile << tags.value("bold_text").value("start");
                    outfile << "Recovery: " + recovery.replace("&", "&amp;");
                    outfile << tags.value("bold_text").value("end");
                    outfile << tags.value("list_item").value("end");
                    // ---
                    outfile << tags.value("numbered_list").value("end");
                    outfile << tags.value("list_item").value("end");
                    // ---------
                    outfile << tags.value("entry_with_numbered_list").value("end");
                    // ------------------------

                    outfile << tags.value("entry_with_numbered_list").value("start");
                    for (auto e : q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("corrective_maintenance_actions")).toString().replace("&", "&amp;").split(".", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive)) {
                        outfile << tags.value("list_item").value("start");
                        outfile << e;
                        outfile << tags.value("list_item").value("end");
                    }
                    outfile << tags.value("entry_with_numbered_list").value("end");

                    outfile << tags.value("row").value("end");
                    // ------ End of Row ------
                }
            }

            f.close();
        }
    }
}

// Export the WCA Guidebook XML Launch and Recovery file to the location chosen by the user.
// Like the other WCA Guidebook functions, this function takes a pointer to the query to use,
// the name of the release to use, and an int representing the payload to export for:
// 0 - UISS
// 1 - MH
// The function will automatically complete 3 separate exports for the different fault severities
// and append the appropriate ending to the filenames ("_warning", "_caution", and "_advisory").
void Exporter::exportWcaGuidebookLR(QWidget *parent, QSqlQuery *q, QString releaseName, int payload) {
    QString filename = QFileDialog::getSaveFileName(parent, "Save WCA Guidebook XML - LCS L&R", "l_and_r.txt", "Text File (*.txt)");

    for (int i = 0; i < 3; i++) {
        int index = filename.lastIndexOf('.');
        QString sub_filename = filename;
        sub_filename.insert(index, "_" + severities[i]);

        if (!filename.isEmpty()) {    // Allows user to close out and not save a file
            qInfo() << "Exporting File";
            QFile f(sub_filename);

            if (!f.open(QFile::WriteOnly | QFile::Text)) {
                qInfo() << "Error Writing File";
                f.close();
            }

            QTextStream outfile(&f);
            if (payload == 0) {
                q->exec("select * from " + releaseName + " where configuration like '%UISS%' or configuration like '%Common Craft%' order by fault_code");
            }
            else if (payload == 1) {
                q->exec("select * from " + releaseName + " where configuration like '%MH%' or configuration like '%Common Craft%' order by fault_code");
            }


            while (q->next()) { // Output a row for every fault
                QString severity = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_severity")).toString().replace("&", "&amp;");
                QString faultCode = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fault_code")).toString().replace("&", "&amp;");
                QString landr = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("lcs_lr")).toString().replace("&", "&amp;");
                QString launch;
                QString recovery;

                // String parsing to try and find the Launch and Recovery instructions through the labels
                if (landr.contains("Launch:", Qt::CaseInsensitive)) {
                    if (landr.contains("Recovery:", Qt::CaseInsensitive)) {
                        QStringList actions = landr.split("Recovery:", QString::SkipEmptyParts, Qt::CaseInsensitive);
                        launch = actions[0].remove("Launch:", Qt::CaseInsensitive).trimmed();
                        recovery = actions[1].trimmed();
                    }
                    else {
                        launch = landr.remove("Launch:", Qt::CaseInsensitive).trimmed();
                    }
                }
                else if (landr.contains("Recovery:", Qt::CaseInsensitive)) {
                    recovery = landr.remove("Recovery:", Qt::CaseInsensitive).trimmed();
                }
                else {
                    launch = recovery = landr.trimmed();
                }

                if ((q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fm_status")).toString().contains("in use", Qt::CaseSensitivity::CaseInsensitive) ||
                        q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fm_status")).toString().contains("active", Qt::CaseSensitivity::CaseInsensitive))
                        && (severity.toLower() == severities[i])) { // Check the fm_status and wca_severity for export
                    // ----- Start of Row -----
                    outfile << tags.value("row").value("start");
                    // ------------------------
                    outfile << tags.value("bold_entry_with_id").value("start");
                    outfile << faultCode;
                    outfile << tags.value("bold_entry_with_id").value("middle");
                    outfile << faultCode;
                    outfile << tags.value("bold_entry_with_id").value("end");
                    // ------------------------
                    outfile << tags.value("bold_entry").value("start");
                    outfile << q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_message")).toString().replace("&", "&amp;");
                    outfile << tags.value("bold_entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry_with_numbered_list").value("start");
                    for (auto e : launch.replace("&", "&amp;").split(".", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive)) {
                        outfile << tags.value("list_item").value("start");
                        outfile << e;
                        outfile << tags.value("list_item").value("end");
                    }
                    outfile << tags.value("entry_with_numbered_list").value("end");
                    // ------------------------
                    outfile << tags.value("entry_with_numbered_list").value("start");
                    for (auto e : recovery.replace("&", "&amp;").split(".", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive)) {
                        outfile << tags.value("list_item").value("start");
                        outfile << e;
                        outfile << tags.value("list_item").value("end");
                    }
                    outfile << tags.value("entry_with_numbered_list").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("random_list").value("start");
                    for (auto e : q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_notes")).toString().replace("&", "&amp;").split(".", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive)) {
                        outfile << tags.value("list_item").value("start");
                        outfile << e;
                        outfile << tags.value("list_item").value("end");
                    }
                    outfile << tags.value("random_list").value("end");
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("random_list").value("start");
                    for (auto e : q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("failure_impact")).toString().replace("&", "&amp;").split(".", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive)) {
                        outfile << tags.value("list_item").value("start");
                        outfile << e;
                        outfile << tags.value("list_item").value("end");
                    }
                    outfile << tags.value("random_list").value("end");
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("row").value("end");
                    // ------ End of Row ------
                }
            }

            f.close();
        }
    }
}

// Export the WCA Guidebook XML Offboard LCS file to the location chosen by the user.
// Like the other WCA Guidebook functions, this function takes a pointer to the query to use,
// the name of the release to use, and an int representing the payload to export for:
// 0 - UISS
// 1 - MH
// The function will automatically complete 3 separate exports for the different fault severities
// and append the appropriate ending to the filenames ("_warning", "_caution", and "_advisory").
void Exporter::exportWcaGuidebookOffboardLCS(QWidget *parent, QSqlQuery *q, QString releaseName, int payload) {
    QString filename = QFileDialog::getSaveFileName(parent, "Save WCA Guidebook XML - Offboard LCS", "offboard_lcs.txt", "Text File (*.txt)");

    for (int i = 0; i < 3; i++) {
        int index = filename.lastIndexOf('.');
        QString sub_filename = filename;
        sub_filename.insert(index, "_" + severities[i]);

        if (!filename.isEmpty()) {    // Allows user to close out and not save a file
            qInfo() << "Exporting File";
            QFile f(sub_filename);

            if (!f.open(QFile::WriteOnly | QFile::Text)) {
                qInfo() << "Error Writing File";
                f.close();
            }

            QTextStream outfile(&f);
            if (payload == 0) {
                q->exec("select * from " + releaseName + " where configuration like '%UISS%' or configuration like '%Common Craft%' order by fault_code");
            }
            else if (payload == 1) {
                q->exec("select * from " + releaseName + " where configuration like '%MH%' or configuration like '%Common Craft%' order by fault_code");
            }


            while (q->next()) { // Output a row for every fault
                QString severity = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_severity")).toString().replace("&", "&amp;");
                QString faultCode = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fault_code")).toString().replace("&", "&amp;");
                QString offboardLcs = q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("offboard_lcs")).toString().replace("&", "&amp;");
                QString offboard_rvo = "No RVO Action";
                QString offboard_rso = "No RSO Action";

                // String parsing to try and find the RVO and RSO instructions through the labels
                if (offboardLcs.contains("RVO:", Qt::CaseInsensitive)) {
                    if (offboardLcs.contains("RSO:", Qt::CaseInsensitive)) {
                        QStringList actions = offboardLcs.split("RSO:", QString::SkipEmptyParts, Qt::CaseInsensitive);
                        offboard_rvo = actions[0].remove("RVO:", Qt::CaseInsensitive).trimmed();
                        offboard_rso = actions[1].trimmed();
                    }
                    else {
                        offboard_rvo = offboardLcs.remove("RVO:", Qt::CaseInsensitive).trimmed();
                    }
                }
                else if (offboardLcs.contains("RSO:", Qt::CaseInsensitive)) {
                    offboard_rso = offboardLcs.remove("RSO:", Qt::CaseInsensitive).trimmed();
                }
                else {
                    offboard_rso = offboard_rvo = offboardLcs.trimmed();
                }

                if ((q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fm_status")).toString().contains("in use", Qt::CaseSensitivity::CaseInsensitive) ||
                        q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("fm_status")).toString().contains("active", Qt::CaseSensitivity::CaseInsensitive))
                        && (severity.toLower() == severities[i])) { // Check the fm_status and wca_severity for export
                    // ----- Start of Row -----
                    outfile << tags.value("row").value("start");
                    // ------------------------
                    outfile << tags.value("bold_entry_with_id").value("start");
                    outfile << faultCode;
                    outfile << tags.value("bold_entry_with_id").value("middle");
                    outfile << faultCode;
                    outfile << tags.value("bold_entry_with_id").value("end");
                    // ------------------------
                    outfile << tags.value("bold_entry").value("start");
                    outfile << q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("wca_message")).toString().replace("&", "&amp;");
                    outfile << tags.value("bold_entry").value("end");
                    // ------------------------
                    outfile << tags.value("entry_with_numbered_list").value("start");
                    for (auto e : offboard_rvo.replace("&", "&amp;").split(".", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive)) {
                        outfile << tags.value("list_item").value("start");
                        outfile << e;
                        outfile << tags.value("list_item").value("end");
                    }
                    outfile << tags.value("entry_with_numbered_list").value("end");
                    // ------------------------
                    outfile << tags.value("entry_with_numbered_list").value("start");
                    for (auto e : offboard_rso.replace("&", "&amp;").split(".", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive)) {
                        outfile << tags.value("list_item").value("start");
                        outfile << e;
                        outfile << tags.value("list_item").value("end");
                    }
                    outfile << tags.value("entry_with_numbered_list").value("end");
                    // ------------------------
                    outfile << tags.value("entry").value("start");
                    outfile << tags.value("random_list").value("start");
                    for (auto e : q->value(myModel->columnMap.value("database_name").value("WCAs").indexOf("failure_impact")).toString().replace("&", "&amp;").split(".", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive)) {
                        outfile << tags.value("list_item").value("start");
                        outfile << e;
                        outfile << tags.value("list_item").value("end");
                    }
                    outfile << tags.value("random_list").value("end");
                    outfile << tags.value("entry").value("end");
                    // ------------------------
                    outfile << tags.value("row").value("end");
                    // ------ End of Row ------
                }
            }

            f.close();
        }
    }
}

// Export the WCA Guidebook XML Failsafes file to the location chosen by the user.
// Like the other WCA Guidebook functions, this function takes a pointer to the query to use
// and the name of the release to use.
void Exporter::exportWcaGuidebookFailsafes(QWidget *parent, QSqlQuery *q, QString releaseName, int payload) {
    QString filename = QFileDialog::getSaveFileName(parent, "Save WCA Guidebook XML - Failsafes", "failsafes.txt", "Text File (*.txt)");

    if (!filename.isEmpty()) {    // Allows user to close out and not save a file
        qInfo() << "Exporting File";
        QFile f(filename);

        if (!f.open(QFile::WriteOnly | QFile::Text)) {
            qInfo() << "Error Writing File";
            f.close();
        }

        QTextStream outfile(&f);
        if (payload == 0) {
            q->exec("select * from " + releaseName + "_failsafes where configuration like '%UISS%' or configuration like '%Common Craft%'");
        }
        else if (payload == 1) {
            q->exec("select * from " + releaseName + "_failsafes where configuration like '%MH%' or configuration like '%Common Craft%'");
        }


        while (q->next()) { // Output a row for every fault
            QStringList faults = q->value(myModel->columnMap.value("database_name").value("Failsafes").indexOf("triggering_faults")).toString().replace("&", "&amp;").split(",", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive);
            QVector<QString> includedFaults;
            int extraRows = faults.size() - 1;
            QString extraRowString = "";

            // Check whether each listed wca in triggering_faults is not deleted in the release currently being viewed
            for (int i = 0; i < faults.size(); i++) {
                if (myModel->checkFaultCode(faults[i], releaseName, "")) {
                    includedFaults.push_back(faults[i]);
                }
                else {
                    extraRows--;
                }
            }
            if (extraRows > 0) {
                extraRowString = "_extra_rows";
            }

            QString failsafeCode = q->value(myModel->columnMap.value("database_name").value("Failsafes").indexOf("failsafe_id")).toString();
            // Parse the operational states field, which is (currently) formatted with newlines separating numbered items and the text "All States:" at the top
            QString operationalStates = q->value(myModel->columnMap.value("database_name").value("Failsafes").indexOf("operational_state")).toString().replace("&", "&amp;");
            operationalStates.remove("All States:", Qt::CaseInsensitive);
            operationalStates.remove(QRegExp("[1234567890]/."));
            operationalStates = operationalStates.trimmed();
            QStringList operationalStates_list = operationalStates.split("\n", QString::SkipEmptyParts, Qt::CaseInsensitive);
            QStringList functionalImpacts = q->value(myModel->columnMap.value("database_name").value("Failsafes").indexOf("functional_impact")).toString().replace("&", "&amp;").split(".", QString::SkipEmptyParts, Qt::CaseInsensitive);
            QVector<QString> automatedResponseFields = {q->value(myModel->columnMap.value("database_name").value("Failsafes").indexOf("first_auto_response")).toString().replace("&", "&amp;"),
                                             q->value(myModel->columnMap.value("database_name").value("Failsafes").indexOf("second_auto_response")).toString().replace("&", "&amp;"),
                                             q->value(myModel->columnMap.value("database_name").value("Failsafes").indexOf("third_auto_response")).toString().replace("&", "&amp;")};

            QVector<QVector<QString>> automatedResponses;
            for (QString e : automatedResponseFields) {
                automatedResponses.push_back({});
                // String parsing to try and find the deployed and stowed sections through the labels
                if (e.contains("Payload Deployed:", Qt::CaseInsensitive) && e.contains("Payload Stowed:", Qt::CaseInsensitive)) {
                    QStringList actions = e.split("Payload Stowed:", QString::SkipEmptyParts, Qt::CaseInsensitive);
                    QString payloadDeployed = actions.at(0);
                    payloadDeployed.remove("Payload Deployed:", Qt::CaseInsensitive);
                    automatedResponses.back().push_back(payloadDeployed.trimmed());
                    automatedResponses.back().push_back(actions.at(1).trimmed());
                }
                else { // If at least one keyword not found, just use the raw text
                    automatedResponses.back().push_back(e);
                }
            }

            QStringList triggers = q->value(myModel->columnMap.value("database_name").value("Failsafes").indexOf("triggers")).toString().split(",", QString::SkipEmptyParts, Qt::CaseInsensitive);

            if (!failsafeCode.isEmpty()) {
                // ----- Start of Row -----
                outfile << tags.value("row").value("start");
                // ------------------------
                outfile << tags.value("bold_entry_with_id" + extraRowString).value("start");
                outfile << failsafeCode;
                if (extraRows > 0) {
                    outfile << tags.value("bold_entry_with_id_extra_rows").value("middle1");
                    outfile << extraRows;
                    outfile << tags.value("bold_entry_with_id_extra_rows").value("middle2");
                }
                else {
                    outfile << tags.value("bold_entry_with_id").value("middle");
                }
                outfile << failsafeCode;
                outfile << tags.value("bold_entry_with_id" + extraRowString).value("end");
                // ------------------------
                outfile << tags.value("bold_entry" + extraRowString).value("start");
                if (extraRows > 0) {
                    outfile << extraRows;
                    outfile << tags.value("bold_entry_extra_rows").value("middle");
                }
                outfile << q->value(myModel->columnMap.value("database_name").value("Failsafes").indexOf("failure_name")).toString().replace("&", "&amp;");
                outfile << tags.value("bold_entry" + extraRowString).value("end");
                // ------------------------
                outfile << tags.value("entry" + extraRowString).value("start");
                if (extraRows > 0) {
                    outfile << extraRows;
                    outfile << tags.value("entry_extra_rows").value("middle");
                }
                outfile << tags.value("random_list").value("start");
                for (auto e : operationalStates_list) {
                    outfile << tags.value("list_item").value("start");
                    outfile << e.trimmed();
                    outfile << tags.value("list_item").value("end");
                }
                outfile << tags.value("random_list").value("end");
                outfile << tags.value("entry" + extraRowString).value("end");
                // ------------------------
                outfile << tags.value("entry" + extraRowString).value("start");
                if (extraRows > 0) {
                    outfile << extraRows;
                    outfile << tags.value("entry_extra_rows").value("middle");
                }
                outfile << tags.value("random_list").value("start");
                for (QString e : triggers) {
                    outfile << tags.value("list_item").value("start");
                    outfile << e.trimmed();
                    outfile << tags.value("list_item").value("end");
                }
                outfile << tags.value("random_list").value("end");
                outfile << tags.value("entry" + extraRowString).value("end");
                // ------------------------
                outfile << tags.value("entry" + extraRowString).value("start");
                if (extraRows > 0) {
                    outfile << extraRows;
                    outfile << tags.value("entry_extra_rows").value("middle");
                }
                outfile << q->value(myModel->columnMap.value("database_name").value("Failsafes").indexOf("indication_method")).toString();
                outfile << tags.value("entry" + extraRowString).value("end");
                // ------------------------
                outfile << tags.value("entry_link_to_failsafe").value("start");
                if (!faults.isEmpty()) {
                    outfile << includedFaults.first();
                }
                outfile << tags.value("entry_link_to_failsafe").value("end");
                // ------------------------
                outfile << tags.value("entry" + extraRowString).value("start");
                if (extraRows > 0) {
                    outfile << extraRows;
                    outfile << tags.value("entry_extra_rows").value("middle");
                }
                outfile << tags.value("random_list").value("start");
                for (auto e : functionalImpacts) {
                    outfile << tags.value("list_item").value("start");
                    outfile << e.trimmed();
                    outfile << tags.value("list_item").value("end");
                }
                outfile << tags.value("random_list").value("end");
                outfile << tags.value("entry" + extraRowString).value("end");
                // ------------------------
                outfile << tags.value("entry" + extraRowString).value("start");
                if (extraRows > 0) {
                    outfile << extraRows;
                    outfile << tags.value("entry_extra_rows").value("middle");
                }
                outfile << tags.value("numbered_list").value("start");
                for (auto e : automatedResponses) {
                    if (e.length() > 1) { // Both key words detected and parsed
                        outfile << tags.value("list_item").value("start");
                        outfile << tags.value("random_list_prefix").value("start");
                        outfile << tags.value("list_item").value("start");
                        outfile << tags.value("bold_text").value("start");
                        outfile << "Payload Deployed: ";
                        outfile << tags.value("bold_text").value("end");
                        outfile << e.first();
                        outfile << tags.value("list_item").value("end");
                        outfile << tags.value("list_item").value("start");
                        outfile << tags.value("bold_text").value("start");
                        outfile << "Payload Stowed: ";
                        outfile << tags.value("bold_text").value("end");
                        outfile << e.back();
                        outfile << tags.value("list_item").value("end");
                        outfile << tags.value("random_list_prefix").value("end");
                        outfile << tags.value("list_item").value("end");
                    }
                    else { // One or both key words not detected
                        outfile << tags.value("list_item").value("start");
                        outfile << e.first();
                        outfile << tags.value("list_item").value("end");
                    }
                }
                outfile << tags.value("numbered_list").value("end");
                outfile << tags.value("entry" + extraRowString).value("end");
                // ------------------------
                outfile << tags.value("row").value("end");

                // To acheive several cells in one for the linking of associated wcas, the 'morerows=' option is used with all
                // the 'entry' start tags for this row and extra rows are appended after with just the link information.
                for (int i = 1; i < includedFaults.length(); i++) {
                    outfile << tags.value("row").value("start");
                    outfile << tags.value("entry_link_to_failsafe").value("start");
                    outfile << includedFaults.at(i);
                    outfile << tags.value("entry_link_to_failsafe").value("end");
                    outfile << tags.value("row").value("end");
                }
                // ------ End of Row ------
            }
        }

        f.close();
    }
}

// Export the WCA Guidebook XML Interlocks file to the location chosen by the user.
// Like the other WCA Guidebook functions, this function takes a pointer to the query to use
// and the name of the release to use.
void Exporter::exportWcaGuidebookInterlocks(QWidget *parent, QSqlQuery *q, QString releaseName, int payload) {
    QString filename = QFileDialog::getSaveFileName(parent, "Save WCA Guidebook XML - Interlocks", "interlocks.txt", "Text File (*.txt)");

    if (!filename.isEmpty()) {    // Allows user to close out and not save a file
        qInfo() << "Exporting File";
        QFile f(filename);

        if (!f.open(QFile::WriteOnly | QFile::Text)) {
            qInfo() << "Error Writing File";
            f.close();
        }

        QTextStream outfile(&f);
        if (payload == 0) {
            q->exec("select * from " + releaseName + "_interlocks where configuration like '%UISS%' or configuration like '%Common Craft%'");
        }
        else if (payload == 1) {
            q->exec("select * from " + releaseName + "_interlocks where configuration like '%MH%' or configuration like '%Common Craft%'");
        }

        while (q->next()) { // Output a row for every fault
            QStringList faults = q->value(myModel->columnMap.value("database_name").value("Interlocks").indexOf("associated_wcas")).toString().replace("&", "&amp;").split(",", QString::SplitBehavior::SkipEmptyParts, Qt::CaseInsensitive);
            QVector<QString> includedFaults;
            int extraRows = faults.size() - 1;
            QString extraRowString = "";
            QString interlockCode = q->value(myModel->columnMap.value("database_name").value("Interlocks").indexOf("interlock_id")).toString();

            // Check whether each listed wca in associated_wcas is not deleted in the release currently being viewed
            for (int i = 0; i < faults.size(); i++) {
                if (myModel->checkFaultCode(faults[i], releaseName, "")) {
                    includedFaults.push_back(faults[i]);
                }
                else {
                    extraRows--;
                }
            }
            if (extraRows > 0) {
                extraRowString = "_extra_rows";
            }

            if (!interlockCode.isEmpty()) {
                // ----- Start of Row -----
                outfile << tags.value("row").value("start");
                // ------------------------
                outfile << tags.value("bold_entry_with_id" + extraRowString).value("start");
                outfile << interlockCode;
                if (extraRows > 0) {
                    outfile << tags.value("bold_entry_with_id_extra_rows").value("middle1");
                    outfile << extraRows;
                    outfile << tags.value("bold_entry_with_id_extra_rows").value("middle2");
                }
                else {
                    outfile << tags.value("bold_entry_with_id").value("middle");
                }
                outfile << interlockCode;
                outfile << tags.value("bold_entry_with_id" + extraRowString).value("end");
                // ------------------------
                outfile << tags.value("entry" + extraRowString).value("start");
                if (extraRows > 0) {
                    outfile << extraRows;
                    outfile << tags.value("entry_extra_rows").value("middle");
                }
                outfile << q->value(myModel->columnMap.value("database_name").value("Interlocks").indexOf("components")).toString().replace("&", "&amp;");
                outfile << tags.value("entry" + extraRowString).value("end");
                // ------------------------
                outfile << tags.value("entry" + extraRowString).value("start");
                if (extraRows > 0) {
                    outfile << extraRows;
                    outfile << tags.value("entry_extra_rows").value("middle");
                }
                outfile << q->value(myModel->columnMap.value("database_name").value("Interlocks").indexOf("interlock_description")).toString().replace("&", "&amp;");
                outfile << tags.value("entry" + extraRowString).value("end");
                // ------------------------
                outfile << tags.value("entry_link_to_failsafe").value("start");
                if (!faults.isEmpty()) {
                    outfile << includedFaults.first();
                }
                outfile << tags.value("entry_link_to_failsafe").value("end");
                // ------------------------
                outfile << tags.value("row").value("end");

                // To acheive several cells in one for the linking of associated wcas, the 'morerows=' option is used with all
                // the 'entry' start tags for this row and extra rows are appended after with just the link information.
                for (int i = 1; i < includedFaults.length(); i++) {
                    outfile << tags.value("row").value("start");
                    outfile << tags.value("entry_link_to_failsafe").value("start");
                    outfile << includedFaults.at(i);
                    outfile << tags.value("entry_link_to_failsafe").value("end");
                    outfile << tags.value("row").value("end");
                }
                // ------ End of Row ------
            }
        }

        f.close();
    }
}

void Exporter::exportWcaGuidebookSystemNotes(QWidget *parent, QSqlQuery *q, QString releaseName, int payload) {
    QString filename = QFileDialog::getSaveFileName(parent, "Save WCA Guidebook XML - System Notes", "systemnotes.txt", "Text File (*.txt)");

    if (!filename.isEmpty()) {    // Allows user to close out and not save a file
        qInfo() << "Exporting File";
        QFile f(filename);

        if (!f.open(QFile::WriteOnly | QFile::Text)) {
            qInfo() << "Error Writing File";
            f.close();
        }

        QTextStream outfile(&f);
        if (payload == 0) {
            q->exec("select * from " + releaseName + "_system_notes where configuration like '%UISS%' or configuration like '%Common Craft%'");
        }
        else if (payload == 1){
            q->exec("select * from " + releaseName + "_system_notes where configuration like '%MH%' or configuration like '%Common Craft%'");
        }

        while (q->next()) { // Output a row for every fault
            if (q->value(myModel->columnMap.value("database_name").value("System Notes").indexOf("fm_status")).toString().contains("active", Qt::CaseInsensitive)) {
                QStringList operators = q->value(myModel->columnMap.value("database_name").value("System Notes").indexOf("operator")).toString().replace("&", "&amp;").split("\n", QString::SkipEmptyParts, Qt::CaseInsensitive);
                QStringList configurations = q->value(myModel->columnMap.value("database_name").value("System Notes").indexOf("configuration")).toString().replace("&", "&amp;").split("\n", QString::SkipEmptyParts, Qt::CaseInsensitive);
                QStringList workarounds = q->value(myModel->columnMap.value("database_name").value("System Notes").indexOf("workarounds")).toString().replace("&", "&amp;").split(".", QString::SkipEmptyParts, Qt::CaseInsensitive);

                outfile << tags.value("row").value("start");

                outfile << tags.value("entry").value("start");
                outfile << tags.value("bold_text").value("start");
                outfile << q->value(myModel->columnMap.value("database_name").value("System Notes").indexOf("summary")).toString().replace("&", "&amp;");
                outfile << tags.value("bold_text").value("end");
                outfile << tags.value("entry").value("end");

                outfile << tags.value("entry").value("start");
                outfile << tags.value("random_list").value("start");
                for (QString e : operators) {
                    outfile << tags.value("list_item").value("start");
                    outfile << e;
                    outfile << tags.value("list_item").value("end");
                }
                outfile << tags.value("random_list").value("end");
                outfile << tags.value("entry").value("end");

                outfile << tags.value("entry").value("start");
                outfile << tags.value("numbered_list").value("start");
                for (QString e : workarounds) {
                    e = e.trimmed();

                    if (e.length() < 2) {
                        continue;
                    }

                    outfile << tags.value("list_item").value("start");
                    outfile << e;
                    outfile << tags.value("list_item").value("end");
                }
                outfile << tags.value("numbered_list").value("end");
                outfile << tags.value("entry").value("end");

                outfile << tags.value("entry").value("start");
                outfile << q->value(myModel->columnMap.value("database_name").value("System Notes").indexOf("external_notes")).toString().replace("&", "&amp;");
                outfile << tags.value("entry").value("end");

                outfile << tags.value("row").value("end");
            }
        }
        f.close();
    }
}

// Add the correct indentation, the given text, and a newline to the given string
void Exporter::addLine(QString line, QString * outfile) {
    for (int i = 0; i < currentIndent; i++) {
        *outfile += " ";
    }

    *outfile += line;
    *outfile += "\n";
}

void Exporter::increaseIndent() {
    currentIndent += 4;
}

void Exporter::decreaseIndent() {
    currentIndent -= 4;
}
