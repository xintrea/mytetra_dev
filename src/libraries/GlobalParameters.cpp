#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QInputDialog>

#include "main.h"
#include "GlobalParameters.h"

#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "views/recordTable/RecordTableScreen.h"
#include "views/findInBaseScreen/FindScreen.h"
#include "libraries/WindowSwitcher.h"


GlobalParameters::GlobalParameters(QObject *pobj)
{
 Q_UNUSED(pobj);

}

GlobalParameters::~GlobalParameters()
{

}


void GlobalParameters::setMainProgramFile(QString file)
{
 mainProgramFile=file;
}


QString GlobalParameters::getMainProgramFile(void)
{
 return mainProgramFile;
}


void GlobalParameters::init(void)
{
 pointTreeScreen=NULL;
 pointRecordTableScreen=NULL;
 pointFindScreen=NULL;
 pointMetaEditor=NULL;
 pointStatusBar=NULL;
 windowSwitcher=NULL;

 initWorkDirectory(); // Инициализация рабочей директории
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
 QFileInfo mainProgramFileInfo(mainProgramFile);
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

 QString welcomeText=tr("Welcome to MyTetra v.")+QString::number(APPLICATION_RELEASE_VERSION)+'.'+QString::number(APPLICATION_RELEASE_SUBVERSION)+'.'+QString::number(APPLICATION_RELEASE_MICROVERSION)+"!";

 QString standartText=tr("Create subdirectory  \"%1\"\nin user directory  \"%2\",\nand create application files in it.").arg(dataDirName).arg(QDir::homePath());

 QString portableText=tr("Create application files\nin current directory  \"%1\".").arg(fullCurrentPath);

 // Если возможно создать только стандартную версию файлового окружения
 if(enablePortable==false)
  {
   qDebug() << "Cant create portable version - cant write data to mytetra bin-file directory";

   QString infoText=tr("The following actions will be performed before running this application: \n\n")+
                    standartText+"\n\n"+
                    tr("Do you agree to perform these?");

   QMessageBox msgBox;
   msgBox.setText(welcomeText);
   msgBox.setInformativeText(infoText);
   msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
   msgBox.setDefaultButton(QMessageBox::Ok);
   int ret = msgBox.exec();

   if(ret==QMessageBox::Ok)
    createStandartProgramFiles();
   else
    exit(0); // Была нажата отмена
  }
 else
  {
   // Иначе есть возможность создать как стандартное файловое окружение,
   // так и "переносимое"

   QString infoText=welcomeText+"\n\n"+
                    tr("Please, select application mode: \n\n")+
                    tr("Standart:\n")+standartText+"\n\n"+
                    tr("Portable:\n")+portableText+"\n\n";

   QStringList items;
   QString standartItem=tr("Standart");
   QString portableItem=tr("Portable");
   items << standartItem << portableItem;

   bool ok;

   QWidget *tempWidget=new QWidget();
   QString item=QInputDialog::getItem(tempWidget,
                                      welcomeText,
                                      infoText,
                                      items, 0, false, &ok);
   delete tempWidget;

   // Если пользователь сделал выбор
   if(ok && !item.isEmpty())
    {
     if(item==standartItem)
      createStandartProgramFiles();
     else
      createPortableProgramFiles();
    }
   else
    exit(0); // Была нажата отмена

  }

 // Заново запускается поиск рабочей директории
 workDirectory="";
 findWorkDirectory();
}


void GlobalParameters::createStandartProgramFiles(void)
{
 qDebug() << "Create standart program files";

 QDir userDir=QDir::home();
 QString dataDirName=".config/"+getApplicationName();
 if(userDir.mkpath(dataDirName))
  {
   qDebug() << "Successfull create subdirectory " << dataDirName << " in directory " << userDir.absolutePath();

   QString createFilePath=userDir.absolutePath()+"/"+dataDirName; // Ранее использовался QDir::homePath()

   createFirstProgramFiles(createFilePath);
  }
 else
  {
   criticalError("Can not created directory \""+dataDirName+"\" in user directory \""+QDir::homePath()+"\"");
   exit(0);
  }
}


void GlobalParameters::createPortableProgramFiles(void)
{
 qDebug() << "Create portable program files";

 // Путь к директории, где лежит бинарник
 QFileInfo mainProgramFileInfo(mainProgramFile);
 QString createFilePath=mainProgramFileInfo.absolutePath();

 createFirstProgramFiles(createFilePath);
}


// Создание первоначального набора файлов в указанной директории
void GlobalParameters::createFirstProgramFiles(QString dirName)
{
 qDebug() << "Create first program files in directory " << dirName;

 QDir dir(dirName);

 // Создается дерево директорий в указанной директории
 dir.mkpath("data/base/1300000000aaaaaaaaa2");
 dir.mkdir("trash");

 // Создаются файлы конфигурации
 QString targetOs=getTargetOs(); // "any" или "meego" или "android"

 QFile::copy(":/resource/standartconfig/"+targetOs+"/conf.ini", dirName+"/conf.ini");
 QFile::setPermissions(dirName+"/conf.ini", QFile::ReadUser | QFile::WriteUser);

 QFile::copy(":/resource/standartconfig/"+targetOs+"/editorconf.ini", dirName+"/editorconf.ini");
 QFile::setPermissions(dirName+"/editorconf.ini", QFile::ReadUser | QFile::WriteUser);

 createStyleSheetFile(dirName);

 // Создается файл базы данных
 QFile::copy(":/resource/standartdata/mytetra.xml", dirName+"/data/mytetra.xml");
 QFile::setPermissions(dirName+"/data/mytetra.xml", QFile::ReadUser | QFile::WriteUser);

 // Создается файл первой записи
 QFile::copy(":/resource/standartdata/base/1300000000aaaaaaaaa2/text.html", dirName+"/data/base/1300000000aaaaaaaaa2/text.html");
 QFile::setPermissions(dirName+"/data/base/1300000000aaaaaaaaa2/text.html", QFile::ReadUser | QFile::WriteUser);
}


void GlobalParameters::createStyleSheetFile(QString dirName)
{
  QString targetOs=getTargetOs();
  QFile::copy(":/resource/standartconfig/"+targetOs+"/stylesheet.css", dirName+"/stylesheet.css");
  QFile::setPermissions(dirName+"/stylesheet.css", QFile::ReadUser | QFile::WriteUser);
}


// Автоопределение рабочей директории
bool GlobalParameters::findWorkDirectory(void)
{
 // Поиск файла conf.ini в той же директории, где находится бинарник

 // Нужно учесть, что программу могут запускать из другой директории
 // QDir::currentPath() - выдает директорию, где была выполнена команда запуска
 // mainProgramFile - содержит путь к бинарнику относительно директории запуска

 // Директория, где была выполнена команда запуска
 QFileInfo mainProgramFileInfo(mainProgramFile);
 QString fullCurrentPath=mainProgramFileInfo.absolutePath();
 qDebug() << "Check full current path " << fullCurrentPath;

 if(isMytetraIniConfig(fullCurrentPath+"/conf.ini")==true)
  {
   qDebug() << "Work directory set to path " << fullCurrentPath;

   // QDir dir=QDir("./");
   // QDir dir=QDir(QDir::currentPath());
   // workDirectory=dir.absolutePath();
   workDirectory=fullCurrentPath;
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
     workDirectory=dir;
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
       workDirectory=dir;
      }
     else
      qDebug() << "File conf.ini can't' find in home subdirectory " << dir;
    }
  }

 // Если рабочая директория не определена
 if(workDirectory.length()==0)
  {
   qDebug() << "Cant find work directory with mytetra data";
   return false;
  }
 else
  {
   // Иначе рабочая директория установлена
   qDebug() << "Set work directory to " << workDirectory;

   // Устанавливается эта директория как рабочая
   if(QDir::setCurrent(workDirectory))
    return true;
   else
    {
     criticalError("Can not set work directory as '"+workDirectory+"'. System problem.");
     return false;
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
   QSettings *conf=new QSettings(fileName, QSettings::IniFormat, this);

   // Если есть переменная version
   if(conf->contains("version"))
    {
     int version=conf->value("version").toInt();

     // Если номер версии конфига до 3 включительно
     if(version<=3)
      {
       // В этих версиях небыло переменной programm, поэтому проверяется
       // переменная tetradir
       if(conf->contains("tetradir")) return true;
       else return false;
      }
     else
      {
       // Иначе номер версии больше 3
       if(conf->contains("programm"))
        {
         if(conf->value("programm").toString()=="mytetra") return true;
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
 return workDirectory;
}


QString GlobalParameters::getActionLogFileName(void)
{
 return workDirectory+"/actionLog.txt";
}


QString GlobalParameters::getActionLogPrevFileName(void)
{
 return workDirectory+"/actionLogPrev.txt";
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
  appName="mytetra";

 if(getTargetOs()=="meego")
  appName="ru.webhamster.mytetra";

 if(getTargetOs()=="android")
   appName="ru.webhamster.mytetra";

 // qDebug() << "In getApplicationName() return \"" << appName << "\"";

 return appName;
}


void GlobalParameters::setTreeScreen(TreeScreen *point)
{
 pointTreeScreen=point;
}

TreeScreen *GlobalParameters::getTreeScreen()
{
 return pointTreeScreen;
}


void GlobalParameters::setRecordTableScreen(RecordTableScreen *point)
{
 pointRecordTableScreen=point;
}

RecordTableScreen *GlobalParameters::getRecordTableScreen()
{
 return pointRecordTableScreen;
}


void GlobalParameters::setFindScreen(FindScreen *point)
{
 pointFindScreen=point;
}

FindScreen *GlobalParameters::getFindScreen()
{
 return pointFindScreen;
}


void GlobalParameters::setMetaEditor(MetaEditor *point)
{
 pointMetaEditor=point;
}

MetaEditor *GlobalParameters::getMetaEditor()
{
 return pointMetaEditor;
}


void GlobalParameters::setStatusBar(QStatusBar *point)
{
 pointStatusBar=point;
}

QStatusBar *GlobalParameters::getStatusBar()
{
 return pointStatusBar;
}


void GlobalParameters::setWindowSwitcher(WindowSwitcher *point)
{
 windowSwitcher=point;
}


WindowSwitcher *GlobalParameters::getWindowSwitcher()
{
 return windowSwitcher;
}


void GlobalParameters::setCryptKey(QByteArray hash)
{
 passwordHash=hash;
}


QByteArray GlobalParameters::getCryptKey(void)
{
 return passwordHash;
}
