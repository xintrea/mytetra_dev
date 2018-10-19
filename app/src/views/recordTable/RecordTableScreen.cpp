#include <QObject>
#include <QMimeData>
#include <QMap>
#include <QList>

#include "main.h"
#include "RecordTableView.h"
#include "RecordTableScreen.h"

#include "views/mainWindow/MainWindow.h"
#include "views/record/MetaEditor.h"
#include "models/appConfig/AppConfig.h"
#include "views/findInBaseScreen/FindScreen.h"
#include "libraries/WindowSwitcher.h"
#include "libraries/GlobalParameters.h"
#include "libraries/FixedParameters.h"
#include "controllers/recordTable/RecordTableController.h"
#include "libraries/ShortcutManager.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;
extern ShortcutManager shortcutManager;


// Виджет, который отображает список записей в ветке
// c кнопками управления

RecordTableScreen::RecordTableScreen(QWidget *parent) : QWidget(parent)
{
  // Инициализируется контроллер списка записей
  recordTableController=new RecordTableController(this);
  recordTableController->setObjectName("recordTableController");

  setupActions();

  recordTableController->init();


  setupUI();
  setupShortcuts();
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
 actionAddNewToEnd = new QAction(tr("Add note"), this);
 actionAddNewToEnd->setStatusTip(tr("Add a new note"));
 actionAddNewToEnd->setIcon(QIcon(":/resource/pic/note_add.svg"));

 // Добавление записи до
 actionAddNewBefore = new QAction(tr("Add note before"), this);
 actionAddNewBefore->setStatusTip(tr("Add a note before selected"));

 // Добавление записи после
 actionAddNewAfter = new QAction(tr("Add note after"), this);
 actionAddNewAfter->setStatusTip(tr("Add a note after selected"));

 // Редактирование записи
 actionEditField = new QAction(tr("Edit properties (name, author, tags...)"), this);
 actionEditField->setStatusTip(tr("Edit note properties (name, author, tags...)"));
 actionEditField->setIcon(QIcon(":/resource/pic/note_edit.svg"));

 // Блокировка записи
 actionBlock = new QAction(tr("Block/Unblock note"), this);
 actionBlock->setStatusTip(tr("Block or unblock change note"));
 actionBlock->setIcon(QIcon(":/resource/pic/note_block.svg"));

 // Удаление записи
 actionDelete = new QAction(tr("Delete note(s)"), this);
 actionDelete->setStatusTip(tr("Delete note(s)"));
 actionDelete->setIcon(QIcon(":/resource/pic/note_delete.svg"));

 // Удаление записи с копированием в буфер обмена
 actionCut = new QAction(tr("&Cut note(s)"), this);
 actionCut->setStatusTip(tr("Cut notes(s) to clipboard"));
 actionCut->setIcon(QIcon(":/resource/pic/cb_cut.svg"));

 // Копирование записи (записей) в буфер обмена
 actionCopy = new QAction(tr("&Copy note(s)"), this);
 actionCopy->setStatusTip(tr("Copy note(s) to clipboard"));
 actionCopy->setIcon(QIcon(":/resource/pic/cb_copy.svg"));

 // Вставка записи из буфера обмена
 actionPaste = new QAction(tr("&Paste note(s)"), this);
 actionPaste->setStatusTip(tr("Paste note(s) from clipboard"));
 actionPaste->setIcon(QIcon(":/resource/pic/cb_paste.svg"));

 // Настройка внешнего вида таблицы конечных записей
 actionSettings = new QAction(tr("&View settings"), this);
 actionSettings->setStatusTip(tr("Setup table view settins"));
 actionSettings->setIcon(QIcon(":/resource/pic/edit_settings.svg"));

 // Перемещение записи вверх
 actionMoveUp = new QAction(tr("&Move Up"), this);
 actionMoveUp->setStatusTip(tr("Move note up"));
 actionMoveUp->setIcon(QIcon(":/resource/pic/move_up.svg"));

 // Перемещение записи вниз
 actionMoveDn=new QAction(tr("&Move Down"), this);
 actionMoveDn->setStatusTip(tr("Move note down"));
 actionMoveDn->setIcon(QIcon(":/resource/pic/move_dn.svg"));

 // Поиск по базе (клик связывается с действием в MainWindow)
 actionFindInBase=new QAction(tr("Find in base"), this);
 actionFindInBase->setStatusTip(tr("Find in base"));
 actionFindInBase->setIcon(QIcon(":/resource/pic/find_in_base.svg"));

 // Синхронизация
 actionSyncro=new QAction(tr("Synchronization"), this);
 actionSyncro->setStatusTip(tr("Run synchronization"));
 actionSyncro->setIcon(QIcon(":/resource/pic/synchronization.svg"));

 // Перемещение по истории посещаемых записей назад
 actionWalkHistoryPrevious=new QAction(tr("Previous viewing note"), this);
 actionWalkHistoryPrevious->setIcon(QIcon(":/resource/pic/walk_history_previous.svg"));

 // Перемещение по истории посещаемых записей вперед
 actionWalkHistoryNext=new QAction(tr("Next viewing note"), this);
 actionWalkHistoryNext->setIcon(QIcon(":/resource/pic/walk_history_next.svg"));

 // Кнопка Назад (Back) в мобильном интерфейсе
 actionBack=new QAction(tr("Back to item tree"), this);
 actionBack->setStatusTip(tr("Back to item tree"));
 actionBack->setIcon(QIcon(":/resource/pic/mobile_back.svg"));

 // Действия по сортировке
 actionSort = new QAction(tr("Toggle sorting"), this);
 actionSort->setStatusTip(tr("Enable/disable sorting by column"));
 actionSort->setIcon(QIcon(":/resource/pic/sort.svg"));

 // Кнопка вызова печати таблицы конечных записей
 actionPrint = new QAction(tr("Print table"), this);
 actionPrint->setStatusTip(tr("Print current notes table"));
 actionPrint->setIcon(QIcon(":/resource/pic/print_table.svg"));

 // Кнопка копирования ссылки на запись
 actionCopyRecordReference = new QAction(tr("Copy note reference"), this);
 actionCopyRecordReference->setStatusTip(tr("Copy note reference to clipboard"));
 actionCopyRecordReference->setIcon(QIcon(":/resource/pic/note_reference.svg"));

 // Кнопка переключения режима одинарного выбора и мультивыбора
 actionSwitchSelectionMode = new QAction(tr("Switch select/multiselect"), this);
 actionSwitchSelectionMode->setStatusTip(tr("Switch note selection mode (Notice: if multiselect is on, drag-and-drop is disabled)"));
 actionSwitchSelectionMode->setIcon(QIcon(":/resource/pic/switch_note_selection_mode.svg"));

 // Сразу после создания все действия запрещены
 disableAllActions();
}


void RecordTableScreen::setupUI(void)
{
 toolsLine=new QToolBar(this);

 /*
 QSize toolBarIconSize(16,16);
 toolsLine->setIconSize(toolBarIconSize);
 */

 if(mytetraConfig.getInterfaceMode()=="mobile")
 {
   insertActionAsButton(toolsLine, actionBack);
   toolsLine->addSeparator();
 }

 insertActionAsButton(toolsLine, actionAddNewToEnd);
 if(mytetraConfig.getInterfaceMode()=="desktop")
 {
   insertActionAsButton(toolsLine, actionEditField);
   insertActionAsButton(toolsLine, actionDelete);
 }

 toolsLine->addSeparator();
 insertActionAsButton(toolsLine, actionCut);
 insertActionAsButton(toolsLine, actionCopy);
 insertActionAsButton(toolsLine, actionPaste);
 toolsLine->addSeparator();
 insertActionAsButton(toolsLine, actionMoveUp);
 insertActionAsButton(toolsLine, actionMoveDn);


 extraToolsLine=new QToolBar(this);

 if(mytetraConfig.getInterfaceMode()=="desktop")
 {
   insertActionAsButton(extraToolsLine, actionSyncro);
   insertActionAsButton(extraToolsLine, actionWalkHistoryPrevious);
   insertActionAsButton(extraToolsLine, actionWalkHistoryNext);
 }
 insertActionAsButton(extraToolsLine, actionFindInBase);

 treePathLabel=new QLabel(this);
 treePathLabel->setWordWrap(true);
 if(mytetraConfig.getInterfaceMode()=="desktop")
   treePathLabel->hide();
}


void RecordTableScreen::setupShortcuts(void)
{
    QString actionName, info;
    ShortcutManager::stringRepresentation mode=ShortcutManager::stringRepresentation::brackets;

    actionName="note-previousNote";
    info=tr("Previous note")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    actionWalkHistoryPrevious->setShortcut(shortcutManager.getKeySequence(actionName));
    actionWalkHistoryPrevious->setStatusTip(info+" - Previous note has been viewing");
    actionWalkHistoryPrevious->setToolTip(info);

    actionName="note-nextNote";
    info=tr("Next note")+" "+shortcutManager.getKeySequenceHumanReadable(actionName, mode);
    actionWalkHistoryNext->setShortcut(shortcutManager.getKeySequence(actionName));
    actionWalkHistoryNext->setStatusTip(info+" - Next note has been viewing");
    actionWalkHistoryNext->setToolTip(info);
}


void RecordTableScreen::setupSignals(void)
{
    // Добавление записи
    connect(actionAddNewToEnd, &QAction::triggered, recordTableController, &RecordTableController::addNewToEndContext);

    // Добавление записи до
    connect(actionAddNewBefore, &QAction::triggered, recordTableController, &RecordTableController::addNewBeforeContext);

    // Добавление записи после
    connect(actionAddNewAfter, &QAction::triggered, recordTableController, &RecordTableController::addNewAfterContext);

    // Редактирование записи
    connect(actionEditField, &QAction::triggered, recordTableController, &RecordTableController::onEditFieldContext);

    // Блокировка записи
    connect(actionBlock, &QAction::triggered, recordTableController, &RecordTableController::onBlockContext);

    // Удаление записи
    connect(actionDelete, &QAction::triggered, recordTableController, &RecordTableController::deleteContext);

    // Удаление записи с копированием в буфер обмена
    connect(actionCut, &QAction::triggered, recordTableController, &RecordTableController::cut);

    // Копирование записи (записей) в буфер обмена
    connect(actionCopy, &QAction::triggered, recordTableController, &RecordTableController::copy);

    // Вставка записи из буфера обмена
    connect(actionPaste, &QAction::triggered, recordTableController, &RecordTableController::paste);

    // Настройка внешнего вида таблицы конечных записей
    connect(actionSettings, &QAction::triggered, recordTableController, &RecordTableController::settings);

    // Перемещение записи вверх
    connect(actionMoveUp, &QAction::triggered, recordTableController, &RecordTableController::moveUp);

    // Перемещение записи вниз
    connect(actionMoveDn, &QAction::triggered, recordTableController, &RecordTableController::moveDn);

    // Поиск по базе (клик связывается с действием в MainWindow)
    // todo: разобраться с коннектом, пока коннекта нет

    // Синхронизация
    connect(actionSyncro, &QAction::triggered, this, &RecordTableScreen::onSyncroClick);

    // Перемещение по истории посещаемых записей назад
    connect(actionWalkHistoryPrevious, &QAction::triggered, this, &RecordTableScreen::onWalkHistoryPreviousClick);

    // Перемещение по истории посещаемых записей вперед
    connect(actionWalkHistoryNext, &QAction::triggered, this, &RecordTableScreen::onWalkHistoryNextClick);

    // Кнопка Назад (Back) в мобильном интерфейсе
    connect(actionBack, &QAction::triggered, this, &RecordTableScreen::onBackClick);

    // Действия по сортировке
    connect(actionSort, &QAction::triggered, recordTableController, &RecordTableController::onSortClick);

    // Кнопка вызова печати таблицы конечных записей
    connect(actionPrint, &QAction::triggered, recordTableController, &RecordTableController::onPrintClick);

    // Кнопка копирования ссылки на запись
    connect(actionCopyRecordReference, &QAction::triggered, this, &RecordTableScreen::onCopyRecordReference);

    // Кнопка переключения режима одинарного выбора и мультивыбора
    connect(actionSwitchSelectionMode, &QAction::triggered, recordTableController, &RecordTableController::onSwitchSelectionMode);
}


void RecordTableScreen::assembly(void)
{
 recordTableToolsLayout=new QHBoxLayout();
 recordTableToolsLayout->addWidget(toolsLine);
 recordTableToolsLayout->addStretch();
 recordTableToolsLayout->addWidget(extraToolsLine);
 
 recordTableScreenLayout=new QVBoxLayout();
 recordTableScreenLayout->setObjectName("recordtablescreen_QVBoxLayout");

 recordTableScreenLayout->addLayout(recordTableToolsLayout);
 recordTableScreenLayout->addWidget(treePathLabel);
 recordTableScreenLayout->addWidget(recordTableController->getView());

 setLayout(recordTableScreenLayout);

 // Границы убираются, так как данный объект будет использоваться как виджет
 QLayout *lt;
 lt=layout();
 lt->setContentsMargins(0,2,0,0);
}


// Отключение всех действий над записями
// (но не всех действий на панели инструментов, так как на панели инструментов есть действия, не оказывающие воздействия на записи)
void RecordTableScreen::disableAllActions(void)
{
 actionAddNewToEnd->setEnabled(false);
 actionAddNewBefore->setEnabled(false);
 actionAddNewAfter->setEnabled(false);
 actionEditField->setEnabled(false);
 actionBlock->setEnabled(false);
 actionDelete->setEnabled(false);

 actionCut->setEnabled(false);
 actionCopy->setEnabled(false);
 actionPaste->setEnabled(false);

 actionMoveUp->setEnabled(false);
 actionMoveDn->setEnabled(false);

 actionCopyRecordReference->setEnabled(false);

 actionSwitchSelectionMode->setEnabled(false);
}


void RecordTableScreen::toolsUpdate(void)
{
  toolsWidgatsUpdate();
  editorModesUpdate();
}


void RecordTableScreen::toolsWidgatsUpdate()
{
 // qDebug() << "recordtablescreen::tools_update()";

 // Отключаются все действия
 disableAllActions();

 if(recordTableController->isTableNotExists())
  return;

 // Выясняется, содержит ли текущая ветка подчиненные ветки
 /*
 QModelIndex index = find_object<TreeScreen>("treeScreen")->get_selection_model()->currentIndex();
 TreeItem *item = find_object<TreeScreen>("treeScreen")->kntrmodel->getItem(index);
 int branch_have_children=0;
 if(item->childCount()>0)branch_have_children=1;
 */

 // Включаются те действия которые разрешены

 // Добавление записи
 // Добавлять можно к любой ветке
 actionAddNewToEnd->setEnabled(true);

 // Добавление записи до
 // Добавлять "до" можно только тогда, когда выбрана только одна строка
 // и не включена сортировка
 if(recordTableController->getView()->selectionModel()->hasSelection() &&
    (recordTableController->getView()->selectionModel()->selectedRows()).size()==1 &&
    recordTableController->getView()->isSortingEnabled()==false )
   actionAddNewBefore->setEnabled(true);

 // Добавление записи после
 // Добавлять "после" можно только тогда, когда выбрана только одна строка
 // и не включена сортировка
 if(recordTableController->getView()->selectionModel()->hasSelection() &&
    (recordTableController->getView()->selectionModel()->selectedRows()).size()==1 &&
    recordTableController->getView()->isSortingEnabled()==false )
   actionAddNewAfter->setEnabled(true);

 // Редактирование записи и получение ссылки на запись
 // Редактировать можно только тогда, когда выбрана только одна строка
 if(recordTableController->getView()->selectionModel()->hasSelection() &&
    (recordTableController->getView()->selectionModel()->selectedRows()).size()==1)
 {
   actionEditField->setEnabled(true);
   actionBlock->setEnabled(true);
   actionCopyRecordReference->setEnabled(true);
 }

 // Удаление записи
 // Пункт активен только если запись (или записи) выбраны в списке
 if(recordTableController->getView()->selectionModel()->hasSelection())
  actionDelete->setEnabled(true);

 // Удаление с копированием записи в буфер обмена
 // Пункт активен только если запись (или записи) выбраны в списке
 if(recordTableController->getView()->selectionModel()->hasSelection())
  actionCut->setEnabled(true);

 // Копирование записи в буфер обмена
 // Пункт активен только если запись (или записи) выбраны в списке
 if(recordTableController->getView()->selectionModel()->hasSelection())
  actionCopy->setEnabled(true);

 // Вставка записи из буфера обмена
 // Вставлять записи можно только тогда, когда выбрана
 // только одна строка (добавляется после выделеной строки)
 // или не выбрано ни одной строки (тогда добавляется в конец списка)
 // или записей вообще нет
 // И проверяется, содержит ли буфер обмена данные нужного формата
 if((recordTableController->getView()->selectionModel()->hasSelection() && (recordTableController->getView()->selectionModel()->selectedRows()).size()==1) ||
     recordTableController->getView()->selectionModel()->hasSelection()==false ||
     recordTableController->getView()->model()->rowCount()==0)
  {
   const QMimeData *mimeData=QApplication::clipboard()->mimeData();
   if(mimeData!=NULL)
    if(mimeData->hasFormat(FixedParameters::appTextId+"/records"))
     actionPaste->setEnabled(true);
  }

 // Перемещение записи вверх
 // Пункт возможен только когда выбрана одна строка
 // и указатель стоит не на начале списка
 // и не включена сортировка
 if(recordTableController->getView()->selectionModel()->hasSelection() &&
    (recordTableController->getView()->selectionModel()->selectedRows()).size()==1 &&
    recordTableController->getView()->isSelectedSetToTop()==false &&
    recordTableController->getView()->isSortingEnabled()==false)
  actionMoveUp->setEnabled(true);

 // Перемещение записи вниз
 // Пункт возможен только когда выбрана одна строка
 // и указатель стоит не в конце списка
 // и не включена сортировка
 if(recordTableController->getView()->selectionModel()->hasSelection() &&
    (recordTableController->getView()->selectionModel()->selectedRows()).size()==1 &&
    recordTableController->getView()->isSelectedSetToBottom()==false &&
    recordTableController->getView()->isSortingEnabled()==false)
  actionMoveDn->setEnabled(true);

 // Переключение между режимами выбора в списке записей возможно всегда
 actionSwitchSelectionMode->setEnabled(true);
}


// Обновление режимов редактора согласно состоянию записи в таблице
void RecordTableScreen::editorModesUpdate()
{
 // Если ни одна запись не выбрана
 if( !(recordTableController->getView()->selectionModel()->hasSelection() &&
       recordTableController->getRowCount()>0) )
 {
   qDebug() << "In table select non present";

   // Блокировка возможности редактирования текста записи (разблокировка произойдет при клике на запись)
   find_object<MetaEditor>("editorScreen")->setTextareaEditable(false);
 }
}


// Получение номера первого выделенного элемента в таблице записи на экране
int RecordTableScreen::getFirstSelectionPos(void)
{
  return recordTableController->getFirstSelectionPos();
}


// Получение ID первого выделенного элемента в таблице записи на экране
QString RecordTableScreen::getFirstSelectionId(void)
{
  return recordTableController->getFirstSelectionId();
}


// Установка засветки в нужную строку в таблице записи на экране
void RecordTableScreen::setSelectionToPos(int pos)
{
  recordTableController->setSelectionToPos(pos);
}


// Установка засветки в нужную запись в таблице записей на экране
void RecordTableScreen::setSelectionToId(QString id)
{
  recordTableController->setSelectionToId(id);
}



// Действия при нажатии кнопки синхронизации
void RecordTableScreen::onSyncroClick(void)
{
  find_object<MainWindow>("mainwindow")->synchronization();
}


void RecordTableScreen::onExecuteCommandFinishWork(void)
{
  // Разблокируется кнопка синхронизации
  actionSyncro->setEnabled(true);
}


void RecordTableScreen::onWalkHistoryPreviousClick(void)
{
  find_object<MainWindow>("mainwindow")->goWalkHistoryPrevious();
}


void RecordTableScreen::onWalkHistoryNextClick(void)
{
  find_object<MainWindow>("mainwindow")->goWalkHistoryNext();
}


// Возвращение к дереву разделов в мобильном интерфейсе
void RecordTableScreen::onBackClick(void)
{
  globalParameters.getWindowSwitcher()->switchFromRecordtableToTree();
}


// Копирование в буфер обмена ссылки на запись
void RecordTableScreen::onCopyRecordReference()
{
  QString reference=FixedParameters::appTextId+"://note/"+getFirstSelectionId();

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(reference);
}


void RecordTableScreen::setTreePath(QString path)
{
  treePath=path; // Запоминается путь к ветке в виде строки
  treePathLabel->setText(tr("<b>Path:</b> ")+treePath);
}


QString RecordTableScreen::getTreePath(void)
{
  return treePath;
}


