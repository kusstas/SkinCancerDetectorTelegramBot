QT -= gui
QT += remoteobjects sql

CONFIG += c++14 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

include(third-party/telegrambot/telegrambotlib-qt.pri)

REPC_REPLICA += qremote/SkinCancerDetectorService.rep

HEADERS += \
    src/core/Bot.h \
    src/utils/Settings.h \
    src/utils/SettingsReader.h

SOURCES += \
    src/core/Bot.cpp \
    src/main.cpp \
    src/utils/Settings.cpp \
    src/utils/SettingsReader.cpp

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
