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
class RecordTableData;
class RecordTableModel;
class ClipboardRecords ;

class RecordTableScreen : public QWidget
{
 Q_OBJECT

public:
 RecordTableScreen(QWidget *parent=0);
 virtual ~RecordTableScreen();

 void save(void);

 void setTableData(RecordTableData *rtData);

 int getFirstSelectionPos(void);
 void setSelectionToPos(int pos);

 bool isSelectedSetToTop(void);
 bool isSelectedSetToBottom(void);

 ClipboardRecords *getSelectedRecords(void);

public slots:

 // Вызов действий для копирования записей в буфер обмена с удалением
 void cut(void);

 // Вызов действий для копирования записей в буфер обмена
 void copy(void);

 // Вызов действий для вставки записей из буфера
 void paste(void);


private slots:

 // Реакия на сдвиг засветки клавишами или мышкой
 // самодельный слот
 // void onSelectionRecordChanged(const QModelIndex &indexCurrent, const QModelIndex &indexPrevious);

 void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected );

 // Обновление панели инструментов
 void toolsUpdate(void);

 // Открытие контекстного меню
 void onCustomContextMenuRequested(const QPoint &pos);

 // Вызов действий из контекстного меню для открытия окна с вводом новой записи
 void addNewToEndContext(void);
 void addNewBeforeContext(void);
 void addNewAfterContext(void);

 // Вызов действий из контекстного меню для редактирования инфополей записи
 void editFieldContext(void);

 // Вызов действий из контекстного меню для удаления конечной записи
 void deleteContext(void);

 void moveUp(void);
 void moveDn(void);
 
 void findInBaseOpen(void);
 void onSyncroClick(void);
 void onWalkHistoryPreviousClick(void);
 void onWalkHistoryNextClick(void);

private:
 QToolBar *toolsLine;
 QToolBar *findLine;
 
 RecordListScreen *recordView;
 RecordTableModel *recordModel; // Класс, расширенный от QAbstractListModel
  
 QHBoxLayout *recordTableToolsLayout;
 QVBoxLayout *recordTableScreenLayout;

 QAction *actionAddNewToEnd;
 QAction *actionAddNewBefore;
 QAction *actionAddNewAfter;
 QAction *actionEditField;
 QAction *actionDelete;
 QAction *actionCut;
 QAction *actionCopy;
 QAction *actionPaste;
 QAction *actionMoveUp;
 QAction *actionMoveDn;
 QAction *actionFindInBase;
 QAction *actionSyncro;
 QAction *actionWalkHistoryPrevious;
 QAction *actionWalkHistoryNext;
 
 QMenu *contextMenu;
  
 void setupUI(void);
 void setupSignals(void);
 void setupActions(void);
 void assembly(void);

 void assemblyContextMenu(void);
 
 void disableAllActions(void);

 void addNewRecord(int mode);
 void addNew(int mode,
             QMap<QString, QString> fields,
             QString text,
             QMap<QString, QByteArray> files=(QMap<QString, QByteArray>()) );

 void editField(int pos,
                QString name,
                QString author,
                QString url,
                QString tags);

 void deleteRecords(void);

 // Реакия на выбор записи мышкой или клавишами
 void onClickToRecord(const QModelIndex &index);
};

#endif /* RECORDTABLESCREEN_H_ */
