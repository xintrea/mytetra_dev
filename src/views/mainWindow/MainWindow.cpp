#include <QString>
#include <QDir>
#include <QString>
#include <QDesktopWidget>

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
#include "views/consoleEmulator/ExecuteCommand.h"
#include "libraries/WalkHistory.h"
#include "libraries/ActionLogger.h"
#include "libraries/WindowSwitcher.h"
#include "views/actionLog/ActionLogScreen.h"

#include "libraries/wyedit/EditorTextArea.h"

extern AppConfig mytetraConfig;
extern TrashMonitoring trashMonitoring;
extern GlobalParameters globalParameters;
extern WalkHistory walkHistory;
extern ActionLogger actionLogger;


MainWindow::MainWindow() : QMainWindow()
{
  extern QObject *pMainWindow;
  pMainWindow=this;
  setObjectName("mainwindow");

  treeScreen=NULL;
  recordTableScreen=NULL;
  findScreenDisp=NULL;
  editorScreen=NULL;
  statusBar=NULL;
  windowSwitcher=NULL;

  installEventFilter(this);

  setupUI();
  setupSignals();
  assembly();

  initFileMenu();
  initToolsMenu();
  initHelpMenu();

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

  // Инициализация генератора случайных чисел
  initRandom();
}


MainWindow::~MainWindow()
{
  saveAllState();

  // В лог записывается событие остановки программы
  actionLogger.addAction("stopProgram");
  
  delete treeScreen;
  delete recordTableScreen;
  delete findScreenDisp;
  delete editorScreen;
  delete statusBar;
  delete windowSwitcher;
}


void MainWindow::setupUI(void)
{
  // При создании объектов не указывается parent, так как он буден задан в момент вставки в layout в методе assembly()

  treeScreen=new TreeScreen;
  treeScreen->setObjectName("treeScreen");
  globalParameters.setTreeScreen(treeScreen);

  recordTableScreen=new RecordTableScreen();
  recordTableScreen->setObjectName("recordTableScreen");
  globalParameters.setRecordTableScreen(recordTableScreen);

  findScreenDisp=new FindScreen();
  findScreenDisp->setObjectName("findScreenDisp");
  globalParameters.setFindScreen(findScreenDisp);

  editorScreen=new MetaEditor();
  editorScreen->setObjectName("editorScreen");
  globalParameters.setMetaEditor(editorScreen);

  statusBar=new QStatusBar();
  statusBar->setObjectName("statusBar");
  setStatusBar(statusBar);
  globalParameters.setStatusBar(statusBar);

  // Вспомогательный объект переключения окон, используется в мобильном интерфейсе
  windowSwitcher=new WindowSwitcher();
  windowSwitcher->setObjectName("windowSwitcher");
  globalParameters.setWindowSwitcher(windowSwitcher);

  // todo: Для проверки, почему то в этом месте поиск объекта по имени не работает, разобраться.
  // MetaEditor *edView=find_object<MetaEditor>("editorScreen");
}


void MainWindow::setupSignals(void)
{
  connect(editorScreen,SIGNAL(send_expand_edit_area(bool)), this, SLOT(onExpandEditArea(bool)));

  // Сигнал, генерирующийся при выходе из оконных систем X11 и Windows
  connect(qApp, SIGNAL(commitDataRequest(QSessionManager&)), this, SLOT(commitData(QSessionManager&)));

  connect(qApp, SIGNAL( focusChanged(QWidget *, QWidget *) ), this, SLOT( onFocusChanged(QWidget *, QWidget *) ));

  // Связывание сигналов кнопки поиска по базе с действием по открытию виджета поиска по базе
  connect(treeScreen->actionList["findInBase"], SIGNAL(triggered()), globalParameters.getWindowSwitcher(), SLOT(findInBaseClick()));
  connect(recordTableScreen->actionFindInBase, SIGNAL(triggered()), globalParameters.getWindowSwitcher(), SLOT(findInBaseClick()));
  connect(editorScreen, SIGNAL(wyeditFindInBaseClicked()), globalParameters.getWindowSwitcher(), SLOT(findInBaseClick()));
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
  saveGeometry();
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
  Q_UNUSED(manager);
  qDebug() << "Session manager send commit data signal.";

  applicationFastExit();
}


// Слот, обрабатывающий внешние сообщения
// Используется для обработки ссобщений, приходящих от других экземпляров MyTetra
void MainWindow::messageHandler(QString message)
{
  qDebug() << "MainWindow recieved message: "+message;

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
void MainWindow::restoreGeometry(void)
{
  if(globalParameters.getTargetOs()=="android")
    setWindowState(Qt::WindowMaximized); // Для Андроида окно просто разворачивается на весь экран
  else
  {
    QRect rect=mytetraConfig.get_mainwingeometry();
    resize(rect.size());
    move(rect.topLeft());
  }

  // move(rect.topLeft());
  // resize(rect.size());

  vSplitter->setSizes(mytetraConfig.get_vspl_size_list());
  hSplitter->setSizes(mytetraConfig.get_hspl_size_list());
  findSplitter->setSizes(mytetraConfig.get_findsplitter_size_list());
}


// Запоминается геометрия окна и позиции основных разделителей
void MainWindow::saveGeometry(void)
{
  qDebug() << "Save window geometry and splitter sizes";

  QRect geom(pos(), size());

  mytetraConfig.set_mainwingeometry(geom.x(), geom.y(),
                                    geom.width(), geom.height());

  // mytetraconfig.set_mainwingeometry(geometry().x(), geometry().y(),
  //                                   geometry().width(), geometry().height());

  mytetraConfig.set_vspl_size_list(vSplitter->sizes());
  mytetraConfig.set_hspl_size_list(hSplitter->sizes());


  // Запоминается размер сплиттера только при видимом виджете поиска,
  // т.к. если виджета поиска невидно, будет запомнен нуливой размер

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


// Создание раздела меню File
void MainWindow::initFileMenu(void)
{
  // Создание меню
  QMenu *menu = new QMenu(tr("&File"), this);
  menuBar()->addMenu(menu);

  QAction *a;

  a = new QAction(tr("&Print..."), this);
  a->setShortcut(QKeySequence::Print);
  connect(a, SIGNAL(triggered()), this, SLOT(filePrint()));
  menu->addAction(a);

  a = new QAction(tr("Print Preview..."), this);
  connect(a, SIGNAL(triggered()), this, SLOT(filePrintPreview()));
  menu->addAction(a);

  a = new QAction(tr("&Export PDF..."), this);
  a->setShortcut(Qt::CTRL + Qt::Key_D);
  connect(a, SIGNAL(triggered()), this, SLOT(filePrintPdf()));
  menu->addAction(a);

  menu->addSeparator();

  a = new QAction(tr("Export tree item"), this);
  connect(a, SIGNAL(triggered()), this, SLOT(fileExportBranch()));
  menu->addAction(a);

  a = new QAction(tr("Import tree item"), this);
  connect(a, SIGNAL(triggered()), this, SLOT(fileImportBranch()));
  menu->addAction(a);

  menu->addSeparator();

  a = new QAction(tr("&Quit"), this);
  a->setShortcut(Qt::CTRL + Qt::Key_Q);
  connect(a, SIGNAL(triggered()), this, SLOT(applicationExit()));
  menu->addAction(a);
}


// Создание раздела меню Tools
void MainWindow::initToolsMenu(void)
{
  // Создание меню
  QMenu *menu = new QMenu(tr("&Tools"), this);
  menuBar()->addMenu(menu);

  QAction *a;

  a = new QAction(tr("Find in ba&se"), this); // Так как есть this, указатель не будет потерян основным окном
  connect(a, SIGNAL(triggered()), this, SLOT(toolsFind()));
  menu->addAction(a);

  a = new QAction(tr("Action &log"), this);
  connect(a, SIGNAL(triggered()), this, SLOT(onActionLogClicked()));
  menu->addAction(a);


  menu->addSeparator();


  if(mytetraConfig.getInterfaceMode()=="desktop")
  {
    a = new QAction(tr("&Preferences"), this);
    connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
    menu->addAction(a);
  }
  else
  {
    // Создание подменю
    QMenu *menu = new QMenu(tr("&Preferences"), this);
    initPreferencesMenu(menu);
  }
}


// Заполнение подраздела меню Preferences
void MainWindow::initPreferencesMenu(QMenu *menu)
{
  QAction *a;

  a = new QAction(tr("Main"), this);
  // connect(a, SIGNAL(triggered()), this, SLOT(toolsFind()));
  menu->addAction(a);

  a = new QAction(tr("Crypt"), this);
  // connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
  menu->addAction(a);

  a = new QAction(tr("Synchro"), this);
  // connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
  menu->addAction(a);

  a = new QAction(tr("RecordTable"), this);
  // connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
  menu->addAction(a);

  a = new QAction(tr("Misc"), this);
  // connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
  menu->addAction(a);
}


// Создание раздела меню Help
void MainWindow::initHelpMenu(void)
{
  // Создание меню
  QMenu *menu = new QMenu(tr("&Help"), this);
  menuBar()->addMenu(menu);

  QAction *a;

  a = new QAction(tr("About MyTetra"), this);
  connect(a, SIGNAL(triggered()), this, SLOT(onClickHelpAboutMyTetra()));
  menu->addAction(a);

  a = new QAction(tr("About Qt"), this);
  connect(a, SIGNAL(triggered()), this, SLOT(onClickHelpAboutQt()));
  menu->addAction(a);

  a = new QAction(tr("Technical info"), this);
  connect(a, SIGNAL(triggered()), this, SLOT(onClickHelpTechnicalInfo()));
  menu->addAction(a);
}


// Новая коллекция
void MainWindow::fileNew(void)
{

}

// Открыть коллекцию
void MainWindow::fileOpen(void)
{

}

// Сохранить текущую статью
bool MainWindow::fileSave(void)
{
  return true;
}

// Сохранить текущую статью как файл
bool MainWindow::fileSaveAs(void)
{
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

  // Если в конфиге настроено, что нужно синхронизироваться при выходе
  // И задана команда синхронизации
  if(mytetraConfig.get_synchroonexit())
    if(mytetraConfig.get_synchrocommand().trimmed().length()>0)
      synchronization();

  // Запуск выхода из программы
  enableRealClose=true;
  emit close();
}


// Быстрый выход из программы, без возможности синхронизации
void MainWindow::applicationFastExit(void)
{
  saveAllState();

  // Запуск выхода из программы
  enableRealClose=true;
  emit close();
}


void MainWindow::toolsFind(void)
{
  // Определяется ссылка на виджет поиска
  FindScreen *findScreenRel=find_object<FindScreen>("findScreenDisp");

  if( !(findScreenRel->isVisible()) )
    findScreenRel->show();
  else
    findScreenRel->hide();
}


void MainWindow::toolsPreferences(void)
{
  // Создается окно настроек, после выхода из этой функции окно удалится
  AppConfigDialog dialog("");
  dialog.show();
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

  QMessageBox *msgBox = new QMessageBox(this);
  msgBox->about(this,
                "MyTetra v."+version,
                info);
}


void MainWindow::onClickHelpAboutQt(void)
{
  QMessageBox *msgBox = new QMessageBox(this);
  msgBox->aboutQt(this);
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

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
  infoDevicePixelRatio="Device pixel ratio: "+(QString::number( qApp->devicePixelRatio(), 'f', 8 ))+"<br/>";
  infoPhysicalDpi="Physical DPI (from screen): "+(QString::number( QApplication::screens().at(0)->physicalDotsPerInch(), 'f', 8 ))+"<br/>";
#endif

  infoPhysicalDpiX="Physical DPI X (from desktop): "+(QString::number( qApp->desktop()->physicalDpiX(), 'f', 8 ))+"<br/>";
  infoPhysicalDpiY="Physical DPI Y (from desktop): "+(QString::number( qApp->desktop()->physicalDpiY(), 'f', 8 ))+"<br/>";

  QString info=infoTargetOs+
               infoProgramFile+
               infoWorkDirectory+
               infoDevicePixelRatio+
               infoPhysicalDpi+
               infoPhysicalDpiX+
               infoPhysicalDpiY;

  QMessageBox *msgBox = new QMessageBox(this);
  msgBox->setIcon( QMessageBox::Information );
  msgBox->setWindowTitle(tr("Technical info"));
  msgBox->setText(tr("<b>Technical info</b>"));
  msgBox->setInformativeText(info);
  msgBox->exec();
}


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


void MainWindow::reloadLoadStage(void)
{
  // Блокируется история
  walkHistory.setDrop(true);

  // Заново считываются данные в дерево
  if( treeScreen->reloadKnowTree() )
  {
    restoreTreePosition();
    restoreRecordTablePosition();
    restoreEditorCursorPosition();
    restoreEditorScrollBarPosition();
  }

  // Разблокируется история посещений элементов
  walkHistory.setDrop(false);
}


// Перечитывание всей базы знаний
void MainWindow::reload(void)
{
  reloadSaveStage();
  reloadLoadStage();
}


void MainWindow::synchronization(bool visible)
{
  // Если кнопка синхронизации заблокирована, начинать синхронизацию нельзя
  if(!recordTableScreen->actionSyncro->isEnabled())
    return;

  // Блокируется кнопка синхронизации, чтобы два раза случайно не нажать синхронизацию (окно синхронизации не модально)
  recordTableScreen->actionSyncro->setEnabled(false);

  actionLogger.addAction("startSyncro");

  reloadSaveStage();

  // Считывается команда синхронизации
  QString command=mytetraConfig.get_synchrocommand();

  // Если команда синхронизации пуста
  if(command.trimmed().length()==0)
  {
    QMessageBox::warning(this,
                         tr("MyTetra: can't synchronization"),
                         tr("Do not set synchronization command.<br>Check the setting in \"Syncro\" section in \"Tools\" menu"),
                         QMessageBox::Close);
    return;
  }


  // Макрос %a заменяется на путь к директории базы данных
  // QString databasePath=globalParameters.getWorkDirectory()+"/"+mytetraConfig.get_tetradir();
  QDir databaseDir( mytetraConfig.get_tetradir() );
  QString databasePath=databaseDir.canonicalPath();

  command.replace("%a", databasePath);

  // Запуск команды синхронизации
  ExecuteCommand exCommand;

  // Связывание сигнала окончания выполнения команды синхронизации со слотом, срабатывающем при завершении выполнения команды
  connect(&exCommand, SIGNAL(finishWork()), recordTableScreen, SLOT(onExecuteCommandFinishWork()));

  exCommand.setWindowTitle(tr("MyTetra synchronization"));
  exCommand.setMessageText(tr("Synchronization in progress, please wait..."));

  exCommand.setCommand(command);
  exCommand.run(visible);

  reloadLoadStage();

  actionLogger.addAction("stopSyncro");
}


void MainWindow::setupIconActions(void)
{
  actionTrayRestore = new QAction(tr("&Restore window"), this);
  connect(actionTrayRestore, SIGNAL(triggered()), this, SLOT(showWindow()));

  actionTrayMaximize = new QAction(tr("Ma&ximize window"), this);
  connect(actionTrayMaximize, SIGNAL(triggered()), this, SLOT(showMaximized()));

  actionTrayMinimize = new QAction(tr("Mi&nimize window"), this);
  connect(actionTrayMinimize, SIGNAL(triggered()), this, SLOT(hide()));

  actionTrayQuit = new QAction(tr("&Quit"), this);
  connect(actionTrayQuit, SIGNAL(triggered()), this, SLOT(applicationExit()));
}


void MainWindow::showWindow()
{
  activateWindow();
  showNormal();
  raise();
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
  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

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
    if(QSystemTrayIcon::isSystemTrayAvailable()==false) return;

    // При приходе события закрыть окно, событие игнорируется
    // и окно просто делается невидимым. Это нужно чтобы при закрытии окна
    // программа не завершала работу
    if(trayIcon->isVisible())
    {
      hide();
      event->ignore();
    }
  }
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
    if(editorScreen!=NULL)
      emit globalPressKey(Qt::Key_Control);
}


// Обработчик отлавливает отжатия клавиш
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
  // Если отжата клавиша Ctrl
  if( event->key() == Qt::Key_Control )
    if(editorScreen!=NULL)
      emit globalReleaseKey(Qt::Key_Control);
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
  Q_UNUSED (widgetFrom);

  if(widgetTo==NULL)
    return;

  qDebug() << "MainWindow::onFocusChanged() to " << widgetTo->objectName();

  return; // Временно ничего не делает
}


