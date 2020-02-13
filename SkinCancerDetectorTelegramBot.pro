QT -= gui
QT += remoteobjects

CONFIG += c++14 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

include(third-party/telegrambot/telegrambotlib-qt.pri)

SOURCES += \
    src/main.cpp

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
