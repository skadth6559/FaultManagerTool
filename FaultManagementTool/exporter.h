#ifndef EXPORTER_H
#define EXPORTER_H

#include "mymodel.h"

class Exporter
{
private:
    int currentIndent;
    void increaseIndent();
    void decreaseIndent();
    void addLine(QString line, QString * outfile);
    MyModel *myModel;
    const QMap<QString, QMap<QString, QString>> tags {{"row", {{"start", "<row>"}, {"end", "</row>"}}},
                                                      {"entry", {{"start", "<entry><para>"}, {"end", "</para></entry>"}}},
                                                      {"entry_extra_rows", {{"start", R"(<entry morerows=")"}, {"middle", R"("><para>)"}, {"end", R"(</para></entry>)"}}},
                                                      {"entry_link_to_failsafe", {{"start", R"(<entry><para><dmRef referredFragment=")"}, {"end", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="HP0" subSystemCode="0" subSubSystemCode="2" assyCode="0000" disassyCode="00" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="A" /></dmRefIdent></dmRef></para></entry>)"}}},
                                                      {"entry_with_id", {{"start", R"(<entry id=")"}, {"middle", R"("><para>)"}, {"end", R"(</para></entry>)"}}},
                                                      {"bold_entry", {{"start", "<entry><para><emphasis emphasisType=\"em01\">"}, {"end", "</emphasis></para></entry>"}}},
                                                      {"bold_entry_extra_rows", {{"start", "<entry morerows=\""}, {"middle", "\"><para><emphasis emphasisType=\"em01\">"}, {"end", "</emphasis></para></entry>"}}},
                                                      {"bold_text", {{"start", "<emphasis emphasisType=\"em01\">"}, {"end", "</emphasis>"}}},
                                                      {"bold_entry_with_id", {{"start", R"(<entry id=")"}, {"middle", R"("><para><emphasis emphasisType="em01">)"}, {"end", R"(</emphasis></para></entry>)"}}},
                                                      {"bold_entry_with_id_extra_rows", {{"start", R"(<entry id=")"}, {"middle1", R"(" morerows=")"},{"middle2", R"("><para><emphasis emphasisType="em01">)"}, {"end", R"(</emphasis></para></entry>)"}}},
                                                      {"entry_with_list", {{"start", "<entry><para><randomList><listItem><para>"}, {"end", "</para>	</listItem></randomList></para></entry>"}}},
                                                      {"entry_with_numbered_list", {{"start", R"(<entry><para><sequentialList>)"}, {"end", R"(</sequentialList></para></entry>)"}}},
                                                      {"numbered_list", {{"start", "<sequentialList>"}, {"end", "</sequentialList>"}}},
                                                      {"random_list", {{"start", "<randomList>"}, {"end", "</randomList>"}}},
                                                      {"random_list_prefix", {{"start", R"(<randomList listItemPrefix="pf01">)"}, {"end", R"(</randomList>)"}}},
                                                      {"list_item", {{"start", "<listItem><para>"}, {"end", "</para>	</listItem>"}}},
                                                      {"ref", {{"start", "<dmRef referredFragment=\""}}},
                                                      {"offboard_lcs_mhp_20", {{"warning", R"("><dmRefIdent> <dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="08" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                                {"caution", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="09" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>")"},
                                                                                {"advisory", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="10" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"}}},
                                                      {"lcs_lr_mhp_20", {{"warning", R"("><dmRefIdent> <dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="14" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                        {"caution", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="15" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                        {"advisory", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="16" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"}}},
                                                      {"onboard_lcs_mhp_20", {{"warning", R"("><dmRefIdent> <dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="11" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                        {"caution", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="12" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                        {"advisory", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="13" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"}}},
                                                      {"mine_warfare_evaluator_mhp_20", {{"warning", R"("><dmRefIdent> <dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="20" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                                        {"caution", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="21" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                                        {"advisory", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="22" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"}}},
                                                      {"header", {{"onboard_lcs", R"(<emphasis>Onboard LCS:</emphasis>)"},
                                                                    {"underway", R"(<emphasis>Underway:</emphasis>)"},
                                                                    {"lcs_lr", R"(<emphasis>LCS L&amp;R:</emphasis>)"}}},
                                                      {"offboard_lcs_iss", {{"warning", R"("><dmRefIdent> <dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="02" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                            {"caution", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="03" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                            {"advisory", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="04" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"}}},
                                                      {"lcs_lr_iss", {{"warning", R"("><dmRefIdent> <dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="17" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                        {"caution", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="18" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                        {"advisory", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="19" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"}}},
                                                      {"onboard_lcs_iss", {{"warning", R"("><dmRefIdent> <dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="26" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                            {"caution", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="06" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                            {"advisory", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="07" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"}}},
                                                      {"mine_warfare_evaluator_iss", {{"warning", R"("><dmRefIdent> <dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="23" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                                        {"caution", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="24" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"},
                                                                                        {"advisory", R"("><dmRefIdent><dmCode modelIdentCode="UISS" systemDiffCode="A" systemCode="H00" subSystemCode="0" subSubSystemCode="0" assyCode="0000" disassyCode="25" disassyCodeVariant="A" infoCode="441" infoCodeVariant="A" itemLocationCode="D" /></dmRefIdent></dmRef>)"}}}};
    QString severities[3] = {"warning", "caution", "advisory"};

    QString payloads[2] = {"uiss", "mh"};

public:
    enum datatype {WCAS, FAILSAFES, INTERLOCKS};
    Exporter(MyModel *model);
    void exportFaultConstants(QString version, QWidget * parent, int payload);
    void exportFaultStringLibh(QString version, QWidget * parent, int payload);
    void exportFaultStringLibcpp(QString version, QWidget * parent, int payload);
    void exportToCSV(QString version, QWidget * parent, QString category);
    void exportWcaGuidebookIndex(QWidget * parent, QSqlQuery *q, QString releaseName, int payload);
    void exportWcaGuidebookOnboardLCS(QWidget *parent, QSqlQuery *q, QString releaseName, int payload);
    void exportWcaGuidebookMineWarfareEval(QWidget *parent, QSqlQuery *q, QString releaseName, int payload);
    void exportWcaGuidebookLR(QWidget *parent, QSqlQuery *q, QString releaseName, int payload);
    void exportWcaGuidebookOffboardLCS(QWidget *parent, QSqlQuery *q, QString releaseName, int payload);
    void exportWcaGuidebookFailsafes(QWidget *parent, QSqlQuery *q, QString releaseName, int payload);
    void exportWcaGuidebookInterlocks(QWidget *parent, QSqlQuery *q, QString releaseName, int payload);
    void exportWcaGuidebookSystemNotes(QWidget *parent, QSqlQuery *q, QString releaseName, int payload);
};

#endif // EXPORTER_H
