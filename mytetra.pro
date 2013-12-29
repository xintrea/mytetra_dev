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

TARGET = mytetra
RESOURCES = bin/mytetra.qrc
TRANSLATIONS = bin/resource/translations/mytetra_ru.ts
CODECFORTR  = utf8
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = 
QMAKE_LFLAGS += -L/usr/lib/qt4/lib

!contains(TARGET_OS_IS_MEEGO, 1) {
 message(Building the any OS version...)
 SYSTEM_PROGRAM_NAME=mytetra
 BINARY_INSTALL_PATH=/usr/local/bin
} else {
 message(Building the MeeGo OS version...)
 SYSTEM_PROGRAM_NAME=ru.webhamster.mytetra
 BINARY_INSTALL_PATH=/opt/$${SYSTEM_PROGRAM_NAME}/bin
 DEFINES+=TARGET_OS_MEEGO
}

message(Set installation directory for binary file to $${BINARY_INSTALL_PATH})

# mytetra_binary.path=$${BINARY_INSTALL_PATH}
# mytetra_binary.files=bin/mytetra
# INSTALLS+=mytetra_binary
target.path=$${BINARY_INSTALL_PATH}
INSTALLS+=target

desktop_file.path=/usr/share/applications
!contains(TARGET_OS_IS_MEEGO, 1) {
 desktop_file.files=desktop/any/mytetra.desktop
} else {
 desktop_file.files=desktop/meego/mytetra.desktop
}
INSTALLS+=desktop_file

icon_scalable_file.path=/usr/share/icons/hicolor/scalable/apps
icon_scalable_file.files=desktop/mytetra.svg
INSTALLS+=icon_scalable_file

icon_48_file.path=/usr/share/icons/hicolor/48x48/apps
icon_48_file.files=desktop/mytetra.png
INSTALLS+=icon_48_file


HEADERS = src/SaveLoad.h \
    src/RecordTableScreen.h \
    src/RecordInfoFieldsEditor.h \
    src/InfoFieldEnter.h \
    src/MetaEditor.h \
    src/libraries/wyedit/Editor.h \
    src/libraries/wyedit/EditorConfig.h \
    src/libraries/wyedit/EditorConfigDialog.h \
    src/libraries/wyedit/EditorConfigFont.h \
    src/libraries/wyedit/EditorConfigMisc.h \
    src/libraries/wyedit/EditorFindDialog.h \
    src/MainWindow.h \
    src/PrintPreview.h \
    src/main.h \
    src/XmlTree.h \
    src/TreeModel.h \
    src/TreeItem.h \
    src/KnowTreeModel.h \
    src/RecordTableData.h \
    src/RecordTableModel.h \
    src/AppConfig.h \
    src/AddNewRecord.h \
    src/ClipboardRecords.h \
    src/TreeScreen.h \
    src/FindScreen.h \
    src/FindTableWidget.h \
    src/AppConfigDialog.h \
    src/ConfigDialog.h \
    src/ConfigPage.h \
    src/AppConfigPage_Main.h \
    src/AppConfigPage_Misc.h \
    src/AppConfigPage_Crypt.h \
    src/AppConfigPage_Synchro.h \
    src/AppConfigUpdater.h \
    src/TrashMonitoring.h \
    src/libraries/wyedit/EditorContextMenu.h \
    src/libraries/wyedit/EditorTextArea.h \
    src/libraries/wyedit/indentslider/IndentSlider.h \
    src/libraries/wyedit/EditorSplitCellForm.h \
    src/libraries/wyedit/EditorAbsTableCell.h \
    src/libraries/wyedit/EditorAbsTable.h \
    src/libraries/wyedit/EditorAddTableForm.h \
    src/libraries/wyedit/EditorImageProperties.h \
    src/libraries/wyedit/EditorMultiLineInputDialog.h \
    src/libraries/qtsingleapplication/qtsingleapplication.h \
    src/libraries/qtsingleapplication/qtlockedfile.h \
    src/libraries/qtsingleapplication/qtlocalpeer.h \
    src/libraries/qtsingleapplication/qtsinglecoreapplication.h \
    src/RecordListScreen.h \
    src/ClipboardBranch.h \
    src/GlobalParameters.h \
    src/DataBaseConfig.h \
    src/EnterPassword.h \
    src/ExecuteCommand.h \
    src/ConsoleEmulator.h \
    src/WaitClock.h \
    src/WalkHistory.h \
    src/libraries/crypt/Pbkdf2Qt.h \
    src/libraries/crypt/RC5Simple.h \
    src/libraries/crypt/Password.h
SOURCES = src/SaveLoad.cpp \
    src/RecordTableScreen.cpp \
    src/RecordInfoFieldsEditor.cpp \
    src/InfoFieldEnter.cpp \
    src/MetaEditor.cpp \
    src/libraries/wyedit/Editor.cpp \
    src/libraries/wyedit/EditorConfig.cpp \
    src/libraries/wyedit/EditorConfigDialog.cpp \
    src/libraries/wyedit/EditorConfigFont.cpp \
    src/libraries/wyedit/EditorConfigMisc.cpp \
    src/libraries/wyedit/EditorFindDialog.cpp \
    src/MainWindow.cpp \
    src/PrintPreview.cpp \
    src/main.cpp \
    src/XmlTree.cpp \
    src/TreeModel.cpp \
    src/TreeItem.cpp \
    src/KnowTreeModel.cpp \
    src/RecordTableData.cpp \
    src/RecordTableModel.cpp \
    src/AppConfig.cpp \
    src/AddNewRecord.cpp \
    src/ClipboardRecords.cpp \
    src/TreeScreen.cpp \
    src/FindScreen.cpp \
    src/FindTableWidget.cpp \
    src/AppConfigDialog.cpp \
    src/ConfigDialog.cpp \
    src/ConfigPage.cpp \
    src/AppConfigPage_Main.cpp \
    src/AppConfigPage_Misc.cpp \
    src/AppConfigPage_Crypt.cpp \
    src/AppConfigPage_Synchro.cpp \
    src/AppConfigUpdater.cpp \
    src/TrashMonitoring.cpp \
    src/libraries/wyedit/EditorContextMenu.cpp \
    src/libraries/wyedit/EditorTextArea.cpp \
    src/libraries/wyedit/indentslider/IndentSlider.cpp \
    src/libraries/wyedit/EditorSplitCellForm.cpp \
    src/libraries/wyedit/EditorAbsTableCell.cpp \
    src/libraries/wyedit/EditorAbsTable.cpp \
    src/libraries/wyedit/EditorAddTableForm.cpp \
    src/libraries/wyedit/EditorImageProperties.cpp \
    src/libraries/wyedit/EditorMultiLineInputDialog.cpp \
    src/libraries/qtsingleapplication/qtsingleapplication.cpp \
    src/libraries/qtsingleapplication/qtlockedfile.cpp \
    src/libraries/qtsingleapplication/qtlocalpeer.cpp \
    src/libraries/qtsingleapplication/qtsinglecoreapplication.cpp \
    src/RecordListScreen.cpp \
    src/ClipboardBranch.cpp \
    src/GlobalParameters.cpp \
    src/DataBaseConfig.cpp \
    src/EnterPassword.cpp \
    src/ExecuteCommand.cpp \
    src/ConsoleEmulator.cpp \
    src/WaitClock.cpp \
    src/WalkHistory.cpp \
    src/libraries/crypt/Pbkdf2Qt.cpp \
    src/libraries/crypt/RC5Simple.cpp \
    src/libraries/crypt/Password.cpp

wince* { 
    CONFIG(debug, release|debug):addPlugins.sources = $$QT_BUILD_TREE/plugins/imageformats/qsvgd4.dll
    CONFIG(release, release|debug):addPlugins.sources = $$QT_BUILD_TREE/plugins/imageformats/qsvg4.dll
    addPlugins.path = imageformats
    DEPLOYMENT += addPlugins
}
