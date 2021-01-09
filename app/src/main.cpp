
// Баг в Qt, Q_OS_ANDROID not defined for MOC. Его исправили, но он снова есть
// Поэтому приходится работать через TARGET_OS
#if TARGET_OS!=ANDROID_OS
#include <sys/timeb.h>
#endif


#include <QTranslator>
#include <QToolButton>
#include <QSplashScreen>
#include <QTextOption>

#include <QScroller>
#include <QScrollerProperties>
#include <QScrollBar>

#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/ClipboardRecords.h"
#include "libraries/TrashMonitoring.h"
#include "libraries/FixedParameters.h"
#include "libraries/GlobalParameters.h"
#include "libraries/ActionLogger.h"

#include "libraries/qtSingleApplication5/qtsingleapplication.h"

#include "models/dataBaseConfig/DataBaseConfig.h"
#include "libraries/WalkHistory.h"
#include "libraries/WindowSwitcher.h"
#include "libraries/crypt/RC5Simple.h"
#include "libraries/crypt/Password.h"
#include "libraries/TraceLogger.h"
#include "libraries/ShortcutManager.h"
#include "libraries/PeriodicCheckBase.h"
#include "libraries/PeriodicSyncro.h"
#include "libraries/IconSelectDialog.h"
#include "libraries/helpers/DebugHelper.h"
#include "libraries/helpers/MessageHelper.h"
#include "libraries/helpers/CssHelper.h"


using namespace std;

// todo: Разгрести объекты глобальной области, переделать все
// на синглтоны, сделать объект ядра, поместить объекты глабальной
// области в ядро как в единую центральную точку доступа


// Фиксированные параметры программы (жестко заданные в текущей версии MyTetra)
FixedParameters fixedParameters;

// Глобальные параметры программы (вычислимые на этапе инициализации, иногда меняющиеся в процессе выполнения программы)
GlobalParameters globalParameters;

// Конфигурация программы (считанная из файла конфигурации)
AppConfig mytetraConfig;

// Конфигурация хранилища данных
DataBaseConfig dataBaseConfig;

// Объект слежения за состоянием корзины
TrashMonitoring trashMonitoring;

// Объект с историей посещаемых записей
WalkHistory walkHistory;

// Логгер действий с данными
ActionLogger actionLogger;

// Менеджер горячих клавиш
ShortcutManager shortcutManager;

// Различные периодические проверки
PeriodicCheckBase periodicCheckBase;
PeriodicSyncro periodicSyncro;

// Указатель на основное окно программы
QObject *pMainWindow;


void printHelp()
{
  printf("\n");
  printf("MyTetra v.%d.%d.%d\n", APPLICATION_RELEASE_VERSION, APPLICATION_RELEASE_SUBVERSION, APPLICATION_RELEASE_MICROVERSION);
  printf("For use control mode, run by standard way MyTetra for show GUI interface, and next use command:\n");
  printf("./mytetra --control --show - Show and activate MyTetra window\n");
  printf("./mytetra --control --hide - Hide MyTetra window\n");
  printf("./mytetra --control --quit - Quit from MyTetra\n");
  printf("./mytetra --control --reload - Reload database\n");
  printf("./mytetra --control --openNote <noteId> - Jump to note with <noteId>\n");
  printf("./mytetra --control --addNoteDialog - Show dialod for create new note in current tree item\n");
  printf("./mytetra --control --openTreeItem <treeItemId> - Jump to tree item with <treeItemId>\n");
  printf("\n");
}


void parseConsoleOption(QtSingleApplication &app)
{
  // Если запрашивается помощь по опциям
  if( app.arguments().contains("-h") || app.arguments().contains("--help"))
  {
    printHelp();
    exit(0);
  }

  // Если MyTetra запущена с какой-то опцией, но нет опции --control
  if( app.arguments().count()>1 && !app.arguments().contains("--control"))
  {
    QString message="Bad options. May be you lost \"--control\"?\n";
    printf("%s", message.toLocal8Bit().data());
    exit(1);
  }

  // Если MyTetra запущена в режиме управления, а другого экземпляра (которым надо управлять) нет
  if(app.arguments().contains("--control") && !app.isRunning())
  {
    QString message="MyTetra exemplar for control is not running.\nPlease, run MyTetra before running your command.\n";
    printf("%s", message.toLocal8Bit().data());
    exit(2);
  }

  // Если MyTetra запущена в обычном режиме, но уже запущен другой экземпляр
  if( !app.arguments().contains("--control") && app.isRunning())
  {
    QString message="Another MyTetra exemplar is running.\n";

    printf("%s", message.toLocal8Bit().data());

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning); // Сообщение со значком предупреждения
    msgBox.setText(message);
    msgBox.exec();

    exit(3);
  }

  // Если MyTetra запущена в режиме управления, и есть другой экземпляр, которым нужно управлять
  if(app.arguments().contains("--control") && app.isRunning())
  {
    if(app.arguments().contains("--show"))
    {
      app.sendMessage("show");
      exit(0);
    }
    else if (app.arguments().contains("--hide"))
    {
      app.sendMessage("hide");
      exit(0);
    }
    else if (app.arguments().contains("--quit"))
    {
      app.sendMessage("quit");
      exit(0);
    }
    else if (app.arguments().contains("--reload"))
    {
      app.sendMessage("reload");
      exit(0);
    }
    else if (app.arguments().contains("--openNote"))
    {
      int openNoteIndex=app.arguments().indexOf("--openNote");
      app.sendMessage("openNote "+app.arguments().at(openNoteIndex+1));
      exit(0);
    }
    else if (app.arguments().contains("--addNoteDialog"))
    {
      app.sendMessage("addNoteDialog");
      exit(0);
    }
    else if (app.arguments().contains("--openBranch")) // Устаревшая опция
    {
      int openBranchIndex=app.arguments().indexOf("--openBranch");
      app.sendMessage("openTreeItem "+app.arguments().at(openBranchIndex+1)); // Аналог сигнала openTreeItem
      exit(0);
    }
    else if (app.arguments().contains("--openTreeItem"))
    {
      int openTreeItemIndex=app.arguments().indexOf("--openTreeItem");
      app.sendMessage("openTreeItem "+app.arguments().at(openTreeItemIndex+1));
      exit(0);
    }
    else
    {
      QString message="Unknown control option.\n";
      printf("%s", message.toLocal8Bit().data());
      exit(4);
    }
  }

}


int main(int argc, char ** argv)
{
 printf("\n\rStart MyTetra v.%d.%d.%d\n\r", APPLICATION_RELEASE_VERSION, APPLICATION_RELEASE_SUBVERSION, APPLICATION_RELEASE_MICROVERSION);

 Q_INIT_RESOURCE(mytetra);

 // Начальные инициализации основных объектов

 // Запоминается имя файла запущенного бинарника
 // Файл запущенной программы (нулевой аргумент функции main)
 // Метод fromLocal8Bit корректно работает для 8-ми битных кодировок локали (Win) и для UTF-8 (Lin)
 // даже если в имени файла встречаются национальные символы
 // а кодек еще не установлен
 QString mainProgramFile=QString::fromLocal8Bit( argv[0] ); // Данные запоминаются в сыром виде и никак не интерпретируются до использования
 globalParameters.setMainProgramFile(mainProgramFile);

 // Перехват отладочных сообщений
 setDebugMessageHandler();

 // Создание объекта приложения
 QtSingleApplication app(argc, argv);

 // Обработка консольных опций
 parseConsoleOption(app);


 // Установка увеличенного разрешения для дисплеев с большим DPI (Retina)
 if( qApp->devicePixelRatio() > 1.0 )
  qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);


 // Инициализация глобальных параметров,
 // внутри происходит установка рабочей директории, настройка кодеков для локали и консоли
 globalParameters.init();

 // Инициализация основных конфигурирующих программу переменных
 mytetraConfig.init();
 if( !globalParameters.getInstallAutodetectLang().isEmpty() )
 {
     // Если была процедура инсталляции, в конфиг записывается автоопределенный язык
     mytetraConfig.set_interfacelanguage( globalParameters.getInstallAutodetectLang() );
 }

 // Инициализация переменных, отвечающих за хранилище данных
 dataBaseConfig.init();

 // Проверяется наличие коллекции прикрепляемых к веткам иконок (и иконки создаются если они отсутствуют)
 IconSelectDialog::iconsCollectionCheck();

 // Установка CSS-оформления
 CssHelper::setCssStyle();

 // Инициализация логирования действий над данными
 actionLogger.init();
 actionLogger.setEnableLogging( mytetraConfig.getEnableLogging() ); // Установка или запрещение логирования согласно конфигурации
 actionLogger.addAction("startProgram"); // Запись в лог старта программы


 // Экран загрузки, показывается только в Андроид версии (так как загрузка идет ~10 сек, и без сплешскрина непонятно что происходит)
 QSplashScreen splash(QPixmap(":/resource/pic/mytetra_splash.png"));
 if(mytetraConfig.getShowSplashScreen())
   splash.show();


 // Подключение перевода интерфейса
 QString langFileName=":/resource/translations/mytetra_"+mytetraConfig.get_interfacelanguage()+".qm";
 qDebug() << "Use language file " << langFileName;
 QTranslator langTranslator;
 langTranslator.load(langFileName);
 app.installTranslator(&langTranslator);

 //Загрузка переводов стандартных диалогов и кнопок
 QTranslator qtTranslator;
 if(qtTranslator.load("qt_" + QLocale().name().split('_').first(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
 {
     if(!app.installTranslator(&qtTranslator))
         qDebug() << "Can't install QT translations";
 }

 // Инициализация менеджера горячих клавиш должна происходить после инициализации переводов,
 // чтобы были переведены все действия по горячим клавишам
 shortcutManager.init();

 // Создание объекта главного окна
 MainWindow win;

 // Сразу восстанавливается вид окна в предыдущий запуск
 // Эти действия нельзя делать в конструкторе главного окна, т.к. окно еще не создано
 // Эти действия надо делать до установки заголовка. В некоторых оконных средах,
 // если сделать после setWindowTitle(), геометрия восстановится некорректно, окно съедет вверх на толщину заголовка
 win.restoreAllWindowState();

 // Настройка объекта главного окна
 win.setWindowTitle("MyTetra");
 if(globalParameters.getTargetOs()=="android")
   win.show(); // В Андроиде нет десктопа, на нем нельзя сворачивать окно
 else
 {
   if(mytetraConfig.get_runinminimizedwindow()==false)
     win.show();
   else
     win.hide();
 }

 // Восстанавливаются открепляемые окна
 win.restoreDockableWindowsState();

 // Восстановление видимости виджета, который был активный, для мобильного интерфейса
 if(mytetraConfig.getInterfaceMode()=="mobile")
   globalParameters.getWindowSwitcher()->restoreFocusWidget();

 qDebug() << "Restore session succesfull";

 // В момент восстановления главного окна восстановилась и последняя редактируемая запись
 // История перехода очищается, так как в нее может попасть
 // первая запись в востаналиваемой ветке и сама восстанавливаемая запись
 walkHistory.clear();


 // Если в конфиге настроено, что нужно синхронизироваться при старте
 // И задана команда синхронизации
 if(mytetraConfig.get_synchroonstartup())
  if(mytetraConfig.get_synchrocommand().trimmed().length()>0)
   win.synchronization();


 // Если настроено в конфиге, сразу запрашивается пароль доступа
 // к зашифрованным данным
 // И если есть хоть какие-то зашифрованные данные
 if(mytetraConfig.get_howpassrequest()=="atStartProgram")
  if(globalParameters.getCryptKey().length()==0)
   if(dataBaseConfig.get_crypt_mode()>0)
    {
     // Запрашивается пароль только в том случае, если ветка,
     // на которую установливается курсор при старте, незашифрована
     // Если ветка зашифрована, пароль и так будет запрошен автоматически
     if(win.isTreePositionCrypt()==false)
      {
       Password password;
       password.retrievePassword();
      }
    }


 // Если в общем конфиге стоит опция хранения пароля
 // И хранимый пароль (точнее его хеш) заполнен
 if(globalParameters.getCryptKey().length()==0)
  if(dataBaseConfig.get_crypt_mode()>0)
   if(mytetraConfig.getPasswordSaveFlag())
    if(mytetraConfig.getPasswordMiddleHash().length()>0)
     {
      // При запросе пароля ключ шифрования будет восстановлен автоматически,
      // если он правильный и подходит для текущих данных
      Password password;
      password.retrievePassword();
     }

 // Распечатывается дерево сгенерированных объектов
 // print_object_tree();


 // Проверяется наличие системного трея
 /*
 if(!QSystemTrayIcon::isSystemTrayAvailable()) {
  QMessageBox::critical(0, QObject::tr("Systray"),
                        QObject::tr("I couldn't detect any system tray on this system."));
  exit(1);
 }
 */

 // Инициалиация периодической проверки изменения базы сторонними программами
 periodicCheckBase.init();
 periodicCheckBase.setDelay( mytetraConfig.getCheckBasePeriod() );
 periodicCheckBase.start();

 // Инициалиация периодической синхронизации
 periodicSyncro.init();
 periodicSyncro.setDelay( mytetraConfig.getPeriodicSyncroPeriod() );
 periodicSyncro.start();

 // При закрытии окна не выходить из программы.
 // Окно программы может быть снова открыто из трея
 QApplication::setQuitOnLastWindowClosed(false);

 app.connect(&app, &QtSingleApplication::lastWindowClosed, &app, &QtSingleApplication::quit);

 // Прием сообщений, испускаемых другим экземпляром MyTetra с помощью консольных команд "--control"
 app.connect(&app, &QtSingleApplication::messageReceived,  &win, &MainWindow::messageHandler);

 // Окно сплеш-скрина скрывается
 if(mytetraConfig.getShowSplashScreen())
   splash.finish(&win);

 return app.exec();
}
