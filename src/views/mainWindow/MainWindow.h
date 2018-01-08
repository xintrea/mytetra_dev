#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QMainWindow>
#include <QWidget>
#include <QTextListFormat>
#include <QTextBlockFormat>

#include <QtXml>
#include <QtGui>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QItemSelectionModel>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextCodec>
#include <QTextEdit>
#include <QToolBar>
#include <QTextCursor>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QList>
#include <QHBoxLayout>
#include <QSplitter>
#include <QSystemTrayIcon>

#include <QStatusBar>

class TreeScreen;
class MetaEditor;
class RecordTableScreen;
class FindScreen;
class WindowSwitcher;


class MainWindow : public QMainWindow
{
Q_OBJECT

public:
 MainWindow();
 ~MainWindow();

 TreeScreen *treeScreen;
 RecordTableScreen *recordTableScreen;
 MetaEditor *editorScreen;
 FindScreen *findScreenDisp;
 QStatusBar *statusBar;
 WindowSwitcher *windowSwitcher;

 void restoreGeometry(void);
 void restoreTreePosition(void);
 void restoreRecordTablePosition(void);
 void restoreEditorCursorPosition(void);
 void restoreEditorScrollBarPosition(void);
 void restoreFindOnBaseVisible(void);

 void setTreePosition(QStringList path);
 bool isTreePositionCrypt();

 void setRecordtablePositionById(QString id);

 void synchronization(bool visible=true);

 void goWalkHistoryPrevious(void);
 void goWalkHistoryNext(void);

 void saveTextarea(void);

 void saveAllState(void);

 void reload(void);

signals:

 void globalPressKey(int key);
 void globalReleaseKey(int key);

public slots:
 void applicationExit(void);
 void applicationFastExit(void);
 void commitData(QSessionManager& manager);
 void messageHandler(QString message);

private slots:

 void showWindow();

 void fileNew(void);
 void fileOpen(void);
 bool fileSave(void);
 bool fileSaveAs(void);
 void filePrint(void);
 void filePrintPreview(void);
 void filePrintPdf(void);
 void fileExportBranch(void);
 void fileImportBranch(void);
 
 void toolsFind(void);
 void toolsPreferences(void);
 void onActionLogClicked(void);

 void onExpandEditArea(bool flag);

 void onClickHelpAboutMyTetra(void);
 void onClickHelpAboutQt(void);
 void onClickHelpTechnicalInfo(void);
 
 void iconActivated(QSystemTrayIcon::ActivationReason reason);

 void onFocusChanged(QWidget *, QWidget *);

private:

 void setupUI(void);
 void setupSignals(void);
 void assembly(void);

 void initFileMenu(void);
 void initToolsMenu(void);
 void initPreferencesMenu(QMenu *menu);
 void initHelpMenu(void);
 
 void initRecordTableActions(void);

 void setupIconActions(void);
 void createTrayIcon(void);
 void setIcon(void);

 void saveGeometry(void);
 void saveTreePosition(void);
 void saveRecordTablePosition(void);
 void saveEditorCursorPosition(void);
 void saveEditorScrollBarPosition(void);

 void reloadSaveStage(void);
 void reloadLoadStage(void);

 QAction *actionTrayRestore;
 QAction *actionTrayMaximize;
 QAction *actionTrayMinimize;
 QAction *actionTrayQuit;

 QSystemTrayIcon *trayIcon;
 QMenu           *trayIconMenu;

 QSplitter *vSplitter;
 QSplitter *hSplitter;
 QSplitter *findSplitter;
 
protected:
 
 void closeEvent(QCloseEvent *event);

 bool eventFilter( QObject * o, QEvent * e ); // Отслеживание прочих событий

 void keyPressEvent(QKeyEvent *event);
 void keyReleaseEvent(QKeyEvent *event);

 void goWalkHistory(void);

 bool enableRealClose;

 
};
#endif
