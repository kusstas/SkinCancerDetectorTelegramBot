QT -= gui
QT += remoteobjects sql

CONFIG += c++17 console
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

COPIES += setttings_file translation_files

setttings_file.files = $$PWD/resources/settings.json
setttings_file.path = $$OUT_PWD

translation_files.files = $$files($$PWD/resources/translations/*)
translation_files.path = $$OUT_PWD/translations/

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
