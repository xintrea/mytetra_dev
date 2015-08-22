#include <QFile>
#include <QMessageBox>

#include "main.h"

#include "Attach.h"
#include "AttachTableData.h"
#include "models/recordTable/Record.h"


Attach::Attach(AttachTableData *iParentTable)
{
  init(iParentTable);
}


Attach::Attach(int iType, AttachTableData *iParentTable)
{
  if(iType!=typeFile && iType!=typeLink)
    critical_error("Incorrect attach type in Attach constructor: "+QString::number(iType));

  type=iType;

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


QStringList Attach::fieldAvailableList(void)
{
  QStringList list;
  list << "id" << "fileName" << "link" << "type";

  return list;
}


// На вход метода подается тег <file>
void Attach::setupDataFromDom(QDomElement iDomElement)
{
  type=iDomElement.attribute("type").toInt();
  id=iDomElement.attribute("id");
  fileName=iDomElement.attribute("fileName");
  link=iDomElement.attribute("link");
}


QDomElement Attach::exportDataToDom(QDomDocument *doc) const
{
  QDomElement elem=doc->createElement("file");

  if(id.size()>0)
    elem.setAttribute("id", id);

  if(fileName.size()>0)
    elem.setAttribute("fileName", fileName);

  if(link.size()>0)
    elem.setAttribute("link", link);

  if(type!=Attach::typeFile && getType()!=Attach::typeLink)
    critical_error("Bad type value in Attach::exportDataToDom()");
  else
    elem.setAttribute("type", getTypeAsName());

  return elem;
}


bool Attach::isEmpty() const
{
  // Заполненный аттач не может содержать пустой id
  if(id.length()==0)
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
    critical_error("Can't switch attach to lite state. Attach id: "+id+" File name: "+fileName);

  fileContent.clear();

  liteFlag=true;
}


void Attach::switchToFat()
{
  // Переключение возможно только из легкого состояния
  if(liteFlag!=true)
    critical_error("Unavailable switching attach object to fat state. Attach Id: "+id+" File name: "+fileName);

  liteFlag=false;
}


void Attach::pushFatDataToDisk()
{
  if(type!=typeFile)
    critical_error("Can't push fat data for non-file attach.");

  if(liteFlag==true)
    critical_error("Can't push fat data for lite attach. Attach id: "+id+" File name: "+fileName);

  QString innerFileName=id+".bin";
  QString innerDirName=parentTable->record->getFullDirName();

  QMap<QString, QByteArray> fileList;
  fileList[innerFileName]=fileContent;
  save_files_to_directory(innerDirName, fileList);
}


// То же самое что и pushFatDataToDisk, только в нужную директорию
void Attach::pushFatDataToDirectory(QString dirName)
{
  if(type!=typeFile)
    critical_error("Can't save to directory "+dirName+" non-file attach.");

  if(liteFlag==true)
    critical_error("Can't save to directory lite attach. Attach id: "+id+" File name: "+fileName);

  QMap<QString, QByteArray> fileList;
  fileList[id+".bin"]=fileContent;
  save_files_to_directory(dirName, fileList);
}


// Втаскивание в объект содержимого файла с диска
void Attach::popFatDataFromDisk()
{
  // Втаскивание возможно только в полновесном состоянии
  if(liteFlag==true)
    critical_error("Can't' pop data for lite attach. Attach id: "+id+" File name: "+fileName);

  fileContent.clear();

  QString innerFileName=id+".bin";
  QString innerDirName=parentTable->record->getFullDirName();

  fileContent.append( (get_files_from_directory(innerDirName, innerFileName)).value(innerFileName) );
}


// fullFileName - имя файла с полным путем, который нужно скопировать в каталог записи
// id - идентификатор аттача. Файл должен сохраниться в каталоге записи с именем <id>.bin
bool Attach::copyFileToBase(QString iFileName, QString id)
{
  QString recordDir=parentTable->record->getFullDirName();
  QString resultFileName=recordDir+"/"+id+".bin";

  QFile file(iFileName);

  if(file.exists()==false)
  {
    QMessageBox msgBox;
    msgBox.setText(QObject::tr("Can't open file %1. File not exists.").arg(iFileName));
    msgBox.exec();

    return false;
  }

  bool result=file.copy(resultFileName);

  if(result==false)
  {
    QMessageBox msgBox;
    msgBox.setText(QObject::tr("Can't copy file %1. May be directory %2 not writable, or target file %3 already exists.").arg(iFileName).arg(recordDir).arg(resultFileName));
    msgBox.exec();
  }

  return result;
}


int Attach::getType() const
{
  return type;
}

QString Attach::getTypeAsName() const
{
  if(type==Attach::typeFile)
    return "file";

  if(type==Attach::typeLink)
    return "link";

  return "";
}


void Attach::setId(QString iId)
{
  id=iId;
}


QString Attach::getId() const
{
  return id;
}


// Короткое имя файла (т. е. без пути)
void Attach::setFileName(QString iFileName)
{
  if(type!=typeFile)
    critical_error("Can't set file name to non-file attach.");

  fileName=iFileName;
}


QString Attach::getFileName() const
{
  if(type!=typeFile)
    critical_error("Can't get file name from non-file attach.");

  return fileName;
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


// Размер аттача в байтах
qint64 Attach::getFileSize() const
{
  QString tempFileName;

  if(type==typeFile)
    tempFileName=fileName; // todo: Добавить путь к файлу

  if(type==typeLink)
    tempFileName=link;

  QFile tempFile(tempFileName);

  if(!tempFile.exists())
    return 0;
  else
    return tempFile.size();
}


// todo: дописать
void Attach::encrypt()
{

}


// todo: дописать
void Attach::decrypt()
{

}
