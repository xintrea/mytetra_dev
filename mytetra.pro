# Please, set manual your target OS
# ANY_OS - for desktop Windows and Linux
# MEEGO_OS - for MEEGO
# ANDROID_OS - for Android
TARGET_OS=ANY_OS


# Create define variable in C++ code
DEFINES+="ANY_OS=1"
DEFINES+="MEEGO_OS=2"
DEFINES+="ANDROID_OS=3"
DEFINES+="TARGET_OS=$${TARGET_OS}"

message(Building running in Qt major version: $${QT_MAJOR_VERSION})

TEMPLATE = app
QT = gui \
    core \
    xml \
    svg \
    network
CONFIG += qt \
    warn_on \
    console \
    debug

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets \
                                        printsupport

contains(TARGET_OS, ANY_OS) {
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
}


TARGET = mytetra
RESOURCES = bin/mytetra.qrc
TRANSLATIONS = bin/resource/translations/mytetra_ru.ts
CODECFORTR  = utf8

# QMAKE_LFLAGS += -L/usr/lib/qt4/lib
INCLUDEPATH += $${_PRO_FILE_PWD_}/src

contains(TARGET_OS, ANY_OS) {
 message(Building the any OS version...)
 SYSTEM_PROGRAM_NAME=mytetra
 BINARY_INSTALL_PATH=/usr/local/bin
}

contains(TARGET_OS, MEEGO_OS){
 message(Building the MeeGo OS version...)
 SYSTEM_PROGRAM_NAME=ru.webhamster.mytetra
 BINARY_INSTALL_PATH=/opt/$${SYSTEM_PROGRAM_NAME}/bin
}

contains(TARGET_OS, ANDROID_OS){
 message(Building the Android OS version...)
 SYSTEM_PROGRAM_NAME=mytetra
 BINARY_INSTALL_PATH=/
}


message(Set installation directory for binary file to $${BINARY_INSTALL_PATH})

# mytetra_binary.path=$${BINARY_INSTALL_PATH}
# mytetra_binary.files=bin/mytetra
# INSTALLS+=mytetra_binary
target.path=$${BINARY_INSTALL_PATH}
INSTALLS+=target

desktop_file.path=/usr/share/applications
contains(TARGET_OS, ANY_OS) {
 desktop_file.files=desktop/any/mytetra.desktop
}
contains(TARGET_OS, MEEGO_OS) {
 desktop_file.files=desktop/meego/mytetra.desktop
}
contains(TARGET_OS, ANDROID_OS) {
 desktop_file.files=desktop/any/mytetra.desktop
}
INSTALLS+=desktop_file

icon_scalable_file.path=/usr/share/icons/hicolor/scalable/apps
icon_scalable_file.files=desktop/mytetra.svg
INSTALLS+=icon_scalable_file

icon_48_file.path=/usr/share/icons/hicolor/48x48/apps
icon_48_file.files=desktop/mytetra.png
INSTALLS+=icon_48_file


HEADERS = src/main.h \
    src/views/recordTable/RecordTableScreen.h \
    src/views/record/RecordInfoFieldsEditor.h \
    src/views/record/InfoFieldEnter.h \
    src/views/record/MetaEditor.h \
    src/libraries/wyedit/Editor.h \
    src/libraries/wyedit/EditorConfig.h \
    src/libraries/wyedit/EditorConfigDialog.h \
    src/libraries/wyedit/EditorConfigFont.h \
    src/libraries/wyedit/EditorConfigMisc.h \
    src/libraries/wyedit/EditorFindDialog.h \
    src/views/mainWindow/MainWindow.h \
    src/views/printPreview/PrintPreview.h \
    src/views/printPreview/PreviewView.h \
    src/models/tree/XmlTree.h \
    src/models/tree/TreeModel.h \
    src/models/tree/TreeItem.h \
    src/models/tree/KnowTreeModel.h \
    src/models/recordTable/RecordTableData.h \
    src/models/recordTable/RecordTableModel.h \
    src/models/appConfig/AppConfig.h \
    src/views/record/AddNewRecord.h \
    src/libraries/ClipboardRecords.h \
    src/views/tree/TreeScreen.h \
    src/views/findInBaseScreen/FindScreen.h \
    src/views/findInBaseScreen/FindTableWidget.h \
    src/views/appConfigWindow/AppConfigDialog.h \
    src/views/appConfigWindow/ConfigDialog.h \
    src/views/appConfigWindow/ConfigPage.h \
    src/views/appConfigWindow/AppConfigPage_Main.h \
    src/views/appConfigWindow/AppConfigPage_Misc.h \
    src/views/appConfigWindow/AppConfigPage_Crypt.h \
    src/views/appConfigWindow/AppConfigPage_Synchro.h \
    src/models/appConfig/AppConfigUpdater.h \
    src/libraries/TrashMonitoring.h \
    src/libraries/wyedit/EditorContextMenu.h \
    src/libraries/wyedit/EditorTextArea.h \
    src/libraries/wyedit/indentslider/IndentSlider.h \
    src/libraries/wyedit/EditorSplitCellForm.h \
    src/libraries/wyedit/EditorAbsTableCell.h \
    src/libraries/wyedit/EditorAbsTable.h \
    src/libraries/wyedit/EditorAddTableForm.h \
    src/libraries/wyedit/EditorImageProperties.h \
    src/libraries/wyedit/EditorMultiLineInputDialog.h \
    src/libraries/qtSingleApplication/qtsingleapplication.h \
    src/libraries/qtSingleApplication/qtlockedfile.h \
    src/libraries/qtSingleApplication/qtlocalpeer.h \
    src/libraries/qtSingleApplication/qtsinglecoreapplication.h \
    src/views/recordTable/RecordListScreen.h \
    src/libraries/ClipboardBranch.h \
    src/libraries/GlobalParameters.h \
    src/models/dataBaseConfig/DataBaseConfig.h \
    src/views/enterPassword/EnterPassword.h \
    src/views/consoleEmulator/ExecuteCommand.h \
    src/views/consoleEmulator/ConsoleEmulator.h \
    src/views/waitClock/WaitClock.h \
    src/libraries/WalkHistory.h \
    src/libraries/crypt/Pbkdf2Qt.h \
    src/libraries/crypt/RC5Simple.h \
    src/libraries/crypt/Password.h 
SOURCES = src/main.cpp \
    src/views/recordTable/RecordTableScreen.cpp \
    src/views/record/RecordInfoFieldsEditor.cpp \
    src/views/record/InfoFieldEnter.cpp \
    src/views/record/MetaEditor.cpp \
    src/libraries/wyedit/Editor.cpp \
    src/libraries/wyedit/EditorConfig.cpp \
    src/libraries/wyedit/EditorConfigDialog.cpp \
    src/libraries/wyedit/EditorConfigFont.cpp \
    src/libraries/wyedit/EditorConfigMisc.cpp \
    src/libraries/wyedit/EditorFindDialog.cpp \
    src/views/mainWindow/MainWindow.cpp \
    src/views/printPreview/PrintPreview.cpp \
    src/views/printPreview/PreviewView.cpp \
    src/models/tree/XmlTree.cpp \
    src/models/tree/TreeModel.cpp \
    src/models/tree/TreeItem.cpp \
    src/models/tree/KnowTreeModel.cpp \
    src/models/recordTable/RecordTableData.cpp \
    src/models/recordTable/RecordTableModel.cpp \
    src/models/appConfig/AppConfig.cpp \
    src/views/record/AddNewRecord.cpp \
    src/libraries/ClipboardRecords.cpp \
    src/views/tree/TreeScreen.cpp \
    src/views/findInBaseScreen/FindScreen.cpp \
    src/views/findInBaseScreen/FindTableWidget.cpp \
    src/views/appConfigWindow/AppConfigDialog.cpp \
    src/views/appConfigWindow/ConfigDialog.cpp \
    src/views/appConfigWindow/ConfigPage.cpp \
    src/views/appConfigWindow/AppConfigPage_Main.cpp \
    src/views/appConfigWindow/AppConfigPage_Misc.cpp \
    src/views/appConfigWindow/AppConfigPage_Crypt.cpp \
    src/views/appConfigWindow/AppConfigPage_Synchro.cpp \
    src/models/appConfig/AppConfigUpdater.cpp \
    src/libraries/TrashMonitoring.cpp \
    src/libraries/wyedit/EditorContextMenu.cpp \
    src/libraries/wyedit/EditorTextArea.cpp \
    src/libraries/wyedit/indentslider/IndentSlider.cpp \
    src/libraries/wyedit/EditorSplitCellForm.cpp \
    src/libraries/wyedit/EditorAbsTableCell.cpp \
    src/libraries/wyedit/EditorAbsTable.cpp \
    src/libraries/wyedit/EditorAddTableForm.cpp \
    src/libraries/wyedit/EditorImageProperties.cpp \
    src/libraries/wyedit/EditorMultiLineInputDialog.cpp \
    src/libraries/qtSingleApplication/qtsingleapplication.cpp \
    src/libraries/qtSingleApplication/qtlockedfile.cpp \
    src/libraries/qtSingleApplication/qtlocalpeer.cpp \
    src/libraries/qtSingleApplication/qtsinglecoreapplication.cpp \
    src/views/recordTable/RecordListScreen.cpp \
    src/libraries/ClipboardBranch.cpp \
    src/libraries/GlobalParameters.cpp \
    src/models/dataBaseConfig/DataBaseConfig.cpp \
    src/views/enterPassword/EnterPassword.cpp \
    src/views/consoleEmulator/ExecuteCommand.cpp \
    src/views/consoleEmulator/ConsoleEmulator.cpp \
    src/views/waitClock/WaitClock.cpp \
    src/libraries/WalkHistory.cpp \
    src/libraries/crypt/Pbkdf2Qt.cpp \
    src/libraries/crypt/RC5Simple.cpp \
    src/libraries/crypt/Password.cpp 

wince* { 
    CONFIG(debug, release|debug):addPlugins.sources = $$QT_BUILD_TREE/plugins/imageformats/qsvgd4.dll
    CONFIG(release, release|debug):addPlugins.sources = $$QT_BUILD_TREE/plugins/imageformats/qsvg4.dll
    addPlugins.path = imageformats
    DEPLOYMENT += addPlugins
}

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml
