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
#include "libraries/WindowSwitcher.h"
#include "libraries/GlobalParameters.h"
#include "controllers/recordTable/RecordTableController.h"


extern GlobalParameters globalParameters;
extern AppConfig mytetraConfig;


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
    connect(actionAddNewToEnd, SIGNAL(triggered()), recordTableController, SLOT(addNewToEndContext()));

// Добавление записи до
    actionAddNewBefore = new QAction(tr("Add note before"), this);
    actionAddNewBefore->setStatusTip(tr("Add a note before selected"));
    connect(actionAddNewBefore, SIGNAL(triggered()), recordTableController, SLOT(addNewBeforeContext()));

// Добавление записи после
    actionAddNewAfter = new QAction(tr("Add note after"), this);
    actionAddNewAfter->setStatusTip(tr("Add a note after selected"));
    connect(actionAddNewAfter, SIGNAL(triggered()), recordTableController, SLOT(addNewAfterContext()));

// Редактирование записи
    actionEditField = new QAction(tr("Edit properties (name, author, tags...)"), this);
    actionEditField->setStatusTip(tr("Edit note properties (name, author, tags...)"));
    actionEditField->setIcon(QIcon(":/resource/pic/note_edit.svg"));
    connect(actionEditField, SIGNAL(triggered()), recordTableController, SLOT(onEditFieldContext()));

// Удаление записи
    actionDelete = new QAction(tr("Delete note(s)"), this);
    actionDelete->setStatusTip(tr("Delete note(s)"));
    actionDelete->setIcon(QIcon(":/resource/pic/note_delete.svg"));
    connect(actionDelete, SIGNAL(triggered()), recordTableController, SLOT(deleteContext()));

// Удаление записи с копированием в буфер обмена
    actionCut = new QAction(tr("&Cut note(s)"), this);
    actionCut->setStatusTip(tr("Cut notes(s) to clipboard"));
    actionCut->setIcon(QIcon(":/resource/pic/cb_cut.svg"));
    connect(actionCut, SIGNAL(triggered()), recordTableController, SLOT(cut()));

// Копирование записи (записей) в буфер обмена
    actionCopy = new QAction(tr("&Copy note(s)"), this);
    actionCopy->setStatusTip(tr("Copy note(s) to clipboard"));
    actionCopy->setIcon(QIcon(":/resource/pic/cb_copy.svg"));
    connect(actionCopy, SIGNAL(triggered()), recordTableController, SLOT(copy()));

// Вставка записи из буфера обмена
    actionPaste = new QAction(tr("&Paste note(s)"), this);
    actionPaste->setStatusTip(tr("Paste note(s) from clipboard"));
    actionPaste->setIcon(QIcon(":/resource/pic/cb_paste.svg"));
    connect(actionPaste, SIGNAL(triggered()), recordTableController, SLOT(paste()));

// Настройка внешнего вида таблицы конечных записей
    actionSettings = new QAction(tr("&View settings"), this);
    actionSettings->setStatusTip(tr("Setup table view settins"));
    connect(actionSettings, SIGNAL(triggered()), recordTableController, SLOT(settings()));

// Перемещение записи вверх
    actionMoveUp = new QAction(tr("&Move Up"), this);
    actionMoveUp->setStatusTip(tr("Move note up"));
    actionMoveUp->setIcon(QIcon(":/resource/pic/move_up.svg"));
    connect(actionMoveUp, SIGNAL(triggered()), recordTableController, SLOT(moveUp()));

// Перемещение записи вниз
    actionMoveDn=new QAction(tr("&Move Down"), this);
    actionMoveDn->setStatusTip(tr("Move note down"));
    actionMoveDn->setIcon(QIcon(":/resource/pic/move_dn.svg"));
    connect(actionMoveDn, SIGNAL(triggered()), recordTableController, SLOT(moveDn()));

// Поиск по базе (клик связывается с действием в MainWindow)
    actionFindInBase=new QAction(tr("Find in base"), this);
    actionFindInBase->setStatusTip(tr("Find in base"));
    actionFindInBase->setIcon(QIcon(":/resource/pic/find_in_base.svg"));

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

// Кнопка Назад (Back) в мобильном интерфейсе
    actionBack=new QAction(tr("Back to item tree"), this);
    actionBack->setStatusTip(tr("Back to item tree"));
    actionBack->setIcon(QIcon(":/resource/pic/mobile_back.svg"));
    connect(actionBack, SIGNAL(triggered()), this, SLOT(onBackClick()));

// Действия по сортировке
    actionSort = new QAction(tr("Toggle sorting"), this);
    actionSort->setStatusTip(tr("Enable/disable sorting by column"));
    actionSort->setIcon(QIcon(":/resource/pic/sort.svg"));
    connect(actionSort, SIGNAL(triggered()), recordTableController, SLOT(onSortClick()));

// Кнопка вызова печати таблицы конечных записей
    actionPrint = new QAction(tr("Print table"), this);
    actionPrint->setStatusTip(tr("Print current notes table"));
    actionPrint->setIcon(QIcon(":/resource/pic/print_record_table.png"));   //actionPrint->setIcon(QIcon(":/resource/pic/print_record_table.svg"));
    connect(actionPrint, SIGNAL(triggered()), recordTableController, SLOT(onPrintClick()));

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

    if(mytetraConfig.getInterfaceMode()=="mobile") {
        insertActionAsButton(toolsLine, actionBack);
        toolsLine->addSeparator();
    }

    insertActionAsButton(toolsLine, actionAddNewToEnd);
    if(mytetraConfig.getInterfaceMode()=="desktop") {
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

    if(mytetraConfig.getInterfaceMode()=="desktop") {
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


void RecordTableScreen::setupSignals(void)
{

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

// Редактирование записи
// Редактировать можно только тогда, когда выбрана только одна строка
    if(recordTableController->getView()->selectionModel()->hasSelection() &&
            (recordTableController->getView()->selectionModel()->selectedRows()).size()==1)
        actionEditField->setEnabled(true);

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
            recordTableController->getView()->model()->rowCount()==0) {
        const QMimeData *mimeData=QApplication::clipboard()->mimeData();
        if(mimeData!=NULL)
            if(mimeData->hasFormat("mytetra/records"))
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

// Обновляется состояние области редактирования текста
    if(recordTableController->getView()->selectionModel()->hasSelection() &&
            recordTableController->getRowCount()>0) {
        qDebug() << "In table select present";
        qDebug() << "In table row count is" << recordTableController->getRowCount();
        find_object<MetaEditor>("editorScreen")->set_textarea_editable(true);
    } else {
        qDebug() << "In table select non present";
        find_object<MetaEditor>("editorScreen")->set_textarea_editable(false);
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


void RecordTableScreen::setTreePath(QString path)
{
    treePath=path; // Запоминается путь к ветке в виде строки
    treePathLabel->setText(tr("<b>Path:</b> ")+treePath);
}


QString RecordTableScreen::getTreePath(void)
{
    return treePath;
}


