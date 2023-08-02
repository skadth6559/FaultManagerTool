QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    exporter.cpp \
    main.cpp \
    mainwindow.cpp \
    mymodel.cpp \
    qcustompushbutton.cpp \
    save_add.cpp \
    save_edits.cpp

HEADERS += \
    exporter.h \
    mainwindow.h \
    mymodel.h \
    qcustompushbutton.h \
    save_add.h \
    save_edits.h

FORMS += \
    mainwindow.ui \
    save_add.ui \
    save_edits.ui

RC_ICONS = icon_Jn2_icon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

