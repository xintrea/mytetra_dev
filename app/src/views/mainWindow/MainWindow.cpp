#include <QString>
#include <QDir>
#include <QString>

#include "main.h"
#include "models/appConfig/AppConfig.h"
#include "MainWindow.h"
#include "views/printPreview/PrintPreview.h"
#include "views/appConfigWindow/AppConfigDialog.h"
#include "libraries/TrashMonitoring.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "views/recordTable/RecordTableScreen.h"
#include "models/tree/TreeItem.h"
#include "views/findInBaseScreen/FindScreen.h"
#include "models/tree/KnowTreeModel.h"
#include "libraries/GlobalParameters.h"
#include "views/consoleEmulator/CommandRun.h"
#include "libraries/WalkHistory.h"
#include "libraries/ActionLogger.h"
#include "libraries/WindowSwitcher.h"
#include "views/actionLog/ActionLogScreen.h"
#include "views/databasesManagement/DatabasesManagementScreen.h"
#include "libraries/ShortcutManager.h"
#include "libraries/helpers/ObjectHelper.h"
#include "libraries/wyedit/EditorTextArea.h"
#include "libraries/wyedit/EditorShowTextDispatcher.h"


extern AppConfig mytetraConfig;
extern TrashMonitoring trashMonitoring;
extern GlobalParameters globalParameters;
extern WalkHistory walkHistory;
extern ActionLogger actionLogger;
extern ShortcutManager shortcutManager;


MainWindow::MainWindow() : QMainWindow()
{
    extern QObject *pMainWindow;
    pMainWindow=this;
    setObjectName("mainwindow");

    installEventFilter(this);

    setupUI();
    initFileMenu();
    initToolsMenu();
    initHelpMenu();
    initHiddenActions();
    setupSignals();
    assembly();

    setupShortcuts();

    setupIconActions();
    createTrayIcon();
    setIcon();
    if(QSystemTrayIcon::isSystemTrayAvailable())
        trayIcon->show();

    // Инициализируется объект слежения за корзиной
    trashMonitoring.init(mytetraConfig.get_trashdir());
    trashMonitoring.update();

    // Закрывать ли по-настоящему окно при обнаружении сигнала closeEvent
    enableRealClose=false;

    srand(time(NULL));
}


MainWindow::~MainWindow()
{
    saveAllState();

    // В лог записывается событие остановки программы
    actionLogger.addAction("stopProgram");
}


void MainWindow::setupUI(void)
{
    // При создании объектов не указывается parent, так как он буден задан в момент вставки в layout в методе assembly()

    treeScreen=new TreeScreen( this );
    treeScreen->setObjectName("treeScreen");
    globalParameters.setTreeScreen(treeScreen);

    recordTableScreen=new RecordTableScreen( this );
    recordTableScreen->setObjectName("recordTableScreen");
    globalParameters.setRecordTableScreen(recordTableScreen);

    findScreenDisp=new FindScreen( this );
    findScreenDisp->setObjectName("findScreenDisp");
    globalParameters.setFindScreen(findScreenDisp);

    editorScreen=new MetaEditor( this );
    editorScreen->setObjectName("editorScreen");
    globalParameters.setMetaEditor(editorScreen);

    statusBar=new QStatusBar( this );
    statusBar->setObjectName("statusBar");
    setStatusBar(statusBar);
    globalParameters.setStatusBar(statusBar);

    // Вспомогательный объект переключения окон, используется в мобильном интерфейсе
    windowSwitcher=new WindowSwitcher( this );
    windowSwitcher->setObjectName("windowSwitcher");
    globalParameters.setWindowSwitcher(windowSwitcher);

    // Вспомогательный объект с виджетом синхронизации базы знаний
    synchroCommandRun=new CommandRun( this );

    // todo: Для проверки, почему то в этом месте поиск объекта по имени не работает, разобраться.
    // MetaEditor *edView=find_object<MetaEditor>("editorScreen");
}


void MainWindow::setupSignals(void)
{
    connect(editorScreen, &MetaEditor::send_expand_edit_area, this, &MainWindow::onExpandEditArea);

    // Сигнал, генерирующийся при выходе из оконных систем X11 и Windows
    connect(qApp, &QApplication::commitDataRequest, this, &MainWindow::commitData);

    connect(qApp, &QApplication::focusChanged, this, &MainWindow::onFocusChanged);


    // Сигналы пунктов меню
    connect(actionFileMenuDatabasesManagement, &QAction::triggered, this, &MainWindow::fileDatabasesManagement);
    connect(actionFileMenuExportTreeItem, &QAction::triggered, this, &MainWindow::fileExportBranch);
    connect(actionFileMenuImportTreeItem, &QAction::triggered, this, &MainWindow::fileImportBranch);
    connect(actionFileMenuPrint, &QAction::triggered, this, &MainWindow::filePrint);
    connect(actionFileMenuPrintPreview, &QAction::triggered, this, &MainWindow::filePrintPreview);
    connect(actionFileMenuExportPdf, &QAction::triggered, this, &MainWindow::filePrintPdf);
    connect(actionFileMenuQuit, &QAction::triggered, this, &MainWindow::applicationExit);


    // Поиск по базе в стандартном меню
    connect(actionToolsMenuFindInBase, &QAction::triggered, this, &MainWindow::toolsFindInBase);
    if(mytetraConfig.getInterfaceMode()=="mobile")
    {
        // Кнопка поиска по базе в меню дерева разделов для мобильного интерфейса
        connect(treeScreen, &TreeScreen::treeScreenFindInBaseClicked, this, &MainWindow::toolsFindInBase);

        // Кнопка поиска по базе в меню редактора для мобильного интефейса
        connect(editorScreen, &MetaEditor::wyeditFindInBaseClicked, this, &MainWindow::toolsFindInBase);
    }

    // Вызов окна просмотра лога
    connect(actionToolsMenuActionLog, &QAction::triggered, this, &MainWindow::onActionLogClicked);

    // Вызов окна настроек
    if(mytetraConfig.getInterfaceMode()=="desktop")
    {
        connect(actionToolsMenuPreferences, &QAction::triggered, this, &MainWindow::toolsPreferences);
    }
    else
    {
        connect(actionDirectPreferencesMain, &QAction::triggered,
                this, [this](){ runDirectPreferences(actionDirectPreferencesMain); } );

        connect(actionDirectPreferencesAppearance, &QAction::triggered,
                this, [this](){ runDirectPreferences(actionDirectPreferencesAppearance); } );

        connect(actionDirectPreferencesCrypt, &QAction::triggered,
                this, [this](){ runDirectPreferences(actionDirectPreferencesCrypt); } );

        connect(actionDirectPreferencesSyncro, &QAction::triggered,
                this, [this](){ runDirectPreferences(actionDirectPreferencesSyncro); } );

        connect(actionDirectPreferencesRecordTable, &QAction::triggered,
                this, [this](){ runDirectPreferences(actionDirectPreferencesRecordTable); } );

        connect(actionDirectPreferencesAttach, &QAction::triggered,
                this, [this](){ runDirectPreferences(actionDirectPreferencesAttach); } );

        connect(actionDirectPreferencesKeyboard, &QAction::triggered,
                this, [this](){ runDirectPreferences(actionDirectPreferencesKeyboard); } );

        connect(actionDirectPreferencesHistory, &QAction::triggered,
                this, [this](){ runDirectPreferences(actionDirectPreferencesHistory); } );

        connect(actionDirectPreferencesMisc, &QAction::triggered,
                this, [this](){ runDirectPreferences(actionDirectPreferencesMisc); } );
    }

    connect(actionHelpMenuAboutMyTetra, &QAction::triggered, this, &MainWindow::onClickHelpAboutMyTetra);
    connect(actionHelpMenuAboutQt, &QAction::triggered, this, &MainWindow::onClickHelpAboutQt);
    connect(actionHelpMenuTechnicalInfo, &QAction::triggered, this, &MainWindow::onClickHelpTechnicalInfo);

    connect(actionFocusTree, &QAction::triggered, this, &MainWindow::onClickFocusTree);
    connect(actionFocusNoteTable, &QAction::triggered, this, &MainWindow::onClickFocusNoteTable);
    connect(actionFocusEditor, &QAction::triggered, this, &MainWindow::onClickFocusEditor);

    // Связывание сигнала окончания выполнения команды синхронизации со слотом, срабатывающем при завершении выполнения команды
    connect(synchroCommandRun, &CommandRun::finishWork,
            recordTableScreen, &RecordTableScreen::onSynchroCommandFinishWork);
    connect(synchroCommandRun, &CommandRun::finishWork,
            this, &MainWindow::onSynchroCommandFinishWork);

    // Связывание сигнала вызова обработки открепляемых окон на предмет того,
    // что они отображают существующие записи
    connect(this, &MainWindow::doUpdateDetachedWindows,
            EditorShowTextDispatcher::instance(), &EditorShowTextDispatcher::closeWindowForNonExistentRecords,
            Qt::QueuedConnection);

    // Обновление горячих клавиш, если они были изменены
    connect(&shortcutManager, &ShortcutManager::updateWidgetShortcut, this, &MainWindow::setupShortcuts);
}


void MainWindow::assembly(void)
{
    vSplitter=new QSplitter(Qt::Vertical);
    vSplitter->addWidget(recordTableScreen); // Список конечных записей
    vSplitter->addWidget(editorScreen);      // Текст записи
    vSplitter->setCollapsible(0,false); // Список конечных записей не может смыкаться
    vSplitter->setCollapsible(1,false); // Содержимое записи не может смыкаться

    hSplitter=new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(treeScreen); // Дерево веток
    hSplitter->addWidget(vSplitter);
    hSplitter->setCollapsible(0,false); // Дерево веток не может смыкаться
    hSplitter->setCollapsible(1,false); // Столбец со списком и содержимым записи не может смыкаться

    findSplitter=new QSplitter(Qt::Vertical);
    findSplitter->addWidget(hSplitter);
    findSplitter->addWidget(findScreenDisp);
    findSplitter->setCollapsible(0,false); // Верхняя часть не должна смыкаться
    findSplitter->setCollapsible(1,false); // Часть для поиска не должна смыкаться
    findSplitter->setObjectName("findsplitter");

    setCentralWidget(findSplitter);
}


void MainWindow::saveAllState(void)
{
    // Сохранение данных в поле редактирования
    saveTextarea();

    // Сохраняются данные сеанса работы
    saveWindowGeometry();
    saveTreePosition();
    saveRecordTablePosition();
    saveEditorCursorPosition();
    saveEditorScrollBarPosition();

    // Синхронизируется с диском конфиг программы
    mytetraConfig.sync();
}


// Слот, срабатывающий когда происходит выход из оконной системы
void MainWindow::commitData(QSessionManager& manager)
{
    Q_UNUSED(manager)
    qDebug() << "Session manager send commit data signal.";

    applicationFastExit();
}


// Слот, обрабатывающий внешние сообщения
// Используется для обработки ссобщений, приходящих от других экземпляров MyTetra
void MainWindow::messageHandler(QString message)
{
    qDebug() << "MainWindow recieved message: "+message;

    if(message=="show")
    {
        showWindow();
    }

    if(message=="hide")
    {
        hide();
    }

    if(message=="quit")
    {
        applicationExit();
    }

    else if(message=="reload")
    {
        reload();
    }

    else if(message.split(" ").at(0)=="openNote")
    {
        QString recordId=message.split(" ").at(1);

        // Если аргумент опции --openNote не обнаружен
        if(recordId.length()==0)
            return;

        // Нахождение ветки, в которой лежит запись с указанным идентификатором
        QStringList pathToRecord=treeScreen->knowTreeModel->getRecordPath(recordId);

        // Установка курсора в дереве и в таблице конечных записей
        setTreePosition( pathToRecord );
        setRecordtablePositionById( recordId );
    }

    else if(message=="addNoteDialog")
    {
        // Определение, было ли окно MyTetra скрыто при обработке сообщения
        bool isHidden=this->isHidden();

        // Окно показыается, чтобы небыло некорректного выхода QtSingleApplication при свернутом окне после добавления записи
        if(isHidden) {
            this->showWindow();
        }

        // Нажимается кнопка добавления записи
        if( recordTableScreen->actionAddNewToEnd->isEnabled() ) {
            recordTableScreen->actionAddNewToEnd->trigger();
        } else {
            qDebug() << "Can not initial add new note dialog. Add new note action is disable now. Try again.";
        }

        // Окно скрывается
        if(isHidden) {
            this->hide();
        }
    }

    else if(message.split(" ").at(0)=="openTreeItem")
    {
        QString branchId=message.split(" ").at(1);

        // Если аргумент опции --openTreeItem (устаревшее --openBranch) не обнаружен
        if(branchId.length()==0)
            return;

        // Установка курсора в дереве
        treeScreen->setCursorToId( branchId );
    }
}


// Восстанавливается геометрия окна и позиции основных разделителей
void MainWindow::restoreWindowGeometry(void)
{
    if(globalParameters.getTargetOs()=="android")
        setWindowState(Qt::WindowMaximized); // Для Андроида окно просто разворачивается на весь экран
    else
        restoreGeometry( mytetraConfig.get_mainwingeometry() );

    vSplitter->setSizes(mytetraConfig.get_vspl_size_list());
    hSplitter->setSizes(mytetraConfig.get_hspl_size_list());
    findSplitter->setSizes(mytetraConfig.get_findsplitter_size_list());
}


// Запоминается геометрия окна и позиции основных разделителей
void MainWindow::saveWindowGeometry(void)
{
    qDebug() << "Save window geometry and splitter sizes";

    mytetraConfig.set_mainwingeometry( saveGeometry() );

    mytetraConfig.set_vspl_size_list(vSplitter->sizes());
    mytetraConfig.set_hspl_size_list(hSplitter->sizes());


    // Запоминается размер сплиттера только при видимом виджете поиска,
    // т.к. если виджета поиска невидно, будет запомнен нулевой размер

    // if(findScreenDisp->isVisible()) - так делать нельзя, т.к.
    // данный метод вызывается из декструктора главного окна, и к этому моменту
    // виджет уже невиден

    if(mytetraConfig.get_findscreen_show())
        mytetraConfig.set_findsplitter_size_list(findSplitter->sizes());
}


void MainWindow::restoreTreePosition(void)
{
    // Путь к последнему выбранному в дереве элементу
    QStringList path=mytetraConfig.get_tree_position();

    qDebug() << "MainWindow::restoreTreePosition() : " << path;

    setTreePosition(path);
}


void MainWindow::saveTreePosition(void)
{
    // Получение QModelIndex выделенного в дереве элемента
    QModelIndex index=treeScreen->getCurrentItemIndex();

    // Получаем указатель вида TreeItem
    TreeItem *item =treeScreen->knowTreeModel->getItem(index);

    // Сохраняем путь к элементу item
    mytetraConfig.set_tree_position(item->getPath());
}


void MainWindow::setTreePosition(QStringList path)
{
    if(treeScreen->knowTreeModel->isItemValid(path)==false)
        return;

    // Получаем указатель на элемент вида TreeItem, используя путь, этот указатель может вернуть и корень (если путь пустой)
    TreeItem *item =treeScreen->knowTreeModel->getItem(path);

    // qDebug() << "Set tree position to " << item->getField("name") << " id " << item->getField("id");

    // Из указателя на элемент TreeItem получаем QModelIndex
    QModelIndex setto=treeScreen->knowTreeModel->getIndexByItem(item);

    // Курсор устанавливается в нужную позицию
    treeScreen->setCursorToIndex(setto);
}


bool MainWindow::isTreePositionCrypt()
{
    QStringList path=mytetraConfig.get_tree_position();

    if(treeScreen->knowTreeModel->isItemValid(path)==false)
        return false;

    // Получаем указатель на элемент вида TreeItem, используя путь
    TreeItem *item =treeScreen->knowTreeModel->getItem(path);

    if(item->getField("crypt")=="1")
        return true;
    else
        return false;
}


void MainWindow::restoreRecordTablePosition(void)
{
    QString id=mytetraConfig.get_recordtable_selected_record_id();

    if(id.length()>0)
        setRecordtablePositionById(id);
}


void MainWindow::saveRecordTablePosition(void)
{
    QString id=recordTableScreen->getFirstSelectionId();

    mytetraConfig.set_recordtable_selected_record_id(id);
}


void MainWindow::setRecordtablePositionById(QString id)
{
    recordTableScreen->setSelectionToId(id);
}


void MainWindow::saveEditorCursorPosition(void)
{
    int n=editorScreen->getCursorPosition();

    mytetraConfig.setEditorCursorPosition(n);
}


void MainWindow::restoreEditorCursorPosition(void)
{
    int n=mytetraConfig.getEditorCursorPosition();

    editorScreen->setCursorPosition(n);
}


void MainWindow::saveEditorScrollBarPosition(void)
{
    int n=editorScreen->getScrollBarPosition();

    mytetraConfig.setEditorScrollBarPosition(n);
}


void MainWindow::restoreEditorScrollBarPosition(void)
{
    int n=mytetraConfig.getEditorScrollBarPosition();

    editorScreen->setScrollBarPosition(n);
}


void MainWindow::restoreFindOnBaseVisible(void)
{
    bool n=mytetraConfig.get_findscreen_show();

    // Определяется ссылка на виджет поиска
    FindScreen *findScreenRel=find_object<FindScreen>("findScreenDisp");

    if(n)
        findScreenRel->show();
    else
        findScreenRel->hide();
}


void MainWindow::restoreAllWindowState(void)
{
    globalParameters.getWindowSwitcher()->disableSwitch();

    restoreFindOnBaseVisible();
    restoreWindowGeometry();
    restoreTreePosition();
    restoreRecordTablePosition();
    restoreEditorCursorPosition();
    restoreEditorScrollBarPosition();

    globalParameters.getWindowSwitcher()->enableSwitch();
}


void MainWindow::restoreDockableWindowsState()
{
    // Восстанавливаются открепляемые окна
    EditorShowTextDispatcher::instance()->restoreOpenWindows();
}


// Создание раздела меню File
void MainWindow::initFileMenu(void)
{
    // Создание меню
    QMenu *menu = new QMenu(tr("&File"), this);
    this->menuBar()->addMenu(menu);

    actionFileMenuDatabasesManagement = new QAction(tr("Databases management"), this);
    menu->addAction(actionFileMenuDatabasesManagement);

    actionFileMenuExportTreeItem = new QAction(tr("Export tree item"), this);
    menu->addAction(actionFileMenuExportTreeItem);

    actionFileMenuImportTreeItem = new QAction(tr("Import tree item"), this);
    menu->addAction(actionFileMenuImportTreeItem);

    menu->addSeparator();

    actionFileMenuPrint = new QAction(tr("&Print..."), this);
    menu->addAction(actionFileMenuPrint);

    actionFileMenuPrintPreview = new QAction(tr("Print Preview..."), this);
    menu->addAction(actionFileMenuPrintPreview);

    actionFileMenuExportPdf = new QAction(tr("&Export PDF..."), this);
    menu->addAction(actionFileMenuExportPdf);

    menu->addSeparator();

    actionFileMenuQuit = new QAction(tr("&Quit"), this);
    menu->addAction(actionFileMenuQuit);
}


// Создание раздела меню Tools
void MainWindow::initToolsMenu(void)
{
    // Создание меню
    QMenu *menu = new QMenu(tr("&Tools"), this);
    this->menuBar()->addMenu(menu);

    actionToolsMenuFindInBase = new QAction(this); // Так как есть this, указатель не будет потерян основным окном
    menu->addAction(actionToolsMenuFindInBase);

    actionToolsMenuActionLog = new QAction(tr("Action &log"), this);
    menu->addAction(actionToolsMenuActionLog);

    menu->addSeparator();

    if(mytetraConfig.getInterfaceMode()=="desktop")
    {
        actionToolsMenuPreferences = new QAction(tr("&Preferences"), this);
        menu->addAction(actionToolsMenuPreferences);
    }
    else
    {
        // Создание подменю
        QMenu *subMenu = new QMenu(tr("&Preferences"), this);
        initPreferencesMenu(subMenu);
        menu->addMenu(subMenu);
    }
}


// Заполнение подраздела меню Preferences (для мобильного интерфейса)
void MainWindow::initPreferencesMenu(QMenu *menu)
{
    QMap< QString, QAction **> map;

    map[tr("Main")]      = &actionDirectPreferencesMain;
    map[tr("Appearance")]= &actionDirectPreferencesAppearance;
    map[tr("Crypt")]     = &actionDirectPreferencesCrypt;
    map[tr("Syncro")]    = &actionDirectPreferencesSyncro;
    map[tr("Note Area")] = &actionDirectPreferencesRecordTable;
    map[tr("Attaches")]  = &actionDirectPreferencesAttach;
    map[tr("Keyboard")]  = &actionDirectPreferencesKeyboard;
    map[tr("History")]   = &actionDirectPreferencesHistory;
    map[tr("Misc")]      = &actionDirectPreferencesMisc;

    for(auto pageName : map.keys())
    {
        *map[pageName] = new QAction(pageName, this);
        menu->addAction( *map[pageName] );
    }
}


void MainWindow::runDirectPreferences(QAction *action)
{
    QMap< QString, QAction *> map;

    map["pageMain"]       = actionDirectPreferencesMain;
    map["pageAppearance"] = actionDirectPreferencesAppearance;
    map["pageCrypt"]      = actionDirectPreferencesCrypt;
    map["pageSynchro"]    = actionDirectPreferencesSyncro;
    map["pageRecordTable"]= actionDirectPreferencesRecordTable;
    map["pageAttach"]     = actionDirectPreferencesAttach;
    map["pageKeyboard"]   = actionDirectPreferencesKeyboard;
    map["pageHistory"]    = actionDirectPreferencesHistory;
    map["pageMisc"]       = actionDirectPreferencesMisc;

    AppConfigDialog *dialog=nullptr;

    for(auto pageName : map.keys())
    {
        if( map[pageName]==action )
        {
            dialog=new AppConfigDialog(pageName, this);
            break;
        }
    }

    if(dialog!=nullptr)
    {
        dialog->setMenuListVisible(false);
        dialog->exec();
    }

    delete dialog;
}


// Создание раздела меню Help
void MainWindow::initHelpMenu(void)
{
    // Создание меню
    QMenu *menu = new QMenu(tr("&Help"), this);
    this->menuBar()->addMenu(menu);

    actionHelpMenuAboutMyTetra = new QAction(tr("About MyTetra"), this);
    menu->addAction(actionHelpMenuAboutMyTetra);

    actionHelpMenuAboutQt = new QAction(tr("About Qt"), this);
    menu->addAction(actionHelpMenuAboutQt);

    actionHelpMenuTechnicalInfo = new QAction(tr("Technical info"), this);
    menu->addAction(actionHelpMenuTechnicalInfo);
}


// Создание скрытых действий для работы прочих шорткатов уровня приложения
void MainWindow::initHiddenActions(void)
{
    // Действия добавляются в сам виджет
    // Для таких действий нет визуального представления, но горячие клавиши для них работают
    actionFocusTree = new QAction(this);
    this->addAction(actionFocusTree);

    actionFocusNoteTable = new QAction(this);
    this->addAction(actionFocusNoteTable);

    actionFocusEditor = new QAction(this);
    this->addAction(actionFocusEditor);
}


void MainWindow::setupShortcuts(void)
{
    qDebug() << "Setup shortcut for" << staticMetaObject.className();

    shortcutManager.initAction("misc-print", actionFileMenuPrint );
    shortcutManager.initAction("misc-exportPdf", actionFileMenuExportPdf );
    shortcutManager.initAction("misc-quit", actionFileMenuQuit );

    shortcutManager.initAction("misc-findInBase", actionToolsMenuFindInBase );

    shortcutManager.initAction("misc-focusTree", actionFocusTree );
    shortcutManager.initAction("misc-focusNoteTable", actionFocusNoteTable );
    shortcutManager.initAction("misc-focusEditor", actionFocusEditor );
}


// Сохранить текущую статью
bool MainWindow::fileSave(void)
{
    // todo: сделать функционал

    return true;
}


// Сохранить текущую статью как файл
bool MainWindow::fileSaveAs(void)
{
    // todo: сделать функционал
    return true;
}


// Напечатать текущую статью
void MainWindow::filePrint(void)
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    dlg->setWindowTitle(tr("Print Document"));

    if (dlg->exec() == QDialog::Accepted)
        editorScreen->getTextareaDocument()->print(&printer);

    delete dlg;
#endif
}


// Предпросмотр печати текущей статьи
void MainWindow::filePrintPreview(void)
{
    PrintPreview *preview = new PrintPreview(editorScreen->getTextareaDocument(), this);

    preview->setModal(true);
    preview->setAttribute(Qt::WA_DeleteOnClose);
    preview->show();
}


// Вывод текущей статьи в PDF файл
void MainWindow::filePrintPdf(void)
{
#ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
                                                    QString(), "*.pdf");
    if (!fileName.isEmpty())
    {
        if(QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        editorScreen->getTextareaDocument()->print(&printer);
    }
#endif
}


// Раздел меню File, управление базами данных
void MainWindow::fileDatabasesManagement(void)
{
    DatabasesManagementScreen databasesManagementScreen(this);
    databasesManagementScreen.exec();
}


// Раздел меню File, экспорт текущей ветки и всех ее подветок
void MainWindow::fileExportBranch(void)
{
    // Создается окно выбора директории назначения
    QFileDialog directorySelectDialog(this);
    directorySelectDialog.setFileMode(QFileDialog::Directory);
    directorySelectDialog.setWindowTitle(tr("Select an empty directory to export data"));
    directorySelectDialog.setDirectory("/");

    if( directorySelectDialog.exec() )
        if( !directorySelectDialog.directory().absolutePath().isEmpty() ) // Если была выбрана директория (а не нажат Cancel)
            treeScreen->exportBranchToDirectory( directorySelectDialog.directory().absolutePath() );
}


void MainWindow::fileImportBranch(void)
{
    // Создается окно выбора директории, откуда необходимо сделать импорт
    QFileDialog directorySelectDialog(this);
    directorySelectDialog.setFileMode(QFileDialog::Directory);
    directorySelectDialog.setWindowTitle(tr("Select directory to import data"));
    directorySelectDialog.setDirectory("/");

    if( directorySelectDialog.exec() )
        if( !directorySelectDialog.directory().absolutePath().isEmpty() ) // Если была выбрана директория (а не нажат Cancel)
            treeScreen->importBranchFromDirectory( directorySelectDialog.directory().absolutePath() );
}


// Слот - Нормальный выход из программы
void MainWindow::applicationExit(void)
{
    saveAllState();

    // Если происходит первая инициализация выхода из программы
    // И если в конфиге настроено, что нужно синхронизироваться при выходе
    // И задана команда синхронизации
    if(enableRealClose==false)
        if(mytetraConfig.get_synchroonexit())
            if(mytetraConfig.get_synchrocommand().trimmed().length()>0)
            {
                enableRealClose=true;
                synchronization(); // В конце синхронизации будет вызван слот onSynchroCommandFinishWork()
            }

    // Запуск выхода из программы
    enableRealClose=true;
    close();
}


// Быстрый выход из программы, без возможности синхронизации
void MainWindow::applicationFastExit(void)
{
    saveAllState();

    // Запуск выхода из программы
    enableRealClose=true;
    close();
}


void MainWindow::toolsFindInBase(void)
{
    // Определяется ссылка на виджет поиска
    FindScreen *findScreenRel=find_object<FindScreen>("findScreenDisp");

    if( !(findScreenRel->isVisible()) )
        findScreenRel->widgetShow();
    else
        findScreenRel->widgetHide();
}


void MainWindow::toolsPreferences(void)
{
    AppConfigDialog dialog("", this); // this нужен чтобы пробрасывать иконку приложения
    dialog.exec();
}


void MainWindow::onActionLogClicked()
{
    ActionLogScreen actionLogScreen(this);
    actionLogScreen.exec();
}


// Слот, срабатывающий при нажатии в редакторе кнопки "распахнуть"
// Флаг показывает, что нужно сделать
// true - распахнуть область отводимую редактору
// false - сделать область, отводимую редактору, обычной
void MainWindow::onExpandEditArea(bool flag)
{
    if(flag)
    {
        globalParameters.getTreeScreen()->hide();
        globalParameters.getRecordTableScreen()->hide();
    }
    else
    {
        globalParameters.getTreeScreen()->show();
        globalParameters.getRecordTableScreen()->show();
    }
}


void MainWindow::onClickHelpAboutMyTetra(void)
{
    QString version=QString::number(APPLICATION_RELEASE_VERSION)+"."+QString::number(APPLICATION_RELEASE_SUBVERSION)+"."+QString::number(APPLICATION_RELEASE_MICROVERSION);

    QString infoProgramName;
    QString infoVersion;
    QString infoAuthor;
    QString infoEmail;
    QString infoLicense;

    infoProgramName="<b>MyTetra</b> - smart manager<br/>for information collecting<br/><br/>";
    infoVersion="v."+version+"<br/><br/>";
    infoAuthor="Author: Sergey M. Stepanov<br/>";
    infoEmail="Author Email:<i>xintrea@gmail.com</i><br/><br/>";
    infoLicense="GNU General Public License v.3.0<br/><br/>";

    QString info=infoProgramName+
            infoVersion+
            infoAuthor+
            infoEmail+
            infoLicense;

    QMessageBox(this).about(this, "MyTetra v."+version, info);
}


void MainWindow::onClickHelpAboutQt(void)
{
    QMessageBox(this).aboutQt(this);
}


void MainWindow::onClickHelpTechnicalInfo(void)
{
    QString infoTargetOs;
    QString infoProgramFile;
    QString infoWorkDirectory;
    QString infoDevicePixelRatio;
    QString infoPhysicalDpi;
    QString infoPhysicalDpiX;
    QString infoPhysicalDpiY;

    infoTargetOs="Target OS: "+globalParameters.getTargetOs()+"<br/>";
    infoProgramFile="Program file: "+globalParameters.getMainProgramFile()+"<br/>";
    infoWorkDirectory="Work directory: "+globalParameters.getWorkDirectory()+"<br/>";

    infoDevicePixelRatio="Device pixel ratio: "+(QString::number( qApp->devicePixelRatio(), 'f', 2 ))+"<br/>";
    infoPhysicalDpi="Physical DPI: "+(QString::number( QApplication::screens().at(0)->physicalDotsPerInch(), 'f', 2 ))+"<br/>";
    QString info=infoTargetOs+
            infoProgramFile+
            infoWorkDirectory+
            infoDevicePixelRatio+
            infoPhysicalDpi+
            infoPhysicalDpiX+
            infoPhysicalDpiY;

    QMessageBox msgBox(this);
    msgBox.setIcon( QMessageBox::Information );
    msgBox.setWindowTitle(tr("Technical info"));
    msgBox.setText(tr("<b>Technical info</b>"));
    msgBox.setInformativeText(info);
    msgBox.exec();
}


void MainWindow::onClickFocusTree(void)
{
    treeScreen->setFocusToBaseWidget();
}


void MainWindow::onClickFocusNoteTable(void)
{
    recordTableScreen->setFocusToBaseWidget();
}


void MainWindow::onClickFocusEditor(void)
{
    editorScreen->setFocusToBaseWidget();
}


// Перечитывание всей базы знаний
void MainWindow::reload(void)
{
  reloadSaveStage();

  // Стадия Load идет сразу после стадии Save, флаг о наличии задержки не выставляется
  reloadLoadStage(false);
}


// Перечитывание дерева знаний, стадия Save
void MainWindow::reloadSaveStage(void)
{
  // Сохраняются данные в поле редактирования
  saveTextarea();

  // Сохраняются данные о курсорах в дереве и таблице конечных записей
  saveTreePosition();
  saveRecordTablePosition();
  saveEditorCursorPosition();
  saveEditorScrollBarPosition();
}


// Перечитывание дерева знаний, стадия Load
void MainWindow::reloadLoadStage(bool isLongTimeReload)
{
    // Блокируется история
    walkHistory.setDrop(true);

    // Стадия Load может запуститься сильно позже по времени после стадии Save.
    // И пользователь можейт уйти с записи, которую он редактировал на стадии Save.
    // Поэтому для такого случая перед перечитыванием модели нужно обязательно
    // сохранить положение пользователя в дереве, чтобы восстановить именно
    // актуальное состояние после перечитывания модели
    if( isLongTimeReload )
    {
        saveTreePosition();
        saveRecordTablePosition();
        saveEditorCursorPosition();
        saveEditorScrollBarPosition();
    }

    // Если дерево успешно перечитано и обнаружено, что оно было изменено
    // (При этом модель будет полностью пересоздана, все индексы будут новые)
    if( treeScreen->reloadKnowTree() )
    {
        // Чтобы при дальнейшей работе небыло сегфолтов, так как индексы изменились,
        // нужно восстановить положение пользователя в дереве знаний
        restoreTreePosition();
        restoreRecordTablePosition();
        restoreEditorCursorPosition();
        restoreEditorScrollBarPosition();
    }

    // Разблокируется история посещений элементов
    walkHistory.setDrop(false);
}


// Старт синхронизации
void MainWindow::synchronization(bool visible)
{
    // Если кнопка синхронизации заблокирована, начинать синхронизацию нельзя
    if(!recordTableScreen->actionSynchro->isEnabled())
        return;

    // Блокируется кнопка синхронизации, чтобы два раза случайно не нажать синхронизацию (окно синхронизации не модально)
    recordTableScreen->actionSynchro->setEnabled(false);

    actionLogger.addAction("startSynchro");

    reloadSaveStage();

    // Считывается команда синхронизации
    QString command=mytetraConfig.get_synchrocommand();

    // Если команда синхронизации пуста
    if(command.trimmed().length()==0)
    {
        QMessageBox::warning(this,
                             tr("MyTetra: can't synchronization"),
                             tr("Do not set synchronization command.<br>Check the setting in \"Sync\" section in \"Tools\" menu"),
                             QMessageBox::Close);

        // Кнопка синхронизации разблокируется, чтобы ее можно было снова нажать
        recordTableScreen->actionSynchro->setEnabled(true);

        return;
    }

    auto cmdArgs = QProcess::splitCommand(command);
    // Макрос %a заменяется на путь к директории базы данных
    // QString databasePath=globalParameters.getWorkDirectory()+"/"+mytetraConfig.get_tetradir();
    QDir databaseDir( mytetraConfig.get_tetradir() );
    QString databasePath=databaseDir.canonicalPath();

    cmdArgs.replaceInStrings("%a", databasePath);

    // Запуск команды синхронизации
    synchroCommandRun->setWindowTitle(tr("MyTetra synchronization"));
    synchroCommandRun->setMessageText(tr("Synchronization in progress, please wait..."));
    synchroCommandRun->setCommand(cmdArgs[0]);
    cmdArgs.removeFirst();
    synchroCommandRun->setArgs(cmdArgs);
    synchroCommandRun->run(visible);
}


// Завершение синхронизации
void MainWindow::onSynchroCommandFinishWork()
{
    // Функция перечитывания дерева знаний вызывается с флагом,
    // что от предыдущей стадии была большая задержка
    reloadLoadStage(true);

    // После того, как дерево перечитано, содержимое открепляемых окон обновляется,
    // так как содержимое записей могло поменяться (т. к. получены новые изменения)
    EditorShowTextDispatcher::instance()->updateAllWindows();

    // Возможно, что после обновления имеются открепляемые окна, показывающие
    // записи, которые были удалены при синхронизации. Такие окна должны быть закрыты.
    // Проверка и закрытие таких окон делается в отдельном потоке, потому вызов
    // делается через сингнал-слот
    emit doUpdateDetachedWindows();

    actionLogger.addAction("stopSynchro");

    // В конце синхронизации нужно проверить, не происходит ли выход из программы
    if(enableRealClose==true)
    {
        this->applicationExit();
    }
}


void MainWindow::setupIconActions(void)
{
    actionTrayRestore = new QAction(tr("&Restore window"), this);
    connect(actionTrayRestore, &QAction::triggered, this, &MainWindow::showWindow);

    actionTrayMaximize = new QAction(tr("Ma&ximize window"), this);
    connect(actionTrayMaximize, &QAction::triggered, this, &MainWindow::showMaximized);

    actionTrayMinimize = new QAction(tr("Mi&nimize window"), this);
    connect(actionTrayMinimize, &QAction::triggered, this, &MainWindow::hide);

    actionTrayQuit = new QAction(tr("&Quit"), this);
    connect(actionTrayQuit, &QAction::triggered, this, &MainWindow::applicationExit);
}


void MainWindow::showWindow()
{
    activateWindow();
    showNormal();
    raise();

    // Восстанавливается поведение открепляемых окон
    EditorShowTextDispatcher::instance()->restoreBehavior();
}


void MainWindow::createTrayIcon(void)
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(actionTrayRestore);
    trayIconMenu->addAction(actionTrayMaximize);
    trayIconMenu->addAction(actionTrayMinimize);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(actionTrayQuit);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}


void MainWindow::setIcon(void)
{
    connect(trayIcon, &QSystemTrayIcon::activated,
            this,     &MainWindow::iconActivated);

    QIcon icon = QIcon(":/resource/pic/logo.svg");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    // tray_icon->setToolTip(iconComboBox->itemText(index));
}


void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(QSystemTrayIcon::isSystemTrayAvailable()==false)
        return;

    qDebug() << "Click on tray icon";

    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        // Если окно видно
        if(isVisible())
        {
            // Если окно неактивно, значит активно другое (и возможно оно перекрывает окно MyTetra)
            // Не работает в Windows, в Linux не проверял
            // Причина неработоспособности - при клике на иконку, окно MyTera всегда становится неактивным (т.к. активен систрей)
            // И условие срабатывает всегда. Доделать или отказаться
            // if(QGuiApplication::applicationState() == Qt::ApplicationInactive)
            // {
            //   activateWindow();
            //   return;
            // }

            if(isMinimized())
            {
                qDebug() << "If visible and minimized";
                showWindow();
                return;
            }
            else
            {
                qDebug() << "Hide";
                hide();
                return;
            }
        }
        else
        {
            qDebug() << "If not visible";
            showWindow();
            return;
            // if(isMinimized()) showNormal();
            // else show();
        }
    default:
        ;
    }
}


// Слот закрытия окна
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(enableRealClose==false)
    {
        if(QSystemTrayIcon::isSystemTrayAvailable()==false)
        {
            return;
        }

        // При приходе события закрыть окно, событие игнорируется
        // и окно просто делается невидимым. Это нужно чтобы при закрытии окна
        // программа не завершала работу
        if(trayIcon->isVisible())
        {
            hide();
            event->ignore();
            return;
        }
    }

    // Здесь код оказывается если далее должно однозначно происходить закрытие программы

    // Так как закрытие программы может происходить не с первого раза, например при
    // настройке синхронизации при закрытии программы, то действия
    // которые должны выполняться при закрытии должны срабатывать только единожды
    if(exitCounter==0)
    {
        // Запоминается список и состояния открепляемых окон,
        // данное действие нельзя делать в saveAllState(), вызываемое из деструктора, так как если
        // все открепляемые окна не будут закрыты (см. следующею команду), то деструктор
        // главного окна не будет вызываться объектом приложения
        EditorShowTextDispatcher::instance()->saveOpenWindows();

        // Закрытие открепляемых окон, даже если главное окно не является родителем для
        // открепляемых окон, чтобы не осталось "висячих" открепляемых окон, которые
        // не будут давать закрыться приложению
        EditorShowTextDispatcher::instance()->closeAllWindowsForExit();

        exitCounter++;
    }

    event->accept();
}


bool MainWindow::eventFilter( QObject *o, QEvent * e )
{
    // Q_UNUSED(o);
    // qDebug() << "Event: " << e->type();

    // Отлавливание потери фокуса
    // QEvent::ActivationChange
    if(o==this && e->type() == QEvent::WindowDeactivate)
    {
        qDebug() << "Main window focus deactivate, save all state.";
        saveAllState();
    }

    // Продолжать оработку событий дальше
    // return false;
    return QMainWindow::eventFilter(o, e);
}    


// Обработчик отлавливает нажатия клавиш
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Проброс нажатия клавиши Ctrl в редактор, даже если он не активный (не в фокусе)
    if( event->key() == Qt::Key_Control )
        if(editorScreen!=nullptr)
            emit globalPressKey(Qt::Key_Control);

    QMainWindow::keyPressEvent(event);
}


// Обработчик отлавливает отжатия клавиш
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // Если отжата клавиша Ctrl
    if( event->key() == Qt::Key_Control )
        if(editorScreen!=nullptr)
            emit globalReleaseKey(Qt::Key_Control);

    QMainWindow::keyReleaseEvent(event);
}


void MainWindow::goWalkHistoryPrevious(void)
{
    editorScreen->saveTextarea();

    // Идентификатор текущей записи запоминается в историю
    QString id=editorScreen->getMiscField("id");
    walkHistory.add(id,
                    editorScreen->getCursorPosition(),
                    editorScreen->getScrollBarPosition(),
                    WALK_HISTORY_GO_PREVIOUS);

    goWalkHistory();
}


void MainWindow::goWalkHistoryNext(void)
{
    editorScreen->saveTextarea();

    // Идентификатор текущей записи запоминается в историю
    QString id=editorScreen->getMiscField("id");
    walkHistory.add(id,
                    editorScreen->getCursorPosition(),
                    editorScreen->getScrollBarPosition(),
                    WALK_HISTORY_GO_NEXT);

    goWalkHistory();
}


void MainWindow::goWalkHistory(void)
{
    // Выясняется идентификатор записи, на которую надо переключиться
    QString id=walkHistory.getId();

    if(id.length()==0)
        return;

    // Выясняется путь к ветке, где находится данная запись
    QStringList path=treeScreen->knowTreeModel->getRecordPath(id);

    // Проверяем, есть ли такая ветка
    if(treeScreen->knowTreeModel->isItemValid(path)==false)
        return;


    // Выясняется позицию записи в таблице конечных записей
    TreeItem *item =treeScreen->knowTreeModel->getItem(path);

    // Проверяем, есть ли такая позиция
    if(item->recordtableGetTableData()->isRecordExists(id)==false)
        return;

    setTreePosition(path);
    setRecordtablePositionById(id);

    if(mytetraConfig.getRememberCursorAtHistoryNavigation())
    {
        editorScreen->setCursorPosition( walkHistory.getCursorPosition(id) );
        editorScreen->setScrollBarPosition( walkHistory.getScrollBarPosition(id) );
    }
}


// Метод, вызываемый из всех точек интерфейса, в которых происходит
// переход к другой записи. Метод вызывается до перехода, чтобы сохранить
// текст редактируемой записи
void MainWindow::saveTextarea(void)
{
    QString id=editorScreen->getMiscField("id");

    qDebug() << "MainWindow::saveTextarea() : id :" << id;

    editorScreen->saveTextarea();

    walkHistory.add(id,
                    editorScreen->getCursorPosition(),
                    editorScreen->getScrollBarPosition());
}


// Слот, обрабатывающий смену фокуса на виджетах
void MainWindow::onFocusChanged(QWidget *widgetFrom, QWidget *widgetTo)
{
    Q_UNUSED (widgetFrom)

    if(widgetTo==nullptr)
        return;

    qDebug() << "MainWindow::onFocusChanged() to " << widgetTo->objectName();

    return; // Временно ничего не делает
}

