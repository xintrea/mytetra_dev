#include <sys/timeb.h>

#include <QTranslator>

#include "main.h"
#include "MainWindow.h"
#include "AppConfig.h"
#include "ClipboardRecords.h"
#include "TrashMonitoring.h"
#include "GlobalParameters.h"
#include "qtsingleapplication/qtsingleapplication.h"
#include "DataBaseConfig.h"
#include "WalkHistory.h"
#include "crypt/RC5Simple.h"
#include "crypt/Password.h"

using namespace std;

// Глобальные параметры программы
GlobalParameters globalParameters;

// Конфигурация программы
AppConfig mytetraconfig;

// Конфигурация данных
DataBaseConfig dataBaseConfig;

// Объект слежения за состоянием корзины
TrashMonitoring trashmonitoring;

// Объект с историей посещаемых записей
WalkHistory walkHistory;

// Указатель на основное окно программы
QObject *pMainWindow;


void logprint(char *lpszText, ...)
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


void critical_error(QString message)
{
 qDebug() << " ";
 qDebug() << "---------------";
 qDebug() << "Critical error!";
 qDebug() << "---------------";
 qDebug() << message;
 
 QMessageBox::critical(qobject_cast<QWidget *>(pMainWindow), "Critical error",
                             message+"\n\n Programm was closed.",
                             QMessageBox::Ok); 

 exit(1);
}


void info_window(QString i)
{
 QTextEdit *textArea=new QTextEdit;
 textArea->setPlainText(i);

 QPushButton *closeButton=new QPushButton;
 closeButton->setText(QObject::tr("Close"));
 closeButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

 QVBoxLayout *assemblyLayout=new QVBoxLayout;
 assemblyLayout->addWidget(textArea);
 assemblyLayout->addWidget(closeButton,0,Qt::AlignRight);

 QWidget *win=new QWidget;
 win->setLayout(assemblyLayout);
 win->setWindowModality(Qt::ApplicationModal);

 QObject::connect(closeButton,SIGNAL(clicked()),win,SLOT(close ()));

 win->show();
}


QString xmlnode_to_string(QDomNode xmlData)
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


// Удаление директории с копированием содержимого в корзину
void remove_directory_to_trash(QString nameDirFrom)
{
 QDir dirfrom(nameDirFrom);
 QStringList fileList=dirfrom.entryList();

 QString nameDirTo=mytetraconfig.get_trashdir();

 // Перебор всех файлов в удаляемой директории
 for(int i=0;i<fileList.size();i++)
 {
  // Директории с именами "." и ".." обрабатывать не нужно
  if(fileList.at(i)=="." || fileList.at(i)=="..")continue;

  // Исходный файл, который будет перенесен в корзину
  QString fileNameFrom=nameDirFrom+"/"+fileList.at(i);

  // Конечный файл, который должен лежать в корзине
  QString fileNameToShort;
  QString fileNameTo;
  bool targetFileFree=false;
  do {
   fileNameToShort=get_unical_id()+"_"+fileList.at(i);
   fileNameTo       =nameDirTo+"/"+fileNameToShort;
  
   if(QFile::exists(fileNameTo)) targetFileFree=false;
   else targetFileFree=true;
  } while(!targetFileFree);

  qDebug() << "Move file from " << fileNameFrom << " to " << fileNameTo;

  // Перенос файла в корзину
  if( QFile::rename(fileNameFrom,fileNameTo)==true )
   trashmonitoring.addFile(fileNameToShort); // Оповещение что в корзину добавлен файл
  else
   critical_error("Can not remove file\n"+fileNameFrom+"\nto directory\n"+nameDirTo+"\nwith new name\n"+fileNameTo);
 }

 // Удаление директории
 // Из-за проблем с синтаксисом метода rmdir(), нельзя удалить ту
 // директорию, на которую указывает объект, поэтому удаление происходит
 // через дополнительный QDir объект, который указывает на директорию
 // где лежит бинарник.
 // Если в rmdir() передать относительный путь, то будет удалена директория
 // относительно директории бинарника.
 // Если в rmdir() передать асолютный путь, то будет удалена директория
 // по абсолютному пути
 QDir applicationdir(QCoreApplication::applicationDirPath());
 qDebug() << "Try delete directory " << nameDirFrom;
 if(!applicationdir.rmdir(nameDirFrom))
  qDebug() << "Directory " << nameDirFrom << " NOT deleted";
 else
  qDebug() << "Directory " << nameDirFrom << " delete succesfull";
}


// Удаление файла с копированием его копии в корзину
void remove_file_to_trash(QString fileNameFrom)
{
 // Получение короткого имени исходного файла
 QFileInfo fileInfo(fileNameFrom);
 QString fileNameFromShort=fileInfo.fileName();
 
 // Получение имени файла для сохранения в корзине
 QString fileNameToShort=get_unical_id()+"_"+fileNameFromShort;
 QString fileNameTo     =mytetraconfig.get_trashdir()+"/"+fileNameToShort;
 
 qDebug() << "Move file from " << fileNameFrom << " to " << fileNameTo;
 
 // Файл перемещается в корзину
 if( QFile::rename(fileNameFrom,fileNameTo)==true )
  trashmonitoring.addFile(fileNameToShort); // Оповещение что в корзину добавлен файл
 else
  critical_error("Can not remove file\n"+fileNameFrom+"\nto reserve file\n"+fileNameTo);
}


// Создание временной директории
QString create_temp_directory(void)
{
 QDir dir;
 QString systemTempDirName=dir.tempPath();

 QString temp_dir_name="mytetra"+get_unical_id();

 // Создается директория
 dir.setPath(systemTempDirName);
 dir.mkdir(temp_dir_name);

 QString createTempDirName=systemTempDirName+"/"+temp_dir_name;

 qDebug() << "Create temporary directory "+createTempDirName;

 return createTempDirName;
}


bool remove_directory(const QString &dirName)
{
  bool result = true;
  QDir dir(dirName);

  if (dir.exists(dirName))
   {
    Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
     {
      if (info.isDir())
       result = remove_directory(info.absoluteFilePath());
      else
       result = QFile::remove(info.absoluteFilePath());

      if(!result)
       return result;
     }

    result = dir.rmdir(dirName);
   }

  return result;
}


// Копирование содержимого директории
// Копируются только файлы
bool copy_directory(const QString &fromName, const QString &toName)
{
  QDir fromDir(fromName);
  QDir toDir(toName);

  if (fromDir.exists() && toDir.exists())
   {
    Q_FOREACH(QFileInfo info, fromDir.entryInfoList(QDir::Files))
     {
      QFile::copy(info.absoluteFilePath(), toName+"/"+info.fileName());
     }

    return true;
   }

  return false;
}


// Получение списка файлов с их содержимым в указанной директории
QMap<QString, QByteArray> get_files_from_directory(QString dirName, QString fileMask)
{
 QMap<QString, QByteArray> result;
 QDir directory(dirName);

 if(directory.exists())
  {
   QStringList filter;
   filter << fileMask;
   
   foreach(QFileInfo info, directory.entryInfoList(filter, QDir::Files))
    {
     QFile f(info.absoluteFilePath());
     if(!f.open(QIODevice::ReadOnly))
      critical_error("get_files_from_directory() : File '"+info.absoluteFilePath()+"' open error");

     // Содержимое файла
     QByteArray b = f.readAll();

     // Содержимое файла сохраняется с ключем в виде имени файла
     result.insert(info.fileName(), b);
    }
  }
 else
  qDebug() << "get_files_from_directory() : Can not find directory" << dirName;

 return result;
}


bool save_files_to_directory(QString dirName, QMap<QString, QByteArray> fileList)
{
 qDebug() << "save_files_to_directory() : Directory name " << dirName;

 QDir directory(dirName);

 // Если директория существует
 if(directory.exists())
  {
   foreach (QString filename, fileList.keys())
   {
    qDebug() << "save_files_to_directory() : Save file " << filename;

    QFile file(dirName+"/"+filename);

    // Файл открывается для записи
    if(!file.open(QIODevice::WriteOnly))
     {
      qDebug() << "save_files_to_directory() : Can not save file '" << filename << "' to directory '" << dirName << "'";
      return false;
     }

    // Данные сохраняются в файл
    file.write(fileList.value(filename));
   }

   return true;
  }
 else
  {
   qDebug() << "save_files_to_directory() : Can not find directory" << dirName;
   return false;
  }
}


// Преобразование из QString в обычный char
char* fromQStringToChar( const QString& str )
{
 char *tmpC=new char [str.size() + 1];
 QVariant var;

 for(int i=0;i<str.length();i++)
  {
   var=str.at(i);
   tmpC[i] = var.toChar().toAscii();
  }

 tmpC[str.size()] = 0;

 return tmpC;
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


void insert_action_as_button(QToolBar *tools_line, QAction *action)
{
 tools_line->addAction(action);
 qobject_cast<QToolButton*>(tools_line->widgetForAction(action ))->setAutoRaise(false);
}


int imax(int x1, int x2)
{
 if(x1>x2)return x1;
 else return x2;
}


int imin(int x1, int x2)
{
 if(x1<x2)return x1;
 else return x2;
}


void myMessageOutput(QtMsgType type, const char *msg)
{
 // #if DEBUG_PRINT==1


if(!mytetraconfig.is_init())
 {
  fprintf(stderr, "%s\n", msg);
  return;
 }

 // Если в конфигурации запрещен вывод отладочных сообщений
 if(!mytetraconfig.get_printdebugmessages()) return;

 switch (type) {
   case QtDebugMsg:
       fprintf(stderr, "[DBG] %s\n", msg);
       break;
   case QtWarningMsg:
       fprintf(stderr, "[WRN] %s\n", msg);
       break;
   case QtCriticalMsg:
       fprintf(stderr, "[CRERR] %s\n", msg);
       break;
   case QtFatalMsg:
       fprintf(stderr, "[FTERR] %s\n", msg);
       abort();
 }

 // #endif
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
 srand(seed1);

 unsigned int delay=rand()%1000;
 unsigned int r=0;
 for(unsigned int i=0; i<delay; i++) r=r+rand();

 seed1=seed1-get_milli_count()+r;

 unsigned int seed2=time(NULL);
 unsigned int seed3=seed1+seed2;
 unsigned int seed=seed3;

 // qDebug() << "Random seed1" << seed1;
 // qDebug() << "Random seed2" << seed2;
 // qDebug() << "Random seed3" << seed3;
 // qDebug() << "Random seed" << seed;

 srand( seed );

 // qDebug() << "Random number" << rand();
}


void convertByteArrayToVector(const QByteArray &qba, vector<unsigned char> &vec)
{
 unsigned int size=qba.size();

 vec.resize(size, 0);

 for(unsigned int i=0; i<size; i++)
  vec[i]=(unsigned char)qba[i];
}


void convertVectorToByteArray(const vector<unsigned char> &vec, QByteArray &qba)
{
 unsigned int size=vec.size();

 qba.resize(size);

 for(unsigned int i=0; i<size; i++)
  qba[i]=(unsigned char)vec[i];
}


// Шифрация строки
// Результирующая зашифрованная строка - это набор шифрованных байт,
// представленный в кодировке Base64
QString encryptString(QByteArray key, QString line)
{
 if(line=="") return QString();

 // qDebug() << "Encrypt source" << line;

 vector<unsigned char> vectorKey;
 convertByteArrayToVector(key, vectorKey);

 vector<unsigned char> vectorLineIn;
 convertByteArrayToVector(line.toUtf8(), vectorLineIn);

 vector<unsigned char> vectorLineOut;

 // Шифрация
 RC5Simple rc5;
 rc5.RC5_SetKey(vectorKey);
 rc5.RC5_Encrypt(vectorLineIn, vectorLineOut);


 QByteArray result;
 convertVectorToByteArray(vectorLineOut, result);

 QString resultLine( result.toBase64().data() );

 // qDebug() << "Encrypt result" << resultLine;

 return resultLine;
}


// Расшифровка строки
// Принимаемая зашифрованная строка - это набор шифрованных байт,
// представленный в кодировке Base64
QString decryptString(QByteArray key, QString line)
{
 if(line=="") return QString();

 /*
 qDebug() << "Decrypt source" << line;
 qDebug() << "Decrypt source HEX" << QByteArray::fromBase64( line.toAscii() ).toHex();
 qDebug() << "Decrypt key" << key.toHex();
 */

 vector<unsigned char> vectorKey;
 convertByteArrayToVector(key, vectorKey);

 /*
 printf("Decrypt vector key ");
 for(int i=0; i<vectorKey.size(); i++)
  printf("%.2X", vectorKey[i]);
 printf("\n");
 */

 vector<unsigned char> vectorLineIn;
 convertByteArrayToVector(QByteArray::fromBase64( line.toAscii() ), vectorLineIn);

 /*
 printf("Decrypt vector source HEX ");
 for(int i=0; i<vectorLineIn.size(); i++)
  printf("%.2X", vectorLineIn[i]);
 printf("\n");
 */

 vector<unsigned char> vectorLineOut;

 // Дешифрация
 RC5Simple rc5;
 rc5.RC5_SetKey(vectorKey);
 rc5.RC5_Decrypt(vectorLineIn, vectorLineOut);


 QByteArray result;
 convertVectorToByteArray(vectorLineOut, result);

 QString resultLine=QString::fromUtf8( result.data() );

 // qDebug() << "Decrypt result" << resultLine;

 return resultLine;
}


QByteArray encryptStringToByteArray(QByteArray key, QString line)
{
 if(line=="") return QByteArray();

 vector<unsigned char> vectorKey;
 convertByteArrayToVector(key, vectorKey);

 vector<unsigned char> vectorLineIn;
 convertByteArrayToVector(line.toUtf8(), vectorLineIn);

 vector<unsigned char> vectorLineOut;

 // Шифрация
 RC5Simple rc5;
 rc5.RC5_SetKey(vectorKey);
 rc5.RC5_Encrypt(vectorLineIn, vectorLineOut);

 QByteArray result;
 convertVectorToByteArray(vectorLineOut, result);

 return result;
}


QString decryptStringFromByteArray(QByteArray key, QByteArray data)
{
 if(data.length()==0) return QString();

 vector<unsigned char> vectorKey;
 convertByteArrayToVector(key, vectorKey);

 vector<unsigned char> vectorDataIn;
 convertByteArrayToVector(data, vectorDataIn);

 vector<unsigned char> vectorDataOut;

 // Дешифрация
 RC5Simple rc5;
 rc5.RC5_SetKey(vectorKey);
 rc5.RC5_Decrypt(vectorDataIn, vectorDataOut);


 QByteArray result;
 convertVectorToByteArray(vectorDataOut, result);

 QString resultLine=QString::fromUtf8( result.data() );

 return resultLine;
}


void encryptFile(QByteArray key, QString fileName)
{
 encDecFileSmart(key, fileName, 0);
}


void decryptFile(QByteArray key, QString fileName)
{
 encDecFileSmart(key, fileName, 1);
}


void encDecFileSmart(QByteArray key, QString fileName, int mode)
{
 QFile file(fileName);

 if(!file.open(QIODevice::ReadOnly))
  critical_error("encDecFileSmart() : Cant open binary file "+fileName+" for reading.");

 vector<unsigned char> vectorKey;
 convertByteArrayToVector(key, vectorKey);

 vector<unsigned char> vectorDataIn;
 convertByteArrayToVector(file.readAll(), vectorDataIn);

 vector<unsigned char> vectorDataOut;

 file.close();


 // Шифрация / дешифрация
 RC5Simple rc5;
 rc5.RC5_SetKey(vectorKey);
 if(mode==0)
  rc5.RC5_Encrypt(vectorDataIn, vectorDataOut);
 else
  rc5.RC5_Decrypt(vectorDataIn, vectorDataOut);


 QByteArray result;
 convertVectorToByteArray(vectorDataOut, result);

 if(!file.open(QIODevice::WriteOnly))
  critical_error("encryptFile() : Cant open binary file "+fileName+" for write.");
 file.write(result);
}


int main(int argc, char ** argv)
{
 printf("\n\rStart MyTetra v.%d.%d.%d\n\r", APPLICATION_RELEASE_VERSION, APPLICATION_RELEASE_SUBVERSION, APPLICATION_RELEASE_MICROVERSION);

 Q_INIT_RESOURCE(mytetra);


 // Начальные инициализации основных объектов

 // Файл запущенной программы (нулевой аргумент функции main)
 QString mainProgramFile=QString::fromAscii( argv[0] );
 qDebug() << "Set main program file to " << mainProgramFile;

 // Запоминается имя файла запущенного бинарника
 globalParameters.setMainProgramFile(mainProgramFile);


 qInstallMsgHandler(myMessageOutput);

 QtSingleApplication app(argc, argv);

 // Не запущен ли другой экземпляр
 if(app.isRunning())
  {
   printf("Another MyTetra exemplar is running.\n");
   exit(0);
  }

 // Установка кодека текстов
 QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));


 // Инициализация глобальных параметров,
 // внутри происходит установка рабочей директории
 globalParameters.init();

 // Инициализация основных конфигурирующих программу переменных
 mytetraconfig.init();

 // Инициализация переменных, отвечающих за хранилище данных
 dataBaseConfig.init();

 // Подключение перевода интерфейса
 // QString langFileName=globalParameters.getWorkDirectory()+"/resource/translations/mytetra_"+mytetraconfig.get_interfacelanguage()+".qm";
 QString langFileName=":/resource/translations/mytetra_"+mytetraconfig.get_interfacelanguage()+".qm";
 qDebug() << "Use language file " << langFileName;

 QTranslator langTranslator;
 langTranslator.load(langFileName);
 app.installTranslator(&langTranslator);


 // Создание объекта главного окна
 MainWindow win;
 win.setWindowTitle("MyTetra");
 if(mytetraconfig.get_runinminimizedwindow()==false)
  win.show();
 else
  win.hide();

 // win.setObjectName("mainwindow");
 // pMainWindow=&win; // Запоминается указатель на основное окно

 // После создания окна восстанавливается вид окна в предыдущий запуск
 // Эти действия нельзя делать в конструкторе главного окна, 
 // т.к. окно еще не создано
 win.restoreFindOnBaseVisible();
 win.restoreGeometry();
 win.restoreTreePosition();
 win.restoreRecordTablePosition();
 win.restoreEditorCursorPosition();
 win.restoreEditorScrollBarPosition();

 qDebug() << "Restore session succesfull";

 // После восстановления последней редактируемой записи
 // история перехода очищается, так как в не может попасть
 // первая запись в востаналиваемой ветке и сама восстанавливаемая запись
 walkHistory.clear();


 // Если в конфиге настроено, что нужно синхронизироваться при старте
 // И задана команда синхронизации
 if(mytetraconfig.get_synchroonstartup())
  if(mytetraconfig.get_synchrocommand().trimmed().length()>0)
   win.synchronization();


 // Если настроено в конфиге, сразу запрашивается пароль доступа
 // к зашифрованным данным
 // И если есть хоть какие-то зашифрованные данные
 if(mytetraconfig.get_howpassrequest()=="atStartProgram")
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
   if(mytetraconfig.getPasswordSaveFlag())
    if(mytetraconfig.getPasswordMiddleHash().length()>0)
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
 return app.exec();
}
