
DEFINES += REVIEWBOARD_LIBRARY

# Qt Creator linking

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\Nokia\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/Nokia/qtcreator" or "~/.local/share/Nokia/qtcreator" on Linux
##    "~/Library/Application Support/Nokia/Qt Creator" on Mac
#USE_USER_DESTDIR = yes

## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/home/dibben/Develop/QtCreator/qt-creator

## set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/home/dibben/Develop/QtCreator/qt-creator-build-5.4

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)

TEMPLATE = lib

QT += network script

PROVIDER = CreatorPlugins

HEADERS += reviewboardplugin.h \
	reviewboardsettingspage.h \
	reviewboard_global.h \
	reviewboardconstants.h \
	submitreviewdialog.h \
	reviewboardserver.h \
	subversionclient.h \
	repository.h \
	reviewinfo.h \
	updatereviewdialog.h

SOURCES += reviewboardplugin.cpp \
	reviewboardsettingspage.cpp \
	submitreviewdialog.cpp \
	reviewboardserver.cpp \
	subversionclient.cpp \
	repository.cpp \
	reviewinfo.cpp \
	updatereviewdialog.cpp

OTHER_FILES += \
    ReviewBoard.json.in

FORMS += reviewboardsettingspage.ui \
	submitreviewdialog.ui \
	updatereviewdialog.ui

RESOURCES += reviewboard.qrc


LIBS += -L$$IDE_PLUGIN_PATH/QtProject

