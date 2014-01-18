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

class RecordListScreen;
class RecordTableModel;

class RecordTableScreen : public QWidget
{
 Q_OBJECT

public:
 RecordTableScreen(QWidget *parent=0);
 virtual ~RecordTableScreen();

 int getFirstSelectionPos(void);
 void setSelectionToPos(int pos);

 // Действия, используемые как на тулбаре, так и в контекстном меню списка записей
 QAction *actionAddNewToEnd;
 QAction *actionAddNewBefore;
 QAction *actionAddNewAfter;
 QAction *actionEditField;
 QAction *actionDelete;
 QAction *actionCut;
 QAction *actionCopy;
 QAction *actionPaste;

public slots:

 // Обновление панели инструментов
 void toolsUpdate(void);

private slots:
 
 void findInBaseOpen(void);
 void onSyncroClick(void);
 void onWalkHistoryPreviousClick(void);
 void onWalkHistoryNextClick(void);

private:
 QToolBar *toolsLine;
 QToolBar *findLine;
 
 RecordListScreen *recordListScreen;
  
 QHBoxLayout *recordTableToolsLayout;
 QVBoxLayout *recordTableScreenLayout;

 QAction *actionMoveUp;
 QAction *actionMoveDn;
 QAction *actionFindInBase;
 QAction *actionSyncro;
 QAction *actionWalkHistoryPrevious;
 QAction *actionWalkHistoryNext;
  
 void setupUI(void);
 void setupSignals(void);
 void setupActions(void);
 void assembly(void);
 
 void disableAllActions(void);

};

#endif /* RECORDTABLESCREEN_H_ */
