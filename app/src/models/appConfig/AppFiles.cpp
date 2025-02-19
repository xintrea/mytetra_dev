#include <QFile>
#include <QDebug>
#include <QDir>

#include "AppFiles.h"
#include "libraries/helpers/DebugHelper.h"
#include "libraries/helpers/DiskHelper.h"


AppFiles::AppFiles()
{

}


void AppFiles::createStandartProgramFiles(void)
{
    qDebug() << "Create standard program files";

    QDir userDir=QDir::home();
    QString dataDirName=".config/"+getApplicationName();

    // Если директория либо успешно создалась, либо уже существовала (проверяется одним вызовом mkpath)
    if( userDir.mkpath(dataDirName) )
    {
        qDebug() << QString("Successfull create subdirectory %1 in directory %2 or directory already exists")
                    .arg(dataDirName)
                    .arg(userDir.absolutePath());

        QString path=userDir.absolutePath()+"/"+dataDirName; // Ранее использовался QDir::homePath()

        this->createFirstAppFiles(path);
    }
    else
    {
        criticalError("It is not possible to create a directory \""+dataDirName+"\" in user directory \""+QDir::homePath()+"\"");
    }
}


void AppFiles::createPortableProgramFiles(void)
{
    qDebug() << "Create portable program files";

    // Путь к директории, где лежит бинарник
    QFileInfo mainProgramFileInfo(m_mainProgramFile);
    QString path=mainProgramFileInfo.absolutePath();

    this->createFirstAppFiles(path);
}


// Создание первоначального набора файлов в указанной директории
void AppFiles::createFirstAppFiles(QString dirName,
                                   unsigned int flags)
{
    qDebug() << "Create first program files in directory " << dirName;

    // Создание рабочей директории если ее еще нет
    QDir dir(dirName);
    if (!dir.exists())
    {
        QDir createDir;
        bool result = createDir.mkpath(dirName);
        if ( !result ) {
            qDebug() << "Failed to create directory " << dirName;
        }
    }

    // Создаются файлы конфигурации
    if ( flags & CreateFirstAppFilesFlags::APP_CONFIG )
    {
        QString targetOs=getTargetOs(); // "any" или "meego" или "android"

        QFile::copy(":/resource/standartconfig/"+targetOs+"/conf.ini", dirName+"/conf.ini");
        QFile::setPermissions(dirName+"/conf.ini", QFile::ReadUser | QFile::WriteUser);

        QFile::copy(":/resource/standartconfig/"+targetOs+"/editorconf.ini", dirName+"/editorconf.ini");
        QFile::setPermissions(dirName+"/editorconf.ini", QFile::ReadUser | QFile::WriteUser);

        this->createThemesFiles(dirName);
    }

    // Создаются файлы БД
    if ( flags & CreateFirstAppFilesFlags::DB )
    {
        // Создается дерево директорий в указанной директории
        dir.mkpath("data/base/1300000000aaaaaaaaa2");

        // Создается файл базы данных
        QFile::copy(":/resource/standartdata/mytetra.xml", dirName+"/data/mytetra.xml");
        QFile::setPermissions(dirName+"/data/mytetra.xml", QFile::ReadUser | QFile::WriteUser);

        // Создается файл первой записи
        QFile::copy(":/resource/standartdata/base/1300000000aaaaaaaaa2/text.html", dirName+"/data/base/1300000000aaaaaaaaa2/text.html");
        QFile::setPermissions(dirName+"/data/base/1300000000aaaaaaaaa2/text.html", QFile::ReadUser | QFile::WriteUser);
    }

    // Создается каталог корзины
    if ( flags & CreateFirstAppFilesFlags::TRASH )
    {
        dir.mkdir("trash");
    }

    // Синхронизация файловой системы, почему-то после создания файлы
    // не всегда доступны на Linux. Под windows такой утилиты нет в стандартной поставке
#ifdef Q_OS_LINUX
    std::system("sync");
#endif
}


void AppFiles::createThemesFiles(QString dirName, QString themeName)
{
  QString targetOs=getTargetOs();
  if (targetOs == "any" && !themeName.isNull() && !themeName.isEmpty())
  {
    QString fromDir=":/resource/standartconfig/any/styles/"+themeName;
    QString toDir=dirName+"/style";
    DiskHelper::removeDirectory(toDir);
    DiskHelper::copyDirectoryRecursively(fromDir, toDir, QFile::ReadUser | QFile::WriteUser);
  }
  else
  {
    QDir styleDir(dirName);
    styleDir.mkdir("style");
    QFile::copy(":/resource/standartconfig/"+targetOs+"/stylesheet.css", dirName+"/style/stylesheet.css");
    QFile::setPermissions(dirName+"/style/stylesheet.css", QFile::ReadUser | QFile::WriteUser);
  }
}

