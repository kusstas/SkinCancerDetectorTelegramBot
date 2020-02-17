QT -= gui
QT += remoteobjects sql

CONFIG += c++14 console
CONFIG -= app_bundle
CONFIG += file_copies

DEFINES += QT_DEPRECATED_WARNINGS


include(thirdparty/telegrambot/telegrambotlib-qt.pri)

REPC_REPLICA += qremote/SkinCancerDetectorService.rep

HEADERS += \
    src/core/Bot.h \
    src/utils/Settings.h \
    src/utils/SettingsReader.h \
    src/utils/Translator.h

SOURCES += \
    src/core/Bot.cpp \
    src/main.cpp \
    src/utils/Settings.cpp \
    src/utils/SettingsReader.cpp \
    src/utils/Translator.cpp

INCLUDEPATH += src/

COPIES += resources_files

resources_files.files = $$files($$PWD/resources/*)
resources_files.path = $$OUT_PWD

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
