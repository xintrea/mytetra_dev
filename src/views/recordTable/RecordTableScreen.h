#ifndef RECORDTABLESCREEN_H_
#define RECORDTABLESCREEN_H_

#include <QWidget>
#include <QListView>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStringList>
#include <QMap>
#include <QString>
#include <QByteArray>
#include <QItemSelection>
#include <QLabel>

class RecordTableController;

class RecordTableScreen : public QWidget
{
 Q_OBJECT

public:
 RecordTableScreen(QWidget *parent=0);
 virtual ~RecordTableScreen();

 int     getFirstSelectionPos(void);
 QString getFirstSelectionId(void);
 void    setSelectionToPos(int pos);
 void    setSelectionToId(QString id);

 void setTreePath(QString path);
 QString getTreePath(void);


 // Действия, используемые как на тулбаре, так и в контекстном меню списка записей
 QAction *actionAddNewToEnd;
 QAction *actionAddNewBefore;
 QAction *actionAddNewAfter;
 QAction *actionEditField;
 QAction *actionBlock;
 QAction *actionDelete;
 QAction *actionCut;
 QAction *actionCopy;
 QAction *actionPaste;
 QAction *actionSettings;
 QAction *actionBack;
 QAction *actionFindInBase;
 QAction *actionSort;
 QAction *actionPrint;
 QAction *actionCopyRecordReference;
 QAction *actionSwitchSelectionMode;

 // Действие, которое может быть вызвано из MainWindows
 QAction *actionSyncro;

public slots:

 // Обновление состояния экранных элементов согласно состоянию записи и положении зиписи в таблице
 void toolsUpdate(void);

 void onExecuteCommandFinishWork(void);

private slots:
 
 void onSyncroClick(void);
 void onWalkHistoryPreviousClick(void);
 void onWalkHistoryNextClick(void);
 void onBackClick(void);
 void onCopyRecordReference(void);

private:
 QToolBar *toolsLine;
 QToolBar *extraToolsLine;

 QLabel *treePathLabel;
 QString treePath;
 
 RecordTableController *recordTableController;

 QHBoxLayout *recordTableToolsLayout;
 QVBoxLayout *recordTableScreenLayout;

 QAction *actionMoveUp;
 QAction *actionMoveDn;
 QAction *actionWalkHistoryPrevious;
 QAction *actionWalkHistoryNext;
  
 void setupUI(void);
 void setupSignals(void);
 void setupActions(void);
 void assembly(void);
 
 void disableAllActions(void);

 void toolsWidgatsUpdate();
 void editorModesUpdate();

};

#endif /* RECORDTABLESCREEN_H_ */
