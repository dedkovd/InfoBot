#-------------------------------------------------
#
# Project created by QtCreator 2011-08-16T19:02:06
#
#-------------------------------------------------

QT       += widgets network

TARGET = InfoBot
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

FORMS    += dialog.ui

RESOURCES += \
    res.qrc

OTHER_FILES += \
    infobot.ts

LIBS += -L../build-qxmpp-Desktop_Qt_5_5_1_GCC_64bit-Release/src/ -lqxmpp0

INCLUDEPATH += ../qxmpp/src/client \
                ../qxmpp/src/base

TRANSLATIONS += infobot.ts

OTHER_FILES += \
    infobot.ts \
    infobot.rc

RC_FILE += infobot.rc
