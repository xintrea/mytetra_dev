#include <QDir>

#include "DiskHelper.h"
#include "main.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/TrashMonitoring.h"

extern AppConfig mytetraConfig;
extern TrashMonitoring trashMonitoring;


DiskHelper::DiskHelper()
{

}


// Удаление директории с копированием содержимого в корзину
void DiskHelper::removeDirectoryToTrash(QString nameDirFrom)
{
  QDir dirFrom(nameDirFrom);
  QStringList fileList=dirFrom.entryList();

  QString nameDirTo=mytetraConfig.get_trashdir();

  // Перебор всех файлов в удаляемой директории
  const int size = fileList.size();
  QString templNameDirFrom = nameDirFrom+"/";
  QString templNameDirTo = nameDirTo+"/";
  for(int i=0; i<size; ++i)
  {
    // Директории с именами "." и ".." обрабатывать не нужно
    if(fileList.at(i)=="." || fileList.at(i)=="..")continue;

    // Исходный файл, который будет перенесен в корзину
    QString fileNameFrom=templNameDirFrom+fileList.at(i);

    // Конечный файл, который должен лежать в корзине
    QString fileNameToShort;
    QString fileNameTo;
    bool targetFileFree=false;
    do {
      fileNameToShort=getUniqueId()+"_"+fileList.at(i);
      fileNameTo     =templNameDirTo+fileNameToShort;

      if(QFile::exists(fileNameTo)) targetFileFree=false;
      else targetFileFree=true;
    } while(!targetFileFree);

    qDebug() << "Move file from " << fileNameFrom << " to " << fileNameTo;

    // Перенос файла в корзину
    if( QFile::rename(fileNameFrom, fileNameTo)==true )
      trashMonitoring.addFile(fileNameToShort); // Оповещение что в корзину добавлен файл
    else
      criticalError("Can not remove file\n"+fileNameFrom+"\nto directory\n"+nameDirTo+"\nwith new name\n"+fileNameTo);
  }


  // Проверка наличия файлов внутри очищенной директории через новый временный объект директории
  // QDir tempDirFrom(nameDirFrom);
  // foreach(QString fileInDir, tempDirFrom.entryList())
  //   qDebug() << "File in empty dir with temp dir object: " << fileInDir;


  // Удаление директории
  // Если в rmdir() передать относительный путь, то будет удалена директория относительно директории бинарника.
  // Если в rmdir() передать асолютный путь, то будет удалена директория по абсолютному пути
  qDebug() << "Try delete directory " << nameDirFrom;
  if(!QDir().rmdir(nameDirFrom))
    qDebug() << "Directory " << nameDirFrom << " NOT deleted";
  else
    qDebug() << "Directory " << nameDirFrom << " delete succesfull";
}


// Удаление файла с копированием его копии в корзину
void DiskHelper::removeFileToTrash(QString fileNameFrom)
{
  // Получение короткого имени исходного файла
  QFileInfo fileInfo(fileNameFrom);
  QString fileNameFromShort=fileInfo.fileName();

  // Получение имени файла для сохранения в корзине
  QString fileNameToShort=getUniqueId()+"_"+fileNameFromShort;
  QString fileNameTo     =mytetraConfig.get_trashdir()+"/"+fileNameToShort;

  qDebug() << "Move file from " << fileNameFrom << " to " << fileNameTo;

  // Файл перемещается в корзину
  if( QFile::rename(fileNameFrom,fileNameTo)==true )
    trashMonitoring.addFile(fileNameToShort); // Оповещение что в корзину добавлен файл
  else
    criticalError("Can not remove file\n"+fileNameFrom+"\nto reserve file\n"+fileNameTo);
}


// Копирование файла в корзину
// Функция возвращает полное имя файла копии
QString DiskHelper::copyFileToTrash(QString fileNameFrom)
{
  // Получение короткого имени исходного файла
  QFileInfo fileInfo(fileNameFrom);
  QString fileNameFromShort=fileInfo.fileName();

  // Получение имени файла для сохранения в корзине
  QString fileNameToShort=getUniqueId()+"_"+fileNameFromShort;
  QString fileNameTo     =mytetraConfig.get_trashdir()+"/"+fileNameToShort;

  qDebug() << "Copy file from " << fileNameFrom << " to " << fileNameTo;

  // Файл копируется в корзину
  if( QFile::copy(fileNameFrom, fileNameTo)==true )
    trashMonitoring.addFile(fileNameToShort); // Оповещение что в корзину добавлен файл
  else
    criticalError("Can not remove file\n"+fileNameFrom+"\nto reserve file\n"+fileNameTo);

  QFileInfo fileInfoTo(fileNameTo);
  return fileInfoTo.absoluteFilePath();
}


// Создание временной директории
QString DiskHelper::createTempDirectory(void)
{
  QDir dir;
  QString systemTempDirName=dir.tempPath();

  QString temp_dir_name="mytetra"+getUniqueId();

  // Создается директория
  dir.setPath(systemTempDirName);
  dir.mkdir(temp_dir_name);

  QString createTempDirName;
  createTempDirName.reserve(systemTempDirName.length()+1+temp_dir_name.length());
  createTempDirName.append(systemTempDirName);
  createTempDirName.append("/");
  createTempDirName.append(temp_dir_name);

  qDebug() << "Create temporary directory "+createTempDirName;

  return createTempDirName;
}


// Удаление директории вместе со всеми поддиректориями и файлами
bool DiskHelper::removeDirectory(const QString &dirName)
{
  bool result = true;
  QDir dir(dirName);

  if (dir.exists(dirName))
  {
    Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
    {
      if (info.isDir())
        result = removeDirectory(info.absoluteFilePath());
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
bool DiskHelper::copyDirectory(const QString &fromName, const QString &toName)
{
  QDir fromDir(fromName);
  QDir toDir(toName);

  if(fromDir.exists() && toDir.exists())
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
QMap<QString, QByteArray> DiskHelper::getFilesFromDirectory(QString dirName, QString fileMask)
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
        criticalError("DiskHelper::getFilesFromDirectory() : File '"+info.absoluteFilePath()+"' open error");

      // Содержимое файла
      QByteArray b = f.readAll();

      // Содержимое файла сохраняется с ключем в виде имени файла
      result.insert(info.fileName(), b);
    }
  }
  else
    qDebug() << "DiskHelper::getFilesFromDirectory() : Can not find directory" << dirName;

  return result;
}


// Запись в директорию файлов, переданных в виде ассоциативного массива fileList
bool DiskHelper::saveFilesToDirectory(QString dirName, QMap<QString, QByteArray> fileList)
{
  qDebug() << "DiskHelper::saveFilesToDirectory() : Directory name " << dirName;

  QDir directory(dirName);

  // Если директория существует
  if(directory.exists())
  {
    foreach (QString filename, fileList.keys())
    {
      qDebug() << "DiskHelper::saveFilesToDirectory() : Save file " << filename;

      QFile file(dirName+"/"+filename);

      // Файл открывается для записи
      if(!file.open(QIODevice::WriteOnly))
      {
        qDebug() << "DiskHelper::saveFilesToDirectory() : Can not save file '" << filename << "' to directory '" << dirName << "'";
        return false;
      }

      // Данные сохраняются в файл
      file.write(fileList.value(filename));
    }

    return true;
  }
  else
  {
    qDebug() << "DiskHelper::saveFilesToDirectory() : Can not find directory" << dirName;
    return false;
  }
}


// Проверка директории, является ли она пустой
bool DiskHelper::isDirectoryEmpty(QString dirName)
{
  if(QDir(dirName).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() == 0)
    return true;
  else
    return false;
}

