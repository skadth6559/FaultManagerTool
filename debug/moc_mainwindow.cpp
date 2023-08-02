/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FaultManagementTool/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[114];
    char stringdata0[2647];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 16), // "showDeletePrompt"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 29), // "on_applyFiltersButton_clicked"
QT_MOC_LITERAL(4, 59, 30), // "on_cancelFiltersButton_clicked"
QT_MOC_LITERAL(5, 90, 25), // "on_newFaultButton_clicked"
QT_MOC_LITERAL(6, 116, 21), // "on_editButton_clicked"
QT_MOC_LITERAL(7, 138, 11), // "deleteFault"
QT_MOC_LITERAL(8, 150, 30), // "on_createReleaseButton_clicked"
QT_MOC_LITERAL(9, 181, 30), // "on_cancelReleaseButton_clicked"
QT_MOC_LITERAL(10, 212, 26), // "on_addFilterButton_clicked"
QT_MOC_LITERAL(11, 239, 10), // "cancelEdit"
QT_MOC_LITERAL(12, 250, 23), // "on_searchButton_clicked"
QT_MOC_LITERAL(13, 274, 29), // "on_searchLineEdit_textChanged"
QT_MOC_LITERAL(14, 304, 28), // "on_versionComboBox_activated"
QT_MOC_LITERAL(15, 333, 4), // "arg1"
QT_MOC_LITERAL(16, 338, 10), // "closeEvent"
QT_MOC_LITERAL(17, 349, 11), // "setHomePage"
QT_MOC_LITERAL(18, 361, 17), // "updateSaveButtons"
QT_MOC_LITERAL(19, 379, 17), // "setChangesBrowser"
QT_MOC_LITERAL(20, 397, 13), // "QTextBrowser*"
QT_MOC_LITERAL(21, 411, 7), // "changes"
QT_MOC_LITERAL(22, 419, 27), // "on_nextInfoButton_2_clicked"
QT_MOC_LITERAL(23, 447, 29), // "on_nextMCMUSVButton_2_clicked"
QT_MOC_LITERAL(24, 477, 26), // "on_nextWCAButton_2_clicked"
QT_MOC_LITERAL(25, 504, 31), // "on_nextResponseButton_2_clicked"
QT_MOC_LITERAL(26, 536, 28), // "on_nextOtherButton_2_clicked"
QT_MOC_LITERAL(27, 565, 26), // "on_backWCAButton_2_clicked"
QT_MOC_LITERAL(28, 592, 29), // "on_backMCMUSVButton_2_clicked"
QT_MOC_LITERAL(29, 622, 31), // "on_backResponseButton_2_clicked"
QT_MOC_LITERAL(30, 654, 28), // "on_backOtherButton_2_clicked"
QT_MOC_LITERAL(31, 683, 32), // "on_backChangeLogButton_2_clicked"
QT_MOC_LITERAL(32, 716, 29), // "on_cancelInfoButton_2_clicked"
QT_MOC_LITERAL(33, 746, 31), // "on_cancelMCMUSVButton_2_clicked"
QT_MOC_LITERAL(34, 778, 28), // "on_cancelWCAButton_2_clicked"
QT_MOC_LITERAL(35, 807, 33), // "on_cancelResponseButton_2_cli..."
QT_MOC_LITERAL(36, 841, 30), // "on_cancelOtherButton_2_clicked"
QT_MOC_LITERAL(37, 872, 32), // "on_cancelChangeLogButton_clicked"
QT_MOC_LITERAL(38, 905, 27), // "on_editMCMUSVButton_clicked"
QT_MOC_LITERAL(39, 933, 24), // "on_editWCAButton_clicked"
QT_MOC_LITERAL(40, 958, 29), // "on_editResponseButton_clicked"
QT_MOC_LITERAL(41, 988, 26), // "on_editOtherButton_clicked"
QT_MOC_LITERAL(42, 1015, 18), // "createNewFaultPage"
QT_MOC_LITERAL(43, 1034, 14), // "createEditPage"
QT_MOC_LITERAL(44, 1049, 8), // "readonly"
QT_MOC_LITERAL(45, 1058, 14), // "createViewPage"
QT_MOC_LITERAL(46, 1073, 8), // "released"
QT_MOC_LITERAL(47, 1082, 14), // "hideEditFaults"
QT_MOC_LITERAL(48, 1097, 7), // "visible"
QT_MOC_LITERAL(49, 1105, 15), // "ghostEditFaults"
QT_MOC_LITERAL(50, 1121, 5), // "ghost"
QT_MOC_LITERAL(51, 1127, 15), // "ghostSaveFaults"
QT_MOC_LITERAL(52, 1143, 18), // "makeReadOnlyFields"
QT_MOC_LITERAL(53, 1162, 11), // "clearFields"
QT_MOC_LITERAL(54, 1174, 23), // "on_faultIDLE_textEdited"
QT_MOC_LITERAL(55, 1198, 4), // "text"
QT_MOC_LITERAL(56, 1203, 21), // "on_groupLE_textEdited"
QT_MOC_LITERAL(57, 1225, 23), // "on_groupIDLE_textEdited"
QT_MOC_LITERAL(58, 1249, 24), // "on_enumNameLE_textEdited"
QT_MOC_LITERAL(59, 1274, 25), // "on_faultCodeLE_textEdited"
QT_MOC_LITERAL(60, 1300, 22), // "on_pcrNumLE_textEdited"
QT_MOC_LITERAL(61, 1323, 23), // "on_changeeLE_textEdited"
QT_MOC_LITERAL(62, 1347, 26), // "on_faultGroupLE_textEdited"
QT_MOC_LITERAL(63, 1374, 26), // "on_failsafeMHLE_textEdited"
QT_MOC_LITERAL(64, 1401, 28), // "on_failsafeUISSLE_textEdited"
QT_MOC_LITERAL(65, 1430, 27), // "on_interlockMHLE_textEdited"
QT_MOC_LITERAL(66, 1458, 29), // "on_interlockUISSLE_textEdited"
QT_MOC_LITERAL(67, 1488, 10), // "queryField"
QT_MOC_LITERAL(68, 1499, 9), // "fieldname"
QT_MOC_LITERAL(69, 1509, 17), // "updateHighlightLE"
QT_MOC_LITERAL(70, 1527, 10), // "QLineEdit*"
QT_MOC_LITERAL(71, 1538, 2), // "le"
QT_MOC_LITERAL(72, 1541, 36), // "on_implementationNotesPT_text..."
QT_MOC_LITERAL(73, 1578, 27), // "on_wcaMessagePT_textChanged"
QT_MOC_LITERAL(74, 1606, 27), // "on_wcaTriggerPT_textChanged"
QT_MOC_LITERAL(75, 1634, 30), // "on_failureImpactPT_textChanged"
QT_MOC_LITERAL(76, 1665, 32), // "on_failsafeSummaryPT_textChanged"
QT_MOC_LITERAL(77, 1698, 34), // "on_operatorActionsMHPT_textCh..."
QT_MOC_LITERAL(78, 1733, 36), // "on_operatorActionsUISSPT_text..."
QT_MOC_LITERAL(79, 1770, 40), // "on_correctiveMaintenanceMHPT_..."
QT_MOC_LITERAL(80, 1811, 42), // "on_correctiveMaintenanceUISSP..."
QT_MOC_LITERAL(81, 1854, 22), // "on_notesPT_textChanged"
QT_MOC_LITERAL(82, 1877, 17), // "updateHighlightPT"
QT_MOC_LITERAL(83, 1895, 15), // "QPlainTextEdit*"
QT_MOC_LITERAL(84, 1911, 2), // "pt"
QT_MOC_LITERAL(85, 1914, 17), // "updateHighlightCB"
QT_MOC_LITERAL(86, 1932, 10), // "QComboBox*"
QT_MOC_LITERAL(87, 1943, 2), // "cb"
QT_MOC_LITERAL(88, 1946, 33), // "on_fmStatusCB_currentIndexCha..."
QT_MOC_LITERAL(89, 1980, 33), // "on_includedCB_currentIndexCha..."
QT_MOC_LITERAL(90, 2014, 33), // "on_exportMHCB_currentIndexCha..."
QT_MOC_LITERAL(91, 2048, 39), // "on_controlStationCB_currentIn..."
QT_MOC_LITERAL(92, 2088, 36), // "on_wcaSeverityCB_currentIndex..."
QT_MOC_LITERAL(93, 2125, 34), // "on_subsystemCB_currentIndexCh..."
QT_MOC_LITERAL(94, 2160, 36), // "on_failsafeLocCB_currentIndex..."
QT_MOC_LITERAL(95, 2197, 34), // "on_faultTypeCB_currentIndexCh..."
QT_MOC_LITERAL(96, 2232, 36), // "on_payloadTypeCB_currentIndex..."
QT_MOC_LITERAL(97, 2269, 27), // "on_saveMCMUSVButton_clicked"
QT_MOC_LITERAL(98, 2297, 24), // "on_saveWCAButton_clicked"
QT_MOC_LITERAL(99, 2322, 29), // "on_saveResponseButton_clicked"
QT_MOC_LITERAL(100, 2352, 26), // "on_saveOtherButton_clicked"
QT_MOC_LITERAL(101, 2379, 14), // "showInfoPrompt"
QT_MOC_LITERAL(102, 2394, 12), // "saveNewFault"
QT_MOC_LITERAL(103, 2407, 13), // "saveEditFault"
QT_MOC_LITERAL(104, 2421, 30), // "on_actionNew_Release_triggered"
QT_MOC_LITERAL(105, 2452, 12), // "markAsLocked"
QT_MOC_LITERAL(106, 2465, 33), // "on_actionDelete_Release_trigg..."
QT_MOC_LITERAL(107, 2499, 25), // "on_actionRename_triggered"
QT_MOC_LITERAL(108, 2525, 13), // "deleteRelease"
QT_MOC_LITERAL(109, 2539, 12), // "currentTable"
QT_MOC_LITERAL(110, 2552, 8), // "isRename"
QT_MOC_LITERAL(111, 2561, 31), // "on_actionEdit_Release_triggered"
QT_MOC_LITERAL(112, 2593, 17), // "adjustReleaseMenu"
QT_MOC_LITERAL(113, 2611, 35) // "on_actionMark_as_Released_tri..."

    },
    "MainWindow\0showDeletePrompt\0\0"
    "on_applyFiltersButton_clicked\0"
    "on_cancelFiltersButton_clicked\0"
    "on_newFaultButton_clicked\0"
    "on_editButton_clicked\0deleteFault\0"
    "on_createReleaseButton_clicked\0"
    "on_cancelReleaseButton_clicked\0"
    "on_addFilterButton_clicked\0cancelEdit\0"
    "on_searchButton_clicked\0"
    "on_searchLineEdit_textChanged\0"
    "on_versionComboBox_activated\0arg1\0"
    "closeEvent\0setHomePage\0updateSaveButtons\0"
    "setChangesBrowser\0QTextBrowser*\0changes\0"
    "on_nextInfoButton_2_clicked\0"
    "on_nextMCMUSVButton_2_clicked\0"
    "on_nextWCAButton_2_clicked\0"
    "on_nextResponseButton_2_clicked\0"
    "on_nextOtherButton_2_clicked\0"
    "on_backWCAButton_2_clicked\0"
    "on_backMCMUSVButton_2_clicked\0"
    "on_backResponseButton_2_clicked\0"
    "on_backOtherButton_2_clicked\0"
    "on_backChangeLogButton_2_clicked\0"
    "on_cancelInfoButton_2_clicked\0"
    "on_cancelMCMUSVButton_2_clicked\0"
    "on_cancelWCAButton_2_clicked\0"
    "on_cancelResponseButton_2_clicked\0"
    "on_cancelOtherButton_2_clicked\0"
    "on_cancelChangeLogButton_clicked\0"
    "on_editMCMUSVButton_clicked\0"
    "on_editWCAButton_clicked\0"
    "on_editResponseButton_clicked\0"
    "on_editOtherButton_clicked\0"
    "createNewFaultPage\0createEditPage\0"
    "readonly\0createViewPage\0released\0"
    "hideEditFaults\0visible\0ghostEditFaults\0"
    "ghost\0ghostSaveFaults\0makeReadOnlyFields\0"
    "clearFields\0on_faultIDLE_textEdited\0"
    "text\0on_groupLE_textEdited\0"
    "on_groupIDLE_textEdited\0"
    "on_enumNameLE_textEdited\0"
    "on_faultCodeLE_textEdited\0"
    "on_pcrNumLE_textEdited\0on_changeeLE_textEdited\0"
    "on_faultGroupLE_textEdited\0"
    "on_failsafeMHLE_textEdited\0"
    "on_failsafeUISSLE_textEdited\0"
    "on_interlockMHLE_textEdited\0"
    "on_interlockUISSLE_textEdited\0queryField\0"
    "fieldname\0updateHighlightLE\0QLineEdit*\0"
    "le\0on_implementationNotesPT_textChanged\0"
    "on_wcaMessagePT_textChanged\0"
    "on_wcaTriggerPT_textChanged\0"
    "on_failureImpactPT_textChanged\0"
    "on_failsafeSummaryPT_textChanged\0"
    "on_operatorActionsMHPT_textChanged\0"
    "on_operatorActionsUISSPT_textChanged\0"
    "on_correctiveMaintenanceMHPT_textChanged\0"
    "on_correctiveMaintenanceUISSPT_textChanged\0"
    "on_notesPT_textChanged\0updateHighlightPT\0"
    "QPlainTextEdit*\0pt\0updateHighlightCB\0"
    "QComboBox*\0cb\0on_fmStatusCB_currentIndexChanged\0"
    "on_includedCB_currentIndexChanged\0"
    "on_exportMHCB_currentIndexChanged\0"
    "on_controlStationCB_currentIndexChanged\0"
    "on_wcaSeverityCB_currentIndexChanged\0"
    "on_subsystemCB_currentIndexChanged\0"
    "on_failsafeLocCB_currentIndexChanged\0"
    "on_faultTypeCB_currentIndexChanged\0"
    "on_payloadTypeCB_currentIndexChanged\0"
    "on_saveMCMUSVButton_clicked\0"
    "on_saveWCAButton_clicked\0"
    "on_saveResponseButton_clicked\0"
    "on_saveOtherButton_clicked\0showInfoPrompt\0"
    "saveNewFault\0saveEditFault\0"
    "on_actionNew_Release_triggered\0"
    "markAsLocked\0on_actionDelete_Release_triggered\0"
    "on_actionRename_triggered\0deleteRelease\0"
    "currentTable\0isRename\0"
    "on_actionEdit_Release_triggered\0"
    "adjustReleaseMenu\0on_actionMark_as_Released_triggered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      95,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  489,    2, 0x08 /* Private */,
       3,    0,  490,    2, 0x08 /* Private */,
       4,    0,  491,    2, 0x08 /* Private */,
       5,    0,  492,    2, 0x08 /* Private */,
       6,    0,  493,    2, 0x08 /* Private */,
       7,    0,  494,    2, 0x08 /* Private */,
       8,    0,  495,    2, 0x08 /* Private */,
       9,    0,  496,    2, 0x08 /* Private */,
      10,    0,  497,    2, 0x08 /* Private */,
      11,    0,  498,    2, 0x08 /* Private */,
      12,    0,  499,    2, 0x08 /* Private */,
      13,    0,  500,    2, 0x08 /* Private */,
      14,    1,  501,    2, 0x08 /* Private */,
      16,    0,  504,    2, 0x08 /* Private */,
      17,    0,  505,    2, 0x08 /* Private */,
      18,    0,  506,    2, 0x08 /* Private */,
      19,    1,  507,    2, 0x08 /* Private */,
      22,    0,  510,    2, 0x08 /* Private */,
      23,    0,  511,    2, 0x08 /* Private */,
      24,    0,  512,    2, 0x08 /* Private */,
      25,    0,  513,    2, 0x08 /* Private */,
      26,    0,  514,    2, 0x08 /* Private */,
      27,    0,  515,    2, 0x08 /* Private */,
      28,    0,  516,    2, 0x08 /* Private */,
      29,    0,  517,    2, 0x08 /* Private */,
      30,    0,  518,    2, 0x08 /* Private */,
      31,    0,  519,    2, 0x08 /* Private */,
      32,    0,  520,    2, 0x08 /* Private */,
      33,    0,  521,    2, 0x08 /* Private */,
      34,    0,  522,    2, 0x08 /* Private */,
      35,    0,  523,    2, 0x08 /* Private */,
      36,    0,  524,    2, 0x08 /* Private */,
      37,    0,  525,    2, 0x08 /* Private */,
      38,    0,  526,    2, 0x08 /* Private */,
      39,    0,  527,    2, 0x08 /* Private */,
      40,    0,  528,    2, 0x08 /* Private */,
      41,    0,  529,    2, 0x08 /* Private */,
      42,    0,  530,    2, 0x08 /* Private */,
      43,    1,  531,    2, 0x08 /* Private */,
      45,    1,  534,    2, 0x08 /* Private */,
      47,    1,  537,    2, 0x08 /* Private */,
      49,    1,  540,    2, 0x08 /* Private */,
      51,    1,  543,    2, 0x08 /* Private */,
      52,    1,  546,    2, 0x08 /* Private */,
      53,    0,  549,    2, 0x08 /* Private */,
      54,    1,  550,    2, 0x08 /* Private */,
      56,    1,  553,    2, 0x08 /* Private */,
      57,    1,  556,    2, 0x08 /* Private */,
      58,    1,  559,    2, 0x08 /* Private */,
      59,    1,  562,    2, 0x08 /* Private */,
      60,    1,  565,    2, 0x08 /* Private */,
      61,    1,  568,    2, 0x08 /* Private */,
      62,    1,  571,    2, 0x08 /* Private */,
      63,    1,  574,    2, 0x08 /* Private */,
      64,    1,  577,    2, 0x08 /* Private */,
      65,    1,  580,    2, 0x08 /* Private */,
      66,    1,  583,    2, 0x08 /* Private */,
      67,    1,  586,    2, 0x08 /* Private */,
      69,    3,  589,    2, 0x08 /* Private */,
      72,    0,  596,    2, 0x08 /* Private */,
      73,    0,  597,    2, 0x08 /* Private */,
      74,    0,  598,    2, 0x08 /* Private */,
      75,    0,  599,    2, 0x08 /* Private */,
      76,    0,  600,    2, 0x08 /* Private */,
      77,    0,  601,    2, 0x08 /* Private */,
      78,    0,  602,    2, 0x08 /* Private */,
      79,    0,  603,    2, 0x08 /* Private */,
      80,    0,  604,    2, 0x08 /* Private */,
      81,    0,  605,    2, 0x08 /* Private */,
      82,    3,  606,    2, 0x08 /* Private */,
      85,    3,  613,    2, 0x08 /* Private */,
      88,    0,  620,    2, 0x08 /* Private */,
      89,    0,  621,    2, 0x08 /* Private */,
      90,    0,  622,    2, 0x08 /* Private */,
      91,    0,  623,    2, 0x08 /* Private */,
      92,    0,  624,    2, 0x08 /* Private */,
      93,    0,  625,    2, 0x08 /* Private */,
      94,    0,  626,    2, 0x08 /* Private */,
      95,    0,  627,    2, 0x08 /* Private */,
      96,    0,  628,    2, 0x08 /* Private */,
      97,    0,  629,    2, 0x08 /* Private */,
      98,    0,  630,    2, 0x08 /* Private */,
      99,    0,  631,    2, 0x08 /* Private */,
     100,    0,  632,    2, 0x08 /* Private */,
     101,    0,  633,    2, 0x08 /* Private */,
     102,    0,  634,    2, 0x08 /* Private */,
     103,    0,  635,    2, 0x08 /* Private */,
     104,    0,  636,    2, 0x08 /* Private */,
     105,    0,  637,    2, 0x08 /* Private */,
     106,    0,  638,    2, 0x08 /* Private */,
     107,    0,  639,    2, 0x08 /* Private */,
     108,    2,  640,    2, 0x08 /* Private */,
     111,    0,  645,    2, 0x08 /* Private */,
     112,    1,  646,    2, 0x08 /* Private */,
     113,    0,  649,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   44,
    QMetaType::Void, QMetaType::Bool,   46,
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void, QMetaType::Bool,   50,
    QMetaType::Void, QMetaType::Bool,   50,
    QMetaType::Void, QMetaType::Bool,   44,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::QString, QMetaType::QString,   68,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, 0x80000000 | 70,   55,   68,   71,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, 0x80000000 | 83,   55,   68,   84,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, 0x80000000 | 86,   55,   68,   87,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::QString, QMetaType::QString, QMetaType::Bool,  109,  110,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   44,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showDeletePrompt(); break;
        case 1: _t->on_applyFiltersButton_clicked(); break;
        case 2: _t->on_cancelFiltersButton_clicked(); break;
        case 3: _t->on_newFaultButton_clicked(); break;
        case 4: _t->on_editButton_clicked(); break;
        case 5: _t->deleteFault(); break;
        case 6: _t->on_createReleaseButton_clicked(); break;
        case 7: _t->on_cancelReleaseButton_clicked(); break;
        case 8: _t->on_addFilterButton_clicked(); break;
        case 9: _t->cancelEdit(); break;
        case 10: _t->on_searchButton_clicked(); break;
        case 11: _t->on_searchLineEdit_textChanged(); break;
        case 12: _t->on_versionComboBox_activated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->closeEvent(); break;
        case 14: _t->setHomePage(); break;
        case 15: { bool _r = _t->updateSaveButtons();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 16: _t->setChangesBrowser((*reinterpret_cast< QTextBrowser*(*)>(_a[1]))); break;
        case 17: _t->on_nextInfoButton_2_clicked(); break;
        case 18: _t->on_nextMCMUSVButton_2_clicked(); break;
        case 19: _t->on_nextWCAButton_2_clicked(); break;
        case 20: _t->on_nextResponseButton_2_clicked(); break;
        case 21: _t->on_nextOtherButton_2_clicked(); break;
        case 22: _t->on_backWCAButton_2_clicked(); break;
        case 23: _t->on_backMCMUSVButton_2_clicked(); break;
        case 24: _t->on_backResponseButton_2_clicked(); break;
        case 25: _t->on_backOtherButton_2_clicked(); break;
        case 26: _t->on_backChangeLogButton_2_clicked(); break;
        case 27: _t->on_cancelInfoButton_2_clicked(); break;
        case 28: _t->on_cancelMCMUSVButton_2_clicked(); break;
        case 29: _t->on_cancelWCAButton_2_clicked(); break;
        case 30: _t->on_cancelResponseButton_2_clicked(); break;
        case 31: _t->on_cancelOtherButton_2_clicked(); break;
        case 32: _t->on_cancelChangeLogButton_clicked(); break;
        case 33: _t->on_editMCMUSVButton_clicked(); break;
        case 34: _t->on_editWCAButton_clicked(); break;
        case 35: _t->on_editResponseButton_clicked(); break;
        case 36: _t->on_editOtherButton_clicked(); break;
        case 37: _t->createNewFaultPage(); break;
        case 38: _t->createEditPage((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 39: _t->createViewPage((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 40: _t->hideEditFaults((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 41: _t->ghostEditFaults((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 42: _t->ghostSaveFaults((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 43: _t->makeReadOnlyFields((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 44: _t->clearFields(); break;
        case 45: _t->on_faultIDLE_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 46: _t->on_groupLE_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 47: _t->on_groupIDLE_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 48: _t->on_enumNameLE_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 49: _t->on_faultCodeLE_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 50: _t->on_pcrNumLE_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 51: _t->on_changeeLE_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 52: _t->on_faultGroupLE_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 53: _t->on_failsafeMHLE_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 54: _t->on_failsafeUISSLE_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 55: _t->on_interlockMHLE_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 56: _t->on_interlockUISSLE_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 57: { QString _r = _t->queryField((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 58: _t->updateHighlightLE((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QLineEdit*(*)>(_a[3]))); break;
        case 59: _t->on_implementationNotesPT_textChanged(); break;
        case 60: _t->on_wcaMessagePT_textChanged(); break;
        case 61: _t->on_wcaTriggerPT_textChanged(); break;
        case 62: _t->on_failureImpactPT_textChanged(); break;
        case 63: _t->on_failsafeSummaryPT_textChanged(); break;
        case 64: _t->on_operatorActionsMHPT_textChanged(); break;
        case 65: _t->on_operatorActionsUISSPT_textChanged(); break;
        case 66: _t->on_correctiveMaintenanceMHPT_textChanged(); break;
        case 67: _t->on_correctiveMaintenanceUISSPT_textChanged(); break;
        case 68: _t->on_notesPT_textChanged(); break;
        case 69: _t->updateHighlightPT((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QPlainTextEdit*(*)>(_a[3]))); break;
        case 70: _t->updateHighlightCB((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QComboBox*(*)>(_a[3]))); break;
        case 71: _t->on_fmStatusCB_currentIndexChanged(); break;
        case 72: _t->on_includedCB_currentIndexChanged(); break;
        case 73: _t->on_exportMHCB_currentIndexChanged(); break;
        case 74: _t->on_controlStationCB_currentIndexChanged(); break;
        case 75: _t->on_wcaSeverityCB_currentIndexChanged(); break;
        case 76: _t->on_subsystemCB_currentIndexChanged(); break;
        case 77: _t->on_failsafeLocCB_currentIndexChanged(); break;
        case 78: _t->on_faultTypeCB_currentIndexChanged(); break;
        case 79: _t->on_payloadTypeCB_currentIndexChanged(); break;
        case 80: _t->on_saveMCMUSVButton_clicked(); break;
        case 81: _t->on_saveWCAButton_clicked(); break;
        case 82: _t->on_saveResponseButton_clicked(); break;
        case 83: _t->on_saveOtherButton_clicked(); break;
        case 84: _t->showInfoPrompt(); break;
        case 85: _t->saveNewFault(); break;
        case 86: _t->saveEditFault(); break;
        case 87: _t->on_actionNew_Release_triggered(); break;
        case 88: _t->markAsLocked(); break;
        case 89: _t->on_actionDelete_Release_triggered(); break;
        case 90: _t->on_actionRename_triggered(); break;
        case 91: { QString _r = _t->deleteRelease((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 92: _t->on_actionEdit_Release_triggered(); break;
        case 93: _t->adjustReleaseMenu((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 94: _t->on_actionMark_as_Released_triggered(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 16:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTextBrowser* >(); break;
            }
            break;
        case 69:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QPlainTextEdit* >(); break;
            }
            break;
        case 70:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QComboBox* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 95)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 95;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 95)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 95;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
