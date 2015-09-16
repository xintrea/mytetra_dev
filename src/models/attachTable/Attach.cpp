#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

#include "main.h"

#include "Attach.h"
#include "AttachTableData.h"
#include "models/recordTable/Record.h"
#include "libraries/crypt/CryptService.h"
#include "libraries/GlobalParameters.h"

extern GlobalParameters globalParameters;


Attach::Attach(AttachTableData *iParentTable)
{
  init(iParentTable);
}


Attach::Attach(QString iType, AttachTableData *iParentTable)
{
  if(iType!=typeFile && iType!=typeLink)
    critical_error("Incorrect attach type in Attach constructor: "+QString::number(iType));

  setField("type", iType);

  init(iParentTable);
}


Attach::~Attach()
{
  fileContent.clear();
}


void Attach::init(AttachTableData *iParentTable)
{
  liteFlag=true; // По-умолчанию легкий объект
  parentTable=iParentTable;
  fileContent.clear();
}


QStringList Attach::fieldAvailableList(void) const
{
  return QStringList() << "id" << "fileName" << "link" << "type" << "crypt";
}


// На вход метода подается тег <file>
void Attach::setupDataFromDom(QDomElement iDomElement)
{
  fieldsName=fieldAvailableList();
  foreach( QString fieldName, fieldsName ) // Перебираются имена полей (XML-тегов)
    fields[fieldName]=iDomElement.attribute(fieldName);
}


QDomElement Attach::exportDataToDom(QDomDocument *doc) const
{
  QDomElement elem=doc->createElement("file");

  fieldsName=fieldAvailableList();
  foreach( QString fieldName, fieldsName ) // Перебираются имена полей (XML-тегов)
    if( fields[fieldName].size()>0 )
      elem.setAttribute(fieldName, fields[fieldName]);

  return elem;
}


bool Attach::isEmpty() const
{
  // Заполненный аттач не может содержать пустой id
  if(fields.contains("id")==false || getField("id").length()==0)
    return true;
  else
    return false;
}


bool Attach::isLite() const
{
  return liteFlag;
}


void Attach::switchToLite()
{
  // Переключение возможно только из полновесного состояния
  if(liteFlag==true)
    critical_error("Can't switch attach to lite state. Attach id: "+getField("id")+" File name: "+getField("fileName"));

  fileContent.clear();

  liteFlag=true;
}


void Attach::switchToFat()
{
  // Переключение возможно только из легкого состояния
  if(liteFlag!=true)
    critical_error("Unavailable switching attach object to fat state. Attach Id: "+getField("id")+" File name: "+getField("fileName"));

  liteFlag=false;
}


void Attach::pushFatDataToDisk()
{
  if(getField("type")!="file")
    critical_error("Can't push fat data for non-file attach.");

  if(liteFlag==true)
    critical_error("Can't push fat data for lite attach. Attach id: "+getField("id")+" File name: "+getField("fileName"));

  QString innerFileName=getInnerFileName();
  QString innerDirName=parentTable->record->getFullDirName();

  QMap<QString, QByteArray> fileList;
  fileList[innerFileName]=fileContent;
  save_files_to_directory(innerDirName, fileList);
}


// То же самое что и pushFatDataToDisk, только в нужную директорию
void Attach::pushFatDataToDirectory(QString dirName)
{
  if(getField("type")!="file")
    critical_error("Can't save to directory "+dirName+" non-file attach.");

  if(liteFlag==true)
    critical_error("Can't save to directory lite attach. Attach id: "+getField("id")+" File name: "+getField("fileName"));

  QMap<QString, QByteArray> fileList;
  fileList[ getInnerFileName() ]=fileContent;
  save_files_to_directory(dirName, fileList);
}


// Втаскивание в объект содержимого файла с диска
void Attach::popFatDataFromDisk()
{
  // Втаскивание возможно только в полновесном состоянии
  if(liteFlag==true)
    critical_error("Can't' pop data for lite attach. Attach id: "+getField("id")+" File name: "+getField("fileName"));

  fileContent.clear();

  QString innerFileName=getInnerFileName();
  QString innerDirName=parentTable->record->getFullDirName();

  fileContent.append( (get_files_from_directory(innerDirName, innerFileName)).value(innerFileName) );
}


// fullFileName - имя файла с полным путем, который нужно скопировать в каталог записи
bool Attach::copyFileToBase(QString iFileName)
{
  qDebug() << "Before check file. iFileName: "+iFileName;
  qDebug() << "Inner file name: "+getFullInnerFileName();

  // Проверка наличия исходного файла
  QFile file(iFileName);

  qDebug() << "Check file finish";

  if(file.exists()==false)
  {
    showMessageBox(QObject::tr("Can't open file %1. File not exists.").arg(iFileName));
    return false;
  }

  qDebug() << "Before real copy file.";

  bool result=file.copy( getFullInnerFileName() );

  qDebug() << "After real copy file.";

  if(result==false)
    showMessageBox(QObject::tr("Can't copy file %1. May be directory %2 not writable, or target file %3 already exists.").arg(iFileName).arg(getFullInnerDirName()).arg(getFullInnerFileName()));

  return result;
}


// Удаление файла с диска
void Attach::removeFile()
{
  if(getField("type")!="file")
    return;

  // Проверка наличия файла
  QFile file( getFullInnerFileName() );

  if(file.exists()==false)
  {
    showMessageBox(QObject::tr("Can't delete file %1 on disk. File not exists.").arg( getFullInnerFileName() ));
    return;
  }

  file.setPermissions(QFile::ReadOther | QFile::WriteOther);
  file.remove();
}




// todo: подумать, код этого метода нужно использовать в методе setField()
// Короткое имя файла (т. е. без пути)
void Attach::setFileName(QString iFileName)
{
  if(getField("type")=="file")
    setField("fileName", iFileName);
  if(getField("type")=="link")
  {
    if(getField("fileName").length()>0 && iFileName.length()>0) // Если имя уже было задано (при создании аттача), и новое имя не пустое
    {
      showMessageBox(QObject::tr("Can't modify file name for link type attach."));
      return;
    }
    else
      setField("fileName", iFileName);
  }
}
bool Attach::setLink(QString iLink)
{
  if(type!=typeLink)
    critical_error("Can't set link to non-link attach.");

  QFile tempFile(iLink);

  // Если файла, на который ссылается линк, не существует
  if(!tempFile.exists())
  {
    QMessageBox msgBox;
    msgBox.setWindowTitle(QObject::tr("Warning!"));
    msgBox.setText( QObject::tr("Bad link. File not found.") );
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();

    return false;
  }
  else
  {
    link=iLink;
    return true;
  }
}
QString Attach::getLink() const
{
  if(type!=typeLink)
    critical_error("Can't get link from non-link attach.");

  return link;
}




// Внутрисистемное имя файла (без пути)
// Внутрисистемное имя складывается из идентификатора аттача и расширения, взятого от имени файла
QString Attach::getInnerFileName() const
{
  if(getField("type")=="file") // Для файла
  {
    // Выясняется расширение по видимому имени файла
    QFileInfo fileInfo( getField("fileName") );
    QString suffix=fileInfo.suffix();

    QString innerFileName=id+"."+suffix;

    return innerFileName;
  }

  if(getField("type")=="link") // Для линка просто возвращается имя файла, куда указывает линк
    return getField("fileName");

  critical_error("Bad attach type in getInnerFileName():"+getField("type"));

  return "";
}


// Внутрисистемное имя файла с путем
QString Attach::getFullInnerFileName() const
{
  if(getField("type")=="file") // Для файла
  {
    QString resultFileName=getFullInnerDirName()+"/"+getInnerFileName();
    return resultFileName;
  }

  if(getField("type")=="link") // Для линка
    return getLink();

  critical_error("Bad attach type in getFullInnerFileName():"+getField("type"));

  return "";
}


// Внутрисистемное имя файла с абсолютным путем
QString Attach::getAbsoluteInnerFileName() const
{
  QFileInfo fileInfo( getFullInnerFileName() );

  return fileInfo.absoluteFilePath();
}


// Внутрисистемный путь к файлу (полный)
QString Attach::getFullInnerDirName() const
{
  return parentTable->record->getFullDirName();
}


// Размер аттача в байтах
qint64 Attach::getFileSize() const
{
  QString tempFileName;

  if(type==typeFile)
  {
    QString recordDir=parentTable->record->getFullDirName();
    tempFileName=recordDir+"/"+getInnerFileName();
  }

  if(type==typeLink)
    tempFileName=link;

  QFile tempFile(tempFileName);

  if(!tempFile.exists())
    return 0;
  else
    return tempFile.size();
}


void Attach::encrypt()
{
  // Шифруется файл
  if(type==typeFile)
    CryptService::encryptFile(globalParameters.getCryptKey(), getFullInnerFileName());

  // Шифруется имя файла
  if(fileName.length()>0)
    fileName=CryptService::encryptString(globalParameters.getCryptKey(), fileName);

  // Шифруется линк на файл
  if(link.length()>0)
    link=CryptService::encryptString(globalParameters.getCryptKey(), link);

  // Шифруется содержимое файла в памяти, если таковое есть
  if(liteFlag==false)
    fileContent=CryptService::encryptByteArray(globalParameters.getCryptKey(), fileContent);
}


void Attach::decrypt()
{
  // Расшифровывается файл
  if(type==typeFile)
    CryptService::decryptFile(globalParameters.getCryptKey(), getFullInnerFileName());

  // Расшифровывается имя файла
  if(fileName.length()>0)
    fileName=CryptService::decryptString(globalParameters.getCryptKey(), fileName);

  // Расшифровывается линк на файл
  if(link.length()>0)
    link=CryptService::decryptString(globalParameters.getCryptKey(), link);

  // Расшифровывается содержимое файла в памяти, если таковое есть
  if(liteFlag==false)
    fileContent=CryptService::decryptByteArray(globalParameters.getCryptKey(), fileContent);
}
