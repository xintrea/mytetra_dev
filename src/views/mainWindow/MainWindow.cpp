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
#include "views/consoleEmulator/ExecuteCommand.h"
#include "libraries/WalkHistory.h"

#include "libraries/wyedit/EditorTextArea.h"

extern AppConfig mytetraconfig;
extern TrashMonitoring trashmonitoring;
extern GlobalParameters globalParameters;
extern WalkHistory walkHistory;

MainWindow::MainWindow() : QMainWindow()
{
 extern QObject *pMainWindow;
 pMainWindow=this;
 setObjectName("mainwindow");

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
 trashmonitoring.init(mytetraconfig.get_trashdir());
 trashmonitoring.update();

 // Закрывать ли по-настоящему окно при обнаружении сигнала closeEvent
 enableRealClose=false;

 // Инициализация генератора случайных чисел
 init_random();
}


MainWindow::~MainWindow()
{
 saveAllState();
  
 delete treeView;
 delete recordTableView;
 delete editorView;
}


void MainWindow::setupUI(void)
{
 treeView=new TreeScreen;
 treeView->setObjectName("TreeScreen");
 globalParameters.setTreeScreen(treeView);

 recordTableView=new RecordTableScreen();
 recordTableView->setObjectName("recordtableview");
 globalParameters.setRecordTableScreen(recordTableView);

 findScreenDisp=new FindScreen();
 findScreenDisp->setObjectName("findscreendisp");
 globalParameters.setFindScreen(findScreenDisp);

 editorView=new MetaEditor();
 editorView->setObjectName("editorview");
 globalParameters.setMetaEditor(editorView);

 statusBar=new QStatusBar();
 statusBar->setObjectName("statbar");
 setStatusBar(statusBar);
 globalParameters.setStatusBar(statusBar);
}


void MainWindow::setupSignals(void)
{
 connect(editorView,SIGNAL(send_expand_edit_area(bool)), this, SLOT(onExpandEditArea(bool)));

 // Сигнал, генерирующийся при выходе из оконных систем X11 и Windows
 connect(qApp, SIGNAL(commitDataRequest(QSessionManager&)), this, SLOT(commitData(QSessionManager&)));
}


void MainWindow::assembly(void)
{
 vSplitter=new QSplitter(Qt::Vertical);
 vSplitter->addWidget(recordTableView); // Список конечных записей
 vSplitter->addWidget(editorView);      // Текст записи
 vSplitter->setCollapsible(0,false); // Список конечных записей не может смыкаться
 vSplitter->setCollapsible(1,false); // Содержимое записи не может смыкаться

 hSplitter=new QSplitter(Qt::Horizontal);
 hSplitter->addWidget(treeView); // Дерево веток
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
 mytetraconfig.sync();
}


// Слот, срабатывающий когда происходит выход из оконной системы
void MainWindow::commitData(QSessionManager& manager)
{
 Q_UNUSED(manager);
 qDebug() << "Session manager send commit data signal.";

 applicationFastExit();
}


// Восстанавливается геометрия окна и позиции основных разделителей
void MainWindow::restoreGeometry(void)
{
 QRect rect=mytetraconfig.get_mainwingeometry();

 resize(rect.size());
 move(rect.topLeft());

 // move(rect.topLeft());
 // resize(rect.size());
 
 vSplitter->setSizes(mytetraconfig.get_vspl_size_list());
 hSplitter->setSizes(mytetraconfig.get_hspl_size_list());
 findSplitter->setSizes(mytetraconfig.get_findsplitter_size_list());
}


// Запоминается геометрия окна и позиции основных разделителей
void MainWindow::saveGeometry(void)
{
 qDebug() << "Save window geometry and splitter sizes";

 QRect geom(pos(), size());

 mytetraconfig.set_mainwingeometry(geom.x(), geom.y(),
                                   geom.width(), geom.height());

 // mytetraconfig.set_mainwingeometry(geometry().x(), geometry().y(),
 //                                   geometry().width(), geometry().height());
 
 mytetraconfig.set_vspl_size_list(vSplitter->sizes());
 mytetraconfig.set_hspl_size_list(hSplitter->sizes());
 

 // Запоминается размер сплиттера только при видимом виджете поиска,
 // т.к. если виджета поиска невидно, будет запомнен нуливой размер
 
 // if(findScreenDisp->isVisible()) - так делать нельзя, т.к.
 // данный метод вызывается из декструктора главного окна, и к этому моменту
 // виджет уже невиден

 if(mytetraconfig.get_findscreen_show())
  mytetraconfig.set_findsplitter_size_list(findSplitter->sizes());
}


void MainWindow::restoreTreePosition(void)
{
 // Путь к последнему выбранному в дереве элементу
 QStringList path=mytetraconfig.get_tree_position();

 setTreePosition(path);
}


void MainWindow::saveTreePosition(void)
{
 // Получение QModelIndex выделенного в дереве элемента
 QModelIndex index=treeView->getCurrentItemIndex();
 
 // Получаем указатель вида TreeItem
 TreeItem *item =treeView->knowTreeModel->getItem(index);

 // Сохраняем путь к элементу item
 mytetraconfig.set_tree_position(item->getPath());
}
 

void MainWindow::setTreePosition(QStringList path)
{
 if(treeView->knowTreeModel->isItemValid(path)==false) return;

 // Получаем указатель на элемент вида TreeItem, используя путь
 TreeItem *item =treeView->knowTreeModel->getItem(path);
 
 qDebug() << "Set tree position to " << item->getField("name") << " id " << item->getField("id");
 
 // Из указателя на элемент TreeItem получаем QModelIndex
 QModelIndex setto=treeView->knowTreeModel->getIndexByItem(item);

 // Курсор устанавливается в нужную позицию
 treeView->setCursorToIndex(setto);
}


bool MainWindow::isTreePositionCrypt()
{
 QStringList path=mytetraconfig.get_tree_position();

 if(treeView->knowTreeModel->isItemValid(path)==false) return false;

 // Получаем указатель на элемент вида TreeItem, используя путь
 TreeItem *item =treeView->knowTreeModel->getItem(path);

 if(item->getField("crypt")=="1")
  return true;
 else
  return false;
}


void MainWindow::restoreRecordTablePosition(void)
{
 int n=mytetraconfig.get_recordtable_position();

 if(n>=0)
  setRecordtablePosition(n);
}


void MainWindow::saveRecordTablePosition(void)
{
 int n=recordTableView->getFirstSelectionPos();
 
 mytetraconfig.set_recordtable_position(n);
}


void MainWindow::setRecordtablePosition(int n)
{
 recordTableView->setSelectionToPos(n);
}


void MainWindow::saveEditorCursorPosition(void)
{
 int n=editorView->getCursorPosition();

 mytetraconfig.setEditorCursorPosition(n);
}


void MainWindow::restoreEditorCursorPosition(void)
{
 int n=mytetraconfig.getEditorCursorPosition();

 editorView->setCursorPosition(n);
}


void MainWindow::saveEditorScrollBarPosition(void)
{
 int n=editorView->getScrollBarPosition();

 mytetraconfig.setEditorScrollBarPosition(n);
}


void MainWindow::restoreEditorScrollBarPosition(void)
{
 int n=mytetraconfig.getEditorScrollBarPosition();

 editorView->setScrollBarPosition(n);
}


void MainWindow::restoreFindOnBaseVisible(void)
{
 bool n=mytetraconfig.get_findscreen_show();

 // Определяется ссылка на виджет поиска
 FindScreen *findScreenRel=find_object<FindScreen>("findscreendisp");
 
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

   // Создание тулбара
   /*
   QToolBar *tb = new QToolBar(this);
   tb->setWindowTitle(tr("File Actions"));
   addToolBar(tb);
   */

   QAction *a;

   /*
   a = new QAction(tr("&New"), this);
   a->setShortcut(QKeySequence::New);
   connect(a, SIGNAL(triggered()), this, SLOT(fileNew()));
   // tb->addAction(a);
   menu->addAction(a);

   a = new QAction(tr("&Open..."), this);
   a->setShortcut(QKeySequence::Open);
   connect(a, SIGNAL(triggered()), this, SLOT(fileOpen()));
   // tb->addAction(a);
   menu->addAction(a);

   menu->addSeparator();

   a = new QAction(tr("&Save"), this);
   a->setShortcut(QKeySequence::Save);
   connect(a, SIGNAL(triggered()), this, SLOT(fileSave()));
   a->setEnabled(false);
   // tb->addAction(a);
   menu->addAction(a);

   a = new QAction(tr("Save &As..."), this);
   connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
   menu->addAction(a);
   menu->addSeparator();
   */

   a = new QAction(tr("&Print..."), this);
   a->setShortcut(QKeySequence::Print);
   connect(a, SIGNAL(triggered()), this, SLOT(filePrint()));
   // tb->addAction(a);
   menu->addAction(a);

   a = new QAction(tr("Print Preview..."), this);
   connect(a, SIGNAL(triggered()), this, SLOT(filePrintPreview()));
   menu->addAction(a);

   a = new QAction(tr("&Export PDF..."), this);
   a->setShortcut(Qt::CTRL + Qt::Key_D);
   connect(a, SIGNAL(triggered()), this, SLOT(filePrintPdf()));
   // tb->addAction(a);
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

   a = new QAction(tr("Find in ba&se"), this);
   connect(a, SIGNAL(triggered()), this, SLOT(toolsFind()));
   menu->addAction(a);

   menu->addSeparator();

   a = new QAction(tr("&Preferences"), this);
   connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
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
   editorView->get_textarea_document()->print(&printer);

  delete dlg;
#endif
}


// Предпросмотр печати текущей статьи
void MainWindow::filePrintPreview(void)
{
   PrintPreview *preview = new PrintPreview(editorView->get_textarea_document(), this);

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
    editorView->get_textarea_document()->print(&printer);
   }
#endif
}


// Слот - Нормальный выход из программы
void MainWindow::applicationExit(void)
{
 saveAllState();

 // Если в конфиге настроено, что нужно синхронизироваться при выходе
 // И задана команда синхронизации
   if(mytetraconfig.get_synchroonexit())
     if(mytetraconfig.get_synchrocommand().trimmed().length()>0)
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
 FindScreen *findScreenRel=find_object<FindScreen>("findscreendisp");
 
 if( !(findScreenRel->isVisible()) )
  findScreenRel->show();
 else
  findScreenRel->hide();
}


void MainWindow::toolsPreferences(void)
{
 // Создается окно настроек, после выхода из этой функции окно удалится
 AppConfigDialog dialog;
 dialog.show();
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
 QMessageBox *msgBox = new QMessageBox(this);
 msgBox->about(this, 
               "MyTetra v."+QString::number(APPLICATION_RELEASE_VERSION)+"."+QString::number(APPLICATION_RELEASE_SUBVERSION)+"."+QString::number(APPLICATION_RELEASE_MICROVERSION),
"<b>MyTetra</b> - smart manager<br/>\
for information collecting<br/>\
<br/>\
Author:<br/>\
Sergey M. Stepanov, <i>xintrea@gmail.com</i><br/>\
<br/>\
GNU General Public License v.3.0<br/>\
<a href='http://www.gnu.org/copyleft/gpl.html'>http://www.gnu.org/copyleft/gpl.html</a>");
}


void MainWindow::onClickHelpAboutQt(void)
{
 QMessageBox *msgBox = new QMessageBox(this);
 msgBox->aboutQt(this);
}


void MainWindow::synchronization(void)
{
 // Сохраняются данные в поле редактирования
 saveTextarea();

 // Сохраняются данные о курсорах в дереве и таблице конечных записей
 saveTreePosition();
 saveRecordTablePosition();
 saveEditorCursorPosition();
 saveEditorScrollBarPosition();

 // Считывается команда синхронизации
 QString command=mytetraconfig.get_synchrocommand();

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
 QString databasePath=globalParameters.getWorkDirectory()+"/"+mytetraconfig.get_tetradir();
 QDir databaseDir(databasePath);
 databasePath=databaseDir.canonicalPath();

 command.replace("%a", databasePath);

 // Запуск команды синхронизации
 ExecuteCommand cons;
 cons.setWindowTitle(tr("MyTetra synchronization"));
 cons.setMessageText(tr("Synchronization in progress, please wait..."));

 cons.setCommand(command);
 cons.run();

 // Блокируется история
 walkHistory.setDrop(true);

 // Заново считываются данные в дерево
 treeView->reloadKnowTree();
 restoreTreePosition();
 restoreRecordTablePosition();
 restoreEditorCursorPosition();
 restoreEditorScrollBarPosition();

 // Разблокируется история посещений элементов
 walkHistory.setDrop(false);
}


void MainWindow::setupIconActions(void)
{
 actionTrayRestore = new QAction(tr("&Restore window"), this);
 connect(actionTrayRestore, SIGNAL(triggered()), this, SLOT(showNormal()));

 actionTrayMaximize = new QAction(tr("Ma&ximize window"), this);
 connect(actionTrayMaximize, SIGNAL(triggered()), this, SLOT(showMaximized()));

 actionTrayMinimize = new QAction(tr("Mi&nimize window"), this);
 connect(actionTrayMinimize, SIGNAL(triggered()), this, SLOT(hide()));

 actionTrayQuit = new QAction(tr("&Quit"), this);
 connect(actionTrayQuit, SIGNAL(triggered()), this, SLOT(applicationExit()));
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
 if(QSystemTrayIcon::isSystemTrayAvailable()==false) return;

 switch (reason)
     {
      case QSystemTrayIcon::Trigger:
      case QSystemTrayIcon::DoubleClick:
           if(isVisible())
            {
             if(isMinimized()) showNormal();
             else hide();
            } 
           else 
            {
             if(isMinimized()) showNormal();
             else show();
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


bool MainWindow::eventFilter( QObject * o, QEvent * e )
{
  Q_UNUSED(o);  

  // qDebug() << "Event: " << e->type();
  
  // Отлавливание потери фокуса
  // QEvent::ActivationChange
  if( e->type() == QEvent::WindowDeactivate)
  {
   qDebug() << "Main window focus decativate, save text area.";
   saveTextarea();
  }

  return false; // Продолжать оработку событий дальше
}    


void MainWindow::goWalkHistoryPrevious(void)
{
 editorView->save_textarea();

 QString id=editorView->getMiscField("id");
 walkHistory.add(id,
                 editorView->getCursorPosition(),
                 editorView->getScrollBarPosition(),
                 WALK_HISTORY_GO_PREVIOUS);
 walkHistory.setDrop(true);

 goWalkHistory();
}


void MainWindow::goWalkHistoryNext(void)
{
 editorView->save_textarea();

 QString id=editorView->getMiscField("id");
 walkHistory.add(id,
                 editorView->getCursorPosition(),
                 editorView->getScrollBarPosition(),
                 WALK_HISTORY_GO_NEXT);
 walkHistory.setDrop(true);

 goWalkHistory();
}


void MainWindow::goWalkHistory(void)
{
 // Выясняется идентификатор записи, на которую надо переключиться
 QString id=walkHistory.getId();

 if(id.length()==0)
  {
   walkHistory.setDrop(false);
   return;
  }

 // Выясняется путь к ветке, где находится данная запись
 QStringList path=treeView->knowTreeModel->getRecordPath(id);

 // Проверяем, есть ли такая ветка
 if(treeView->knowTreeModel->isItemValid(path)==false)
  {
   walkHistory.setDrop(false);
   return;
  }


 // Выясняется позицию записи в талице конечных записей
 TreeItem *item =treeView->knowTreeModel->getItem(path);
 int pos=item->getRecordPos(id);

 // Проверяем, есть ли такая позиция
 if(pos<0)
  {
   walkHistory.setDrop(false);
   return;
  }

 setTreePosition(path);
 setRecordtablePosition(pos);

 if(mytetraconfig.getRememberCursorAtHistoryNavigation())
  {
   editorView->setCursorPosition( walkHistory.getCursorPosition(id) );
   editorView->setScrollBarPosition( walkHistory.getScrollBarPosition(id) );
  }

 walkHistory.setDrop(false);
}


// Метод, вызываемый из всех точек интерфейса, в которых происходит
// переход к другой записи. Метод вызывается до перехода, чтобы сохранить
// текст редактируемой записи
void MainWindow::saveTextarea(void)
{
 QString id=editorView->getMiscField("id");

 qDebug() << "MainWindow::saveTextarea() : id :" << id;

 editorView->save_textarea();

 walkHistory.add(id, 
                 editorView->getCursorPosition(),
                 editorView->getScrollBarPosition());
}
