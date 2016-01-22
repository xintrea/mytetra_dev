#include <sys/timeb.h>

#include <QTranslator>
#include <QToolButton>
#include <QSplashScreen>

#if QT_VERSION >= 0x050000
#include <QScroller>
#include <QScrollerProperties>
#include <QScrollBar>
#endif

#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/ClipboardRecords.h"
#include "libraries/TrashMonitoring.h"
#include "libraries/FixedParameters.h"
#include "libraries/GlobalParameters.h"

#if QT_VERSION < 0x050000
#include "libraries/qtSingleApplication/qtsingleapplication.h"
#else
#include "libraries/qtSingleApplication5/qtsingleapplication.h"
#endif

#include "models/dataBaseConfig/DataBaseConfig.h"
#include "libraries/WalkHistory.h"
#include "libraries/WindowSwitcher.h"
#include "libraries/crypt/RC5Simple.h"
#include "libraries/crypt/Password.h"
#include "libraries/TraceLogger.h"

using namespace std;

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

// Указатель на основное окно программы
QObject *pMainWindow;


void logPrint(char *lpszText, ...)
{
  va_list argList;
  FILE *pFile;

  // инициализируем список аргументов
  va_start(argList, lpszText);

  // открываем лог-файл для добавления данных
  if((pFile = fopen("mytetralog.txt", "a+")) == NULL)
  {
    printf("\nLog file not writable\n");
    return;
  }

  // пишем текст в файл
  vfprintf(pFile, lpszText, argList);
  // putc('\n', pFile);

  // пишем текст на экран
  vprintf(lpszText, argList);

  // закрываем файл
  fclose(pFile);
  va_end(argList);

  // успешное завершение
  return;
}


void criticalError(QString message)
{
  qDebug() << " ";
  qDebug() << "---------------";
  qDebug() << "Critical error!";
  qDebug() << "---------------";
  qDebug() << message;
  qDebug() << "---------------";
  qDebug() << " ";

  QMessageBox::critical(qobject_cast<QWidget *>(pMainWindow), "Critical error",
                        message+"\n\nProgramm was closed.",
                        QMessageBox::Ok);

  exit(1);
}


// Функция-помощник при отладке генерации XML-кода. Преобразует узел DOM в строку
QString xmlNodeToString(QDomNode xmlData)
{
  // Если узел представляет собой полностью документ
  if(xmlData.isDocument())
  {
    // Значит этот узел можно напрямую преобразовать
    // в документ, а потом в XML строку
    return xmlData.toDocument().toString();
  }
  else
  {
    // Иначе узел не является документом, и его нужно обрабатывать по-другому

    // Строка, где будет размещен XML код
    QString xmlcode;

    // Поток, связаный со строкой, в который будет направляться XML код узла
    QTextStream stream(&xmlcode, QIODevice::WriteOnly);

    // XML документ записывается в поток, после чего автоматом окажется в строке xmlcode
    xmlData.save(stream, 1);

    return xmlcode;
  }
}


// Преобразование из QString в обычный char
char* fromQStringToChar( const QString& str )
{
  /*
 char *tmpC=new char [str.size() + 1];
 QVariant var;

 for(int i=0;i<str.length();i++)
  {
   var=str.at(i);
   tmpC[i] = var.toChar().toAscii();
  }

 tmpC[str.size()] = 0;

 return tmpC;
 */

  return str.toLocal8Bit().data();
}


// Рекурсивная печать дерева объектов, т.к. dumpObjectInfo() и dumpObjectTree() не работают
void print_object_tree_recurse(QObject *pobj)
{
  static int indent=0;

  QObjectList olist;

  olist=pobj->children();

  for(int i=0;i<olist.size();++i)
  {
    QObject *currobj;
    currobj=olist.at(i);

    QString indentline=".";
    for(int j=0;j<indent;j++)indentline=indentline+".";

    if((currobj->objectName()).length()==0)
      qDebug("%s%s",fromQStringToChar(indentline), currobj->metaObject()->className() );
    else
      qDebug("%s%s, NAME %s",fromQStringToChar(indentline), currobj->metaObject()->className(), fromQStringToChar(currobj->objectName()) );

    indent++;
    print_object_tree_recurse(currobj);
    indent--;
  }
}


// Печать дерева объектов, основная функция
void print_object_tree(void)
{
  qDebug() << "Object tree";

  print_object_tree_recurse(pMainWindow);
}


// Функция для сортировки массива из QStringList исходя из длин списков
bool compare_QStringList_len(const QStringList &list1, const QStringList &list2)
{
  return list1.size() < list2.size();
}


void insertActionAsButton(QToolBar *tools_line, QAction *action)
{
  tools_line->addAction(action);
  qobject_cast<QToolButton*>(tools_line->widgetForAction(action))->setAutoRaise(false);
}

void smartPrintDebugMessage(QString msg)
{
 if(globalParameters.getTargetOs()=="any" ||
    globalParameters.getTargetOs()=="meego")
  {
   QTime currTime = QTime::currentTime();
   QString timeText=currTime.toString("hh:mm:ss");
   msg=timeText+" "+msg;

   unsigned int messageLen=msg.toLocal8Bit().size();
   // printf("Len of line: %d\n", messageLen);

   fwrite(msg.toLocal8Bit().data(), sizeof(char), messageLen, stderr);
  }
 
 // В Android пока неясно, как смотреть поток ошибок, для андроида qDebug() не переопределяется
}


// Обработчик (хендлер) вызовов qDebug()
// Внутри этого обработчика нельзя использовать вызовы qDebug(), т. к. получится рекурсия
#if QT_VERSION < 0x050000
void myMessageOutput(QtMsgType type, const char *msg)
#else
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msgText)
#endif
{
 #if QT_VERSION >= 0x050000
 Q_UNUSED(context);
 #endif

 // #if DEBUG_PRINT==1

 #if QT_VERSION < 0x050000
  QString msgText( QString::fromUtf8(msg) );
 #endif


 // Пока идет инициализация конфигурации программы
 if(!mytetraConfig.is_init())
 {
  smartPrintDebugMessage("[INI] "+msgText+"\n");
  return;
 }

 // Если в конфигурации запрещен вывод отладочных сообщений
 if(!mytetraConfig.get_printdebugmessages())
  return;

 switch (type) {
   case QtDebugMsg:
       smartPrintDebugMessage("[DBG] "+msgText+"\n");
       break;
   case QtWarningMsg:
       smartPrintDebugMessage("[WRN] "+msgText+"\n");
       break;
   case QtCriticalMsg:
       smartPrintDebugMessage("[CRERR] "+msgText+"\n");
       break;
   case QtFatalMsg:
       smartPrintDebugMessage("[FTERR] "+msgText+"\n");
       abort();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
   case QtInfoMsg:
       smartPrintDebugMessage("[INF] "+msgText+"\n");
       break;
#endif
 }

 // #endif
}


void setDebugMessageHandler()
{
 qDebug() << "Debug message before set message handler for target OS: " << globalParameters.getTargetOs();

 // Для десктопных операционок можно переустановить обработчик qDebug()
 // Для Андроида переустановка qDebug() приводит к невозможности получения отладочных сообщений в удаленном отладчике
 if(globalParameters.getTargetOs()=="any" ||
    globalParameters.getTargetOs()=="meego")
  {
   qDebug() << "Set alternative handler myMessageOutput() for debug message";

   #if QT_VERSION < 0x050000
    qInstallMsgHandler(myMessageOutput);
   #else
    qInstallMessageHandler(myMessageOutput);
   #endif
  }

 qDebug() << "Debug message after set message handler";
}


// Выдача на экран простого окна с сообщением
void showMessageBox(QString message)
{
  QMessageBox msgBox;
  msgBox.setText(message);
  msgBox.exec();
}


int getScreenSizeY(void)
{
#if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
  int size=(qApp->desktop()->availableGeometry()).height();
#endif

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
  int size=(QApplication::screens().at(0)->availableGeometry()).height();
#endif

  return size;
}


int getScreenSizeX(void)
{
#if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
  int size=(qApp->desktop()->availableGeometry()).width();
#endif

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
  int size=(QApplication::screens().at(0)->availableGeometry()).width();
#endif

  return size;
}


qreal getCalculateIconSizePx(void)
{
#if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
  qreal dpiX=qApp->desktop()->physicalDpiX();
  qreal dpiY=qApp->desktop()->physicalDpiY();
  qreal dpi=(dpiX+dpiY)/2;
#endif

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
  qreal dpi=QApplication::screens().at(0)->physicalDotsPerInch();
#endif

  qreal iconSizeMm=6; // Размер иконки в миллиметрах (рекомендованный)
  qreal iconSizeInch=iconSizeMm/25.4; // Размер иконки в дюймах
  qreal iconSizePx=iconSizeInch*dpi;

  return iconSizePx;
}


// Замена в CSS-стиле все вхождения подстроки META_ICON_SIZE на вычисленный размер иконки в пикселях
QString replaceCssMetaIconSize(QString styleText)
{
  styleText.replace( "META_ICON_SIZE", QString::number( (int) getCalculateIconSizePx() ) );
  styleText.replace( "META_ICON_HALF_SIZE", QString::number( (int)getCalculateIconSizePx()/2 ) );
  styleText.replace( "META_ICON_TWO_THIRDS_SIZE", QString::number( ((int)getCalculateIconSizePx()*2)/3 ) );
  styleText.replace( "META_ICON_QUARTER_SIZE", QString::number( (int)getCalculateIconSizePx()/4 ) );
  styleText.replace( "META_ICON_FIFTH_SIZE", QString::number( (int)getCalculateIconSizePx()/5 ) );
  styleText.replace( "META_ICON_SIXTH_SIZE", QString::number( (int)getCalculateIconSizePx()/6 ) );

  return styleText;
}


void setCssStyle()
{
  QString csspath = globalParameters.getWorkDirectory()+"/stylesheet.css";

  QFile css(csspath);

  bool openResult=css.open(QIODevice::ReadOnly | QIODevice::Text);

  // Если файла не существует
  if(!openResult)
  {
    qDebug() << "Stylesheet not found in " << csspath << ". Create new css file.";
    globalParameters.createStyleSheetFile( globalParameters.getWorkDirectory() );
  }
  css.close();

  // Заново открывается файл
  if(css.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug() << "Stylesheet success loaded from" << csspath;
    QString style = QTextStream(&css).readAll();

    style=replaceCssMetaIconSize(style);

    qApp->setStyleSheet(style);
  }
}


void setKineticScrollArea(QAbstractItemView *object)
{
  #if QT_VERSION < 0x050000

   Q_UNUSED(object);
   return;

  #else

  if(object==NULL)
    return;

  if(globalParameters.getTargetOs()=="android")
  {
    // Настройка жестов прокрутки
    QScroller *scroller = QScroller::scroller(object);

    // For desktop - QScroller::LeftMouseButtonGesture, for Android - QScroller::TouchGesture in doc
    // TouchGesture по факту на Андроиде не работает, а LeftMouseButtonGesture - почему-то работает
    scroller->grabGesture(object, QScroller::LeftMouseButtonGesture);

    // Поведение прокрутки на краях списка (сейчас не пружинит)
    QScrollerProperties properties = scroller->scrollerProperties();
    QVariant overshootPolicy = QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
    properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, overshootPolicy);
    properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, overshootPolicy);
    scroller->setScrollerProperties(properties); // QScrollerProperties::OvershootAlwaysOff

    // Горизонтальный скроллинг скрывается
    object->horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");

    // QScrollBar::add-line:horizontal { border: none; background: none; } QScrollBar::sub-line:horizontal { border: none; background: none; }
    // QScrollBar {width:3px;}
    // QScrollBar::up-arrow, QScrollBar::down-arrow {width: 0px; height: 0px;}
    // QScrollBar::add-line:vertical { height: 0px; } QScrollBar::sub-line:vertical { height: 0px; }
    // QScrollBar::add-line:vertical { border: none; background: none; height: 0px; } QScrollBar::sub-line:vertical { border: none; background: none; height: 0px; }
    // background: transparent; background-color:transparent;
    // "QScrollBar::up-arrow, QScrollBar::down-arrow {width: 0px; height: 0px;}"
    object->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width:3px; border: none; background: transparent; margin: 0;}"
                                               "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {width: 0px; height: 0px; border: none;  background: transparent; image: url(:/resource/pic/transparent_dot.png); }"
                                               "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { image: url(:/resource/pic/transparent_dot.png); }");



    /*
    object->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal {border: 2px solid black; background: grey; height: 15px;}"
                                                 "QScrollBar::add-line:horizontal {border none; background: none;}"
                                                 "QScrollBar::sub-line:horizontal {border none; background: none;}"
                                                 );
    */

    object->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    // setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  }
  
  #endif
}



QStringList text_delimiter_decompose(QString text)
{
 text.replace('"',' ');
 text.replace("'"," ");
 text.replace('.',' ');
 text.replace(',',' ');
 text.replace(';',' ');
 text.replace(':',' ');
 text.replace('-',' ');
 text.replace('?',' ');
 text.replace('!',' ');

 QStringList list = text.split(QRegExp("\\W+"), QString::SkipEmptyParts);

 return list;
}


// Функция всегда возвращает уникальный идентификатор
QString get_unical_id(void)
{
 // Уникальный идентификатор состоит из 10 цифр количества секунд с эпохи UNIX
 // и 10 случайных символов 0-9 a-z

 // Количество секунд как число
 long seconds;
 seconds=(long)time(NULL);

 // Количество секунд как строка
 QString secondsLine=QString::number(seconds, 10);
 secondsLine=secondsLine.rightJustified(10, '0');

 // Строка из 10 случайных символов
 QString symbols="0123456789abcdefghijklmnopqrstuvwxyz";
 QString line;

 for(int i=0; i<10; i++)
  line+=symbols.mid(rand()%symbols.length(), 1);

 QString result=secondsLine+line;

 return result;
}


// Получение псевдо-уникального имени картинки
// Внутренний формат для храния картинок в MyTetra - png
// Вызывающий код должен при необходимости проверять уникальность имени картинки в пределах одной записи
QString getUnicalImageName(void)
{
  return "image"+QString::number(rand())+".png";
}


int get_milli_count(void)
{
  // Something like GetTickCount but portable
  // It rolls over every ~ 12.1 days (0x100000/24/60/60)
  // Use getMilliSpan to correct for rollover
  timeb tb;
  ftime( &tb );
  int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
  return nCount;
}


void init_random(void)
{
 qDebug() << "Init random generator";

 unsigned int seed1=get_milli_count();
 srand(seed1+rand());

 unsigned int delay=rand()%1000;
 unsigned int r=0;
 for(unsigned int i=0; i<delay; i++) r=r+rand();

 seed1=seed1-get_milli_count()+r;

 unsigned int seed2=time(NULL);
 unsigned int seed3=seed1+seed2;
 unsigned int seed=seed3;

 srand( seed );
}


int main(int argc, char ** argv)
{
 printf("\n\rStart MyTetra v.%d.%d.%d\n\r", APPLICATION_RELEASE_VERSION, APPLICATION_RELEASE_SUBVERSION, APPLICATION_RELEASE_MICROVERSION);

 Q_INIT_RESOURCE(mytetra);

 // Начальные инициализации основных объектов

 // Запоминается имя файла запущенного бинарника
 // Файл запущенной программы (нулевой аргумент функции main)
 QString mainProgramFile=QString::fromLatin1( argv[0] ); // todo: Этот код наверно некорректно работает с путями в UTF8
 qDebug() << "Set main program file to " << mainProgramFile;
 globalParameters.setMainProgramFile(mainProgramFile);

 // Перехват отладочных сообщений
 setDebugMessageHandler();

 QtSingleApplication app(argc, argv);

 // Не запущен ли другой экземпляр
 if(app.isRunning())
  {
   QString message="Another MyTetra exemplar is running.\n";

   printf(message.toLocal8Bit());

   QMessageBox msgBox;
   msgBox.setIcon(QMessageBox::Warning);
   msgBox.setText(message);
   msgBox.exec();

   exit(0);
  }

 #if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
 // Установка увеличенного разрешения для дисплеев с большим DPI (Retina)
 if( qApp->devicePixelRatio() > 1.0 )
  qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
 #endif

 #if QT_VERSION < 0x050000
  // Установка кодека текстов
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
 #endif

 // Инициализация глобальных параметров,
 // внутри происходит установка рабочей директории
 globalParameters.init();

 // Инициализация основных конфигурирующих программу переменных
 mytetraConfig.init();

 // Инициализация переменных, отвечающих за хранилище данных
 dataBaseConfig.init();

 // Установка CSS-оформления
 setCssStyle();


 // Экран загрузки, показывается только в Андроид версии (так как загрузка идет ~10 сек, и без сплешскрина непонятно что происходит)
 QSplashScreen splash(QPixmap(":/resource/pic/mytetra_splash.png"));
 if(mytetraConfig.getShowSplashScreen())
   splash.show();


 // Подключение перевода интерфейса
 // QString langFileName=globalParameters.getWorkDirectory()+"/resource/translations/mytetra_"+mytetraconfig.get_interfacelanguage()+".qm";
 QString langFileName=":/resource/translations/mytetra_"+mytetraConfig.get_interfacelanguage()+".qm";
 qDebug() << "Use language file " << langFileName;

 QTranslator langTranslator;
 langTranslator.load(langFileName);
 app.installTranslator(&langTranslator);


 // Создание объекта главного окна
 MainWindow win;
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

 // win.setObjectName("mainwindow");
 // pMainWindow=&win; // Запоминается указатель на основное окно

 // После создания окна восстанавливается вид окна в предыдущий запуск
 // Эти действия нельзя делать в конструкторе главного окна, 
 // т.к. окно еще не создано
 globalParameters.getWindowSwitcher()->disableSwitch();
 win.restoreFindOnBaseVisible();
 win.restoreGeometry();
 win.restoreTreePosition();
 win.restoreRecordTablePosition();
 win.restoreEditorCursorPosition();
 win.restoreEditorScrollBarPosition();
 globalParameters.getWindowSwitcher()->enableSwitch();

 if(mytetraConfig.getInterfaceMode()=="mobile")
   globalParameters.getWindowSwitcher()->restoreFocusWidget();

 qDebug() << "Restore session succesfull";

 // После восстановления последней редактируемой записи
 // история перехода очищается, так как в не может попасть
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
      // При запросе пароля ключ шифрования будет восстановлен автоматически
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

 // При закрытии окна не выходить из программы.
 // Окно программы может быть снова открыто из трея
 QApplication::setQuitOnLastWindowClosed(false);


 // win.show();
 app.connect(&app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

 // app.connect(&app, SIGNAL(app.commitDataRequest(QSessionManager)), SLOT(win.commitData(QSessionManager)));

 // Окно сплеш-скрина скрывается
 if(mytetraConfig.getShowSplashScreen())
   splash.finish(&win);

 return app.exec();
}
