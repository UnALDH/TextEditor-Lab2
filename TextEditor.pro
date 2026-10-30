QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    codeeditor.cpp \
    aboutdialog.cpp \
    finddialog.cpp \
    replacedialog.cpp

HEADERS += \
    mainwindow.h \
    codeeditor.h \
    aboutdialog.h \
    finddialog.h \
    replacedialog.h

FORMS += \
    mainwindow.ui \
    aboutdialog.ui \
    finddialog.ui \
    replacedialog.ui

RESOURCES += \
    icons.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
