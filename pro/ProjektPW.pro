QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/customtreeview.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/masterpwdpromptdialog.cpp \
    src/plaintextdelegate.cpp \
    src/pwdgeneratordialog.cpp \
    src/pwdmaskdelegate.cpp \
    src/simplecrypt.cpp \
    src/utils.cpp

HEADERS += \
    headers/customtreeview.h \
    headers/mainwindow.h \
    headers/masterpwdpromptdialog.h \
    headers/plaintextdelegate.h \
    headers/pwdgeneratordialog.h \
    headers/pwdmaskdelegate.h \
    headers/simplecrypt.h \
    headers/utils.h

FORMS += \
    ui/mainwindow.ui \
    ui/masterpwdpromptdialog.ui \
    ui/pwdgeneratordialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

RC_FILE = icons/bipass.rc
