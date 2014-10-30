#include <QObject>
#include <QMimeData>
#include <QProgressBar>
#include <QMap>
#include <QList>

#include "main.h"
#include "RecordTableView.h"
#include "RecordTableScreen.h"

#include "views/mainWindow/MainWindow.h"
#include "views/record/MetaEditor.h"
#include "models/appConfig/AppConfig.h"
#include "views/findInBaseScreen/FindScreen.h"

extern AppConfig mytetraConfig;


// Виджет, который отображает список записей в ветке
// c кнопками управления

RecordTableScreen::RecordTableScreen(QWidget *parent) : QWidget(parent)
{
 // Инициализируется область со списком записей
 recordTableView=new RecordTableView(this);
 recordTableView->setObjectName("RecordTableView");

 setupActions();

 recordTableView->init();

 setupUI();
 
 setupSignals();
 assembly();
}


RecordTableScreen::~RecordTableScreen()
{

}


// Настройка возможных действий
void RecordTableScreen::setupActions(void)
{
 // Добавление записи
 // a->setShortcut(tr("Ctrl+X"));
 actionAddNewToEnd = new QAction(tr("Add note"), this);
 actionAddNewToEnd->setStatusTip(tr("Add a new note"));
 actionAddNewToEnd->setIcon(QIcon(":/resource/pic/note_add.svg"));
 connect(actionAddNewToEnd, SIGNAL(triggered()), recordTableView, SLOT(addNewToEndContext()));

 // Добавление записи до
 actionAddNewBefore = new QAction(tr("Add note before"), this);
 actionAddNewBefore->setStatusTip(tr("Add a note before selected"));
 connect(actionAddNewBefore, SIGNAL(triggered()), recordTableView, SLOT(addNewBeforeContext()));

 // Добавление записи после
 actionAddNewAfter = new QAction(tr("Add note after"), this);
 actionAddNewAfter->setStatusTip(tr("Add a note after selected"));
 connect(actionAddNewAfter, SIGNAL(triggered()), recordTableView, SLOT(addNewAfterContext()));

 // Редактирование записи
 actionEditField = new QAction(tr("Edit properties (name, author, tags...)"), this);
 actionEditField->setStatusTip(tr("Edit note properties (name, author, tags...)"));
 actionEditField->setIcon(QIcon(":/resource/pic/note_edit.svg"));
 connect(actionEditField, SIGNAL(triggered()), recordTableView, SLOT(editFieldContext()));

 // Удаление записи
 actionDelete = new QAction(tr("Delete note(s)"), this);
 actionDelete->setStatusTip(tr("Delete note(s)"));
 actionDelete->setIcon(QIcon(":/resource/pic/note_delete.svg"));
 connect(actionDelete, SIGNAL(triggered()), recordTableView, SLOT(deleteContext()));

 // Удаление записи с копированием в буфер обмена
 actionCut = new QAction(tr("&Cut note(s)"), this);
 actionCut->setStatusTip(tr("Cut notes(s) to clipboard"));
 actionCut->setIcon(QIcon(":/resource/pic/cb_cut.svg"));
 connect(actionCut, SIGNAL(triggered()), recordTableView, SLOT(cut()));

 // Копирование записи (записей) в буфер обмена
 actionCopy = new QAction(tr("&Copy note(s)"), this);
 actionCopy->setStatusTip(tr("Copy note(s) to clipboard"));
 actionCopy->setIcon(QIcon(":/resource/pic/cb_copy.svg"));
 connect(actionCopy, SIGNAL(triggered()), recordTableView, SLOT(copy()));

 // Вставка записи из буфера обмена
 actionPaste = new QAction(tr("&Paste note(s)"), this);
 actionPaste->setStatusTip(tr("Paste note(s) from clipboard"));
 actionPaste->setIcon(QIcon(":/resource/pic/cb_paste.svg"));
 connect(actionPaste, SIGNAL(triggered()), recordTableView, SLOT(paste()));

 // Настройка внешнего вида таблицы конечных записей
 actionSettings = new QAction(tr("&View settings"), this);
 actionSettings->setStatusTip(tr("Setup table view settins"));
 connect(actionSettings, SIGNAL(triggered()), recordTableView, SLOT(settings()));

 // Перемещение записи вверх
 actionMoveUp = new QAction(tr("&Move Up"), this);
 actionMoveUp->setStatusTip(tr("Move note up"));
 actionMoveUp->setIcon(QIcon(":/resource/pic/move_up.svg"));
 connect(actionMoveUp, SIGNAL(triggered()), recordTableView, SLOT(moveUp()));

 // Перемещение записи вниз
 actionMoveDn=new QAction(tr("&Move Down"), this);
 actionMoveDn->setStatusTip(tr("Move note down"));
 actionMoveDn->setIcon(QIcon(":/resource/pic/move_dn.svg"));
 connect(actionMoveDn, SIGNAL(triggered()), recordTableView, SLOT(moveDn()));

 // Поиск по базе
 actionFindInBase=new QAction(tr("Find in base"), this);
 actionFindInBase->setStatusTip(tr("Find in base"));
 actionFindInBase->setIcon(QIcon(":/resource/pic/find_in_base.svg"));
 connect(actionFindInBase, SIGNAL(triggered()), this, SLOT(findInBaseOpen()));

 // Синхронизация
 actionSyncro=new QAction(tr("Synchronization"), this);
 actionSyncro->setStatusTip(tr("Run synchronization"));
 actionSyncro->setIcon(QIcon(":/resource/pic/synchronization.svg"));
 connect(actionSyncro, SIGNAL(triggered()), this, SLOT(onSyncroClick()));

 // Перемещение по истории посещаемых записей назад
 actionWalkHistoryPrevious=new QAction(tr("Previous viewing note"), this);
 actionWalkHistoryPrevious->setShortcut(tr("Ctrl+<"));
 actionWalkHistoryPrevious->setStatusTip(tr("Previous note has been viewing"));
 actionWalkHistoryPrevious->setIcon(QIcon(":/resource/pic/walk_history_previous.svg"));
 connect(actionWalkHistoryPrevious, SIGNAL(triggered()), this, SLOT(onWalkHistoryPreviousClick()));

 // Перемещение по истории посещаемых записей вперед
 actionWalkHistoryNext=new QAction(tr("Next viewing note"), this);
 actionWalkHistoryNext->setShortcut(tr("Ctrl+>"));
 actionWalkHistoryNext->setStatusTip(tr("Next note has been viewing"));
 actionWalkHistoryNext->setIcon(QIcon(":/resource/pic/walk_history_next.svg"));
 connect(actionWalkHistoryNext, SIGNAL(triggered()), this, SLOT(onWalkHistoryNextClick()));


 // Сразу после создания все действия запрещены
 disableAllActions();
}


void RecordTableScreen::setupUI(void)
{
 toolsLine=new QToolBar(this);

 QSize toolBarIconSize(16,16);
 toolsLine->setIconSize(toolBarIconSize);

 insert_action_as_button(toolsLine, actionAddNewToEnd);
 insert_action_as_button(toolsLine, actionEditField);
 insert_action_as_button(toolsLine, actionDelete);
 toolsLine->addSeparator();
 insert_action_as_button(toolsLine, actionCut);
 insert_action_as_button(toolsLine, actionCopy);
 insert_action_as_button(toolsLine, actionPaste);
 toolsLine->addSeparator();
 insert_action_as_button(toolsLine, actionMoveUp);
 insert_action_as_button(toolsLine, actionMoveDn);


 findLine=new QToolBar(this);
 findLine->setIconSize(toolBarIconSize);
 insert_action_as_button(findLine, actionSyncro);
 insert_action_as_button(findLine, actionWalkHistoryPrevious);
 insert_action_as_button(findLine, actionWalkHistoryNext);
 insert_action_as_button(findLine, actionFindInBase);
}


void RecordTableScreen::setupSignals(void)
{

}


void RecordTableScreen::assembly(void)
{
 recordTableToolsLayout=new QHBoxLayout();
 recordTableToolsLayout->addWidget(toolsLine);
 recordTableToolsLayout->addStretch();
 recordTableToolsLayout->addWidget(findLine);
 
 recordTableScreenLayout=new QVBoxLayout();
 recordTableScreenLayout->setObjectName("recordtablescreen_QVBoxLayout");

 recordTableScreenLayout->addLayout(recordTableToolsLayout);
 recordTableScreenLayout->addWidget(recordTableView);

 setLayout(recordTableScreenLayout);

 // Границы убираются, так как данный объект будет использоваться как виджет
 QLayout *lt;
 lt=layout();
 lt->setContentsMargins(0,2,0,0);
}


// Отключение всех действий
void RecordTableScreen::disableAllActions(void)
{
 actionAddNewToEnd->setEnabled(false);
 actionAddNewBefore->setEnabled(false);
 actionAddNewAfter->setEnabled(false);
 actionEditField->setEnabled(false);
 actionDelete->setEnabled(false);

 actionCut->setEnabled(false);
 actionCopy->setEnabled(false);
 actionPaste->setEnabled(false);

 actionMoveUp->setEnabled(false);
 actionMoveDn->setEnabled(false);
}


void RecordTableScreen::toolsUpdate(void)
{
 qDebug() << "recordtablescreen::tools_update()";

 // Отключаются все действия
 disableAllActions();

 if(recordTableView->getTableData()==NULL)
  return;

 // Выясняется, содержит ли текущая ветка подчиненные ветки
 /*
 QModelIndex index = find_object<TreeScreen>("TreeScreen")->get_selection_model()->currentIndex();
 TreeItem *item = find_object<TreeScreen>("TreeScreen")->kntrmodel->getItem(index);
 int branch_have_children=0;
 if(item->childCount()>0)branch_have_children=1;
 */

 // Включаются те действия которые разрешены

 // Добавление записи
 // Добавлять можно к любой ветке
 actionAddNewToEnd->setEnabled(true);

 // Добавление записи до
 // Добавлять "до" можно только тогда, когда выбрана только одна строка
 if((recordTableView->selectionModel()->hasSelection() &&
     (recordTableView->selectionModel()->selectedRows()).size()==1))
    actionAddNewBefore->setEnabled(true);

 // Добавление записи после
 // Добавлять "после" можно только тогда, когда выбрана только одна строка
 if((recordTableView->selectionModel()->hasSelection() &&
     (recordTableView->selectionModel()->selectedRows()).size()==1))
  actionAddNewAfter->setEnabled(true);

 // Редактирование записи
 // Редактировать можно только тогда, когда выбрана только одна строка
 if(recordTableView->selectionModel()->hasSelection() &&
    (recordTableView->selectionModel()->selectedRows()).size()==1)
  actionEditField->setEnabled(true);

 // Удаление записи
 // Пункт активен только если запись (или записи) выбраны в списке
 if(recordTableView->selectionModel()->hasSelection())
  actionDelete->setEnabled(true);

 // Удаление с копированием записи в буфер обмена
 // Пункт активен только если запись (или записи) выбраны в списке
 if(recordTableView->selectionModel()->hasSelection())
  actionCut->setEnabled(true);

 // Копирование записи в буфер обмена
 // Пункт активен только если запись (или записи) выбраны в списке
 if(recordTableView->selectionModel()->hasSelection())
  actionCopy->setEnabled(true);

 // Вставка записи из буфера обмена
 // Вставлять записи можно только тогда, когда выбрана
 // только одна строка (добавляется после выделеной строки)
 // или не выбрано ни одной строки (тогда добавляется в конец списка)
 // или записей вообще нет
 // И проверяется, содержит ли буфер обмена данные нужного формата
 if((recordTableView->selectionModel()->hasSelection() && (recordTableView->selectionModel()->selectedRows()).size()==1) ||
     recordTableView->selectionModel()->hasSelection()==false ||
     recordTableView->model()->rowCount()==0)
  {
   const QMimeData *mimeData=QApplication::clipboard()->mimeData();
   if(mimeData!=NULL)
    if(mimeData->hasFormat("mytetra/records"))
     actionPaste->setEnabled(true);
  }

 // Перемещение записи вверх
 // Пункт возможен только когда выбрана одна строка
 // и указатель стоит не на начале списка
 if((recordTableView->selectionModel()->hasSelection() && (recordTableView->selectionModel()->selectedRows()).size()==1) &&
    recordTableView->isSelectedSetToTop()==false )
  actionMoveUp->setEnabled(true);

 // Перемещение записи вниз
 // Пункт возможен только когда выбрана одна строка
 // и указатель стоит не в конце списка
 if((recordTableView->selectionModel()->hasSelection() && (recordTableView->selectionModel()->selectedRows()).size()==1) &&
    recordTableView->isSelectedSetToBottom()==false )
  actionMoveDn->setEnabled(true);

 // Обновляется состояние области редактирования текста
 if(recordTableView->selectionModel()->hasSelection() &&
    recordTableView->getRowCount()>0)
  {
   qDebug() << "In table select present";
   qDebug() << "In table row count is" << recordTableView->getRowCount();
   find_object<MetaEditor>("editorview")->set_textarea_editable(true);
  }
 else
  {
   qDebug() << "In table select non present";
   find_object<MetaEditor>("editorview")->set_textarea_editable(false);
  }
}


// Получение номера первого выделенного элемента
int RecordTableScreen::getFirstSelectionPos(void)
{
 return recordTableView->getFirstSelectionPos();
}


// Установка засветки в нужную строку
void RecordTableScreen::setSelectionToPos(int pos)
{
 recordTableView->setSelectionToPos(pos);
}


// Действия при нажатии на кнопку поиска по базе
void RecordTableScreen::findInBaseOpen(void)
{
 // Определяется ссылка на виджет поиска
 FindScreen *findScreen=find_object<FindScreen>("findscreendisp");
 
 // Если виджет не показан, он выводится на экран, и наоборот
 if(findScreen->isVisible()==false)
  findScreen->widgetShow();
 else
  findScreen->widgetHide();
}


// Действия при нажатии кнопки синхронизации
void RecordTableScreen::onSyncroClick(void)
{
 find_object<MainWindow>("mainwindow")->synchronization();
}


void RecordTableScreen::onWalkHistoryPreviousClick(void)
{
 find_object<MainWindow>("mainwindow")->goWalkHistoryPrevious();
}


void RecordTableScreen::onWalkHistoryNextClick(void)
{
 find_object<MainWindow>("mainwindow")->goWalkHistoryNext();
}
