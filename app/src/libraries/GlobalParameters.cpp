#include <cstdlib>

#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QInputDialog>

#include "GlobalParameters.h"

#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "views/recordTable/RecordTableScreen.h"
#include "views/findInBaseScreen/FindScreen.h"
#include "views/installDialog/InstallDialog.h"

#include "libraries/WindowSwitcher.h"
#include "libraries/FixedParameters.h"
#include "libraries/helpers/DebugHelper.h"

#include "models/appConfig/AppFiles.h"


#ifdef Q_OS_WIN32
#include "windows.h"
#endif


extern AppFiles mytetraFiles;


GlobalParameters::GlobalParameters(QObject *pobj) : QObject(pobj)
{

}

GlobalParameters::~GlobalParameters()
{

}


void GlobalParameters::setMainProgramFile(QString file)
{
 m_mainProgramFile=file;
}


QString GlobalParameters::getMainProgramFile(void)
{
 return m_mainProgramFile;
}


void GlobalParameters::init(void)
{
 m_pointTreeScreen=nullptr;
 m_pointRecordTableScreen=nullptr;
 m_pointFindScreen=nullptr;
 m_pointMetaEditor=nullptr;
 m_pointStatusBar=nullptr;
 m_windowSwitcher=nullptr;

 initCodepage(); // устанавливаются кодеки локали и кодеки консоли

 // После установки кодеков можно показать имя бинарника, и оно должно отобразиться правильно
 // даже если путь содержит каталог с национальными символами
 qDebug() << "Set main program file to " << m_mainProgramFile;

 initWorkDirectory(); // Инициализация рабочей директории
}


QString GlobalParameters::getInitSystemCodepage()
{
#ifdef Q_OS_WIN32
    return "CP "+QString::number( GetACP() );
#endif

#ifdef Q_OS_LINUX
    return "UTF-8";
#endif

    return "";
}


QString GlobalParameters::getInitConsoleCodepage()
{
#ifdef Q_OS_WIN32
    return "CP "+QString::number( GetOEMCP() );
#endif

#ifdef Q_OS_LINUX
    return "UTF-8";
#endif

    return "";
}


void GlobalParameters::initCodepage(void)
{
    m_systemCodepage=getInitSystemCodepage();
    qDebug() << "System code page: " << m_systemCodepage;

    m_consoleCodepage=getInitConsoleCodepage();
    qDebug() << "Console code page: " << m_consoleCodepage;

    // Системная кодировка (кодировка локали) устанавливается как основная
    if(m_systemCodepage.size()>0) {
        QTextCodec::setCodecForLocale(QTextCodec::codecForName(m_systemCodepage.toLatin1()));
    }
}


// Системная кодировка, она же кодировка локали
// В этой кодировке происходит работа с именами файлов и директорий
QString GlobalParameters::getSystemCodepage()
{
    return m_systemCodepage;
}


// Кодировка консоли
QString GlobalParameters::getConsoleCodepage()
{
    return m_consoleCodepage;
}


// Инициализация рабочей директории
// Если рабочая директория уже существует, она будет установлена как рабочая
// Если ребочая директория не будет найдена, будут создана новая рабочая директория
// с начальными файлами и она будет установлена как рабочая
void GlobalParameters::initWorkDirectory(void)
{
 // Если рабочая директория найдена автоматически
 if(findWorkDirectory())
  return;

 // Рабочая директория не найдена, и нужно создать начальные данные

 // Проверяется, можно ли создавать файлы в директории бинарника
 // То есть, возможно ли создание "портируемой" версии

 bool enablePortable=false;

 // Путь к директории, где лежит бинарник
 QFileInfo mainProgramFileInfo(m_mainProgramFile);
 QString fullCurrentPath=mainProgramFileInfo.absolutePath();

 // Проверяется, можно ли читать и писать файлы в этой директории
 QDir currentBinaryDir(fullCurrentPath);
 if(currentBinaryDir.isReadable())
  {
   QFile file(currentBinaryDir.filePath("seed.txt"));
   if(file.open(QIODevice::ReadWrite))
    {
     enablePortable=true; // Портируемую версию можно создавать
     file.close();
     file.remove();
    }
  }

 QString dataDirName=".config/"+getApplicationName();


 InstallDialog installDialog;
 installDialog.setStandartData(dataDirName, QDir::homePath());
 installDialog.setPortableData(fullCurrentPath);
 installDialog.setEnablePortable(enablePortable);
 installDialog.update();
 int result=installDialog.exec();

 if(result==QDialog::Accepted)
 {
     // Надо разобраться, какой режим инсталляции был выбран
     if( installDialog.getInstallType()==InstallDialog::InstallType::Standart)
     {
        mytetraFiles.createStandartProgramFiles(); // Установка файлов рабочей директории в режиме Стандартного приложения
     }
     else
     {
        mytetraFiles.createPortableProgramFiles(); // Установка файлов рабочей директории в режиме Переносимого приложения
     }

     // Запоминается автоопределенный язык
     m_installAutodetectLang=installDialog.getAutoDetectLang();
 }
 else
 {
  exit(0); // Была нажата отмена
 }

 // Заново запускается поиск рабочей директории, на этот раз она должна быть найдена
 m_workDirectory="";
 findWorkDirectory();
}


// Автоопределение рабочей директории
bool GlobalParameters::findWorkDirectory(void)
{
 // Поиск файла conf.ini в той же директории, где находится бинарник

 // Нужно учесть, что программу могут запускать из другой директории
 // QDir::currentPath() - выдает директорию, где была выполнена команда запуска
 // mainProgramFile - содержит путь к бинарнику относительно директории запуска

 // Директория, где была выполнена команда запуска
 QFileInfo mainProgramFileInfo(m_mainProgramFile);
 QString fullCurrentPath=mainProgramFileInfo.absolutePath();

 qDebug() << "Check full current path " << fullCurrentPath;

 if(isMytetraIniConfig(fullCurrentPath+"/conf.ini")==true)
  {
   qDebug() << "Work directory set to path " << fullCurrentPath;

   // QDir dir=QDir("./");
   // QDir dir=QDir(QDir::currentPath());
   // workDirectory=dir.absolutePath();
   m_workDirectory=fullCurrentPath;
  }
 else
  {
   // Если в текущей директории запуска нет conf.ini

   // Поиск файла conf.ini в домашней директории пользователя
   // в поддиректории ".имя_программы"
   QString dir=QDir::homePath()+"/."+getApplicationName();

   qDebug() << "Detect home directory " << dir;

   // Если директория существует и в ней есть настоящий файл конфигурации
   if(isMytetraIniConfig(dir+"/conf.ini")==true)
    {
     qDebug() << "Config init file success find in home directory " << dir;
     m_workDirectory=dir;
    }
   else
    {
     // Иначе директории "~/.имя_программы" нет
     // и нужно пробовать найти данные в "~/.config/имя_программы"
     qDebug() << "File conf.ini can't' find in home directory " << dir;

     dir=QDir::homePath()+"/.config/"+getApplicationName();

     qDebug() << "Try find conf.ini in home subdirectory " << dir;

     // Если директория существует и в ней есть настоящий файл конфигурации
     if(isMytetraIniConfig(dir+"/conf.ini")==true)
      {
       qDebug() << "Config init file success find in home subdirectory " << dir;
       m_workDirectory=dir;
      }
     else
      qDebug() << "File conf.ini can't' find in home subdirectory " << dir;
    }
  }

 // Если рабочая директория не определена
 if(m_workDirectory.length()==0)
  {
   qDebug() << "Cant find work directory with mytetra data";
   return false;
  }
 else
  {
   // Иначе рабочая директория установлена
   qDebug() << "Set work directory to " << m_workDirectory;

   // Устанавливается эта директория как рабочая
   if(QDir::setCurrent(m_workDirectory))
    return true;
   else
    {
     criticalError("Can not set work directory as '"+m_workDirectory+"'. System problem.");
    }
  }
}


// Проверка ini-файла
bool GlobalParameters::isMytetraIniConfig(QString fileName)
{
 qDebug() << "Check config file " << fileName;

 QFileInfo info(fileName);

 // Если файл существует, надо определить, от MyTetra он или от другой программы
 if(info.exists())
  {
   qDebug() << "Config file " << fileName << " is exists";

   // Выясняется имя файла без пути к директории
   QString shortFileName=info.fileName();
   qDebug() << "Short config file name " << shortFileName;

   // Выясняется имя директории из имени файла
   QDir dir=info.dir();
   QString dirName=dir.absolutePath();
   qDebug() << "Config directory name " << dirName;

   // Открывается хранилище настроек
   // todo: Странность в Qt - если указать третьим параметром this в качестве
   // родителя, то считывание из файла конфигурации работать не будет. Разобраться
   // Вроде как это был баг одной из версий Qt, теперь считывание работает нормально
   QScopedPointer<QSettings> conf( new QSettings(fileName, QSettings::IniFormat, this) );
   conf->setIniCodec( QTextCodec::codecForName("UTF-8") );

   // Если есть переменная version
   if(conf->contains("version"))
    {
     int version=conf->value("version").toInt();

     // Если номер версии конфига до 3 включительно
     if(version<=3)
      {
       // В этих версиях не было переменной programm, поэтому проверяется
       // переменная tetradir
       if(conf->contains("tetradir")) return true;
       else return false;
      }
     else
      {
       // Иначе номер версии больше 3
       if(conf->contains("programm"))
        {
         if(conf->value("programm").toString()==FixedParameters::appTextId) return true;
         else return false;
        }
       else return false;
      }
    }
   else
    return false; // Нет переменной version
  }
 else
  return false; // Нет указанного файла
}


QString GlobalParameters::getWorkDirectory(void)
{
    return m_workDirectory;
}


QString GlobalParameters::getActionLogFileName(void)
{
    return m_workDirectory+"/actionLog.txt";
}


QString GlobalParameters::getActionLogPrevFileName(void)
{
    return m_workDirectory+"/actionLogPrev.txt";
}


QString GlobalParameters::getTargetOs(void)
{
#if TARGET_OS==ANY_OS
    return "any";
#endif

#if TARGET_OS==MEEGO_OS
    return "meego";
#endif

#if TARGET_OS==ANDROID_OS
    return "android";
#endif
}


// Имя программы в системе
// Используется для создания и поиска каталога с данными пользователя
QString GlobalParameters::getApplicationName(void)
{
    // todo: Подумать и заменить этот код на значения, полученные из PRO-файла
    QString appName="";

    if(getTargetOs()=="any")
        appName=FixedParameters::appTextId;

    if(getTargetOs()=="meego")
        appName="ru.webhamster."+FixedParameters::appTextId;

    if(getTargetOs()=="android")
        appName="ru.webhamster."+FixedParameters::appTextId;

    // qDebug() << "In getApplicationName() return \"" << appName << "\"";

    return appName;
}


void GlobalParameters::setTreeScreen(TreeScreen *point)
{
    m_pointTreeScreen=point;
}

TreeScreen *GlobalParameters::getTreeScreen()
{
    return m_pointTreeScreen;
}


void GlobalParameters::setRecordTableScreen(RecordTableScreen *point)
{
    m_pointRecordTableScreen=point;
}

RecordTableScreen *GlobalParameters::getRecordTableScreen()
{
    return m_pointRecordTableScreen;
}


void GlobalParameters::setFindScreen(FindScreen *point)
{
    m_pointFindScreen=point;
}

FindScreen *GlobalParameters::getFindScreen()
{
    return m_pointFindScreen;
}


void GlobalParameters::setMetaEditor(MetaEditor *point)
{
    m_pointMetaEditor=point;
}

MetaEditor *GlobalParameters::getMetaEditor()
{
    return m_pointMetaEditor;
}


void GlobalParameters::setStatusBar(QStatusBar *point)
{
    m_pointStatusBar=point;
}

QStatusBar *GlobalParameters::getStatusBar()
{
    return m_pointStatusBar;
}


void GlobalParameters::setWindowSwitcher(WindowSwitcher *point)
{
    m_windowSwitcher=point;
}


WindowSwitcher *GlobalParameters::getWindowSwitcher()
{
    return m_windowSwitcher;
}


void GlobalParameters::setSyncroCommandRunner(CommandRunner *point)
{
    m_syncroCommandRunner=point;
}


CommandRunner *GlobalParameters::getSyncroCommandRunnner()
{
    return m_syncroCommandRunner;
}


void GlobalParameters::setCryptKey(QByteArray hash)
{
    passwordHash=hash;
}


QByteArray GlobalParameters::getCryptKey(void)
{
    return passwordHash;
}


QString GlobalParameters::getInstallAutodetectLang()
{
    return m_installAutodetectLang;
}


