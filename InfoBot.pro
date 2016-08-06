#-------------------------------------------------
#
# Project created by QtCreator 2011-08-16T19:02:06
#
#-------------------------------------------------

QT       += widgets network

TARGET = InfoBot
TEMPLATE = app


SOURCES += src/main.cpp\
        src/dialog.cpp

HEADERS  += src/dialog.h

FORMS    += ui/dialog.ui

RESOURCES += \
    res/res.qrc

LIBS += -L../../build-qxmpp-Desktop_Qt_5_5_1_GCC_64bit-Debug/src/ -lqxmpp_d

INCLUDEPATH += ../../qxmpp/src/client \
                ../../qxmpp/src/base

TRANSLATIONS += i18n/infobot.ts

OTHER_FILES += \
    i18n/infobot.ts \
    res/infobot.rc

RC_FILE += res/infobot.rc
