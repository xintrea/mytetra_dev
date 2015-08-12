#include <QFile>
#include <QMessageBox>

#include "main.h"

#include "Attach.h"
#include "AttachTableData.h"

Attach::Attach(int iType, AttachTableData *iParentTable)
{
  if(iType!=typeFile && iType!=typeLink)
    critical_error("Incorrect attach type in Attach constructor: "+QString::number(iType));

  liteFlag=true; // По-умолчанию легкий объект

  parentTable=iParentTable;
  type=iType;

  fileContent=NULL;
}


Attach::~Attach()
{
  if(fileContent!=NULL)
    fileContent->clear();

  delete *fileContent;
}


// На вход метода подается тег <file>
void Attach::setupDataFromDom(QDomElement iDomElement)
{
  type=iDomElement.attribute("type").toInt();
  id=iDomElement.attribute("id");
  fileName=iDomElement.attribute("fileName");
  link=iDomElement.attribute("link");
}


bool Attach::isNull() const
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
    critical_error("Can't switch attach to lite state. Attach id: "+id()+" File name: "+fileName);

  if(fileContent!=NULL)
    fileContent->clear();
  fileContent=NULL;

  liteFlag=true;
}


void Attach::switchToFat()
{
  // Переключение возможно только из легкого состояния
  if(liteFlag!=true)
    critical_error("Unavailable switching attach object to fat state. Attach Id: "+id+" File name: "+fileName);

  fileContent=new QByteArray();

  liteFlag=false;
}


void Attach::pushFatDataToDisk()
{

}


// Втаскивание в объект содержимого файла с диска
void Attach::popFatDataFromDisk()
{
  // Втаскивание возможно только в полновесном состоянии
  if(liteFlag==true)
    critical_error("Can't' pop data for lite attach. Attach id: "+id()+" File name: "+fileName);

  if(fileConten==NULL)
    critical_error("Incorrect initialise fat state for attach. Attach id: "+id()+" File name: "+fileName);

  fileConten->clean();

  QString innerFileName=id+".bin";
  QString innerDirName=getFullDirName();











  fileContent->append( (get_files_from_directory(directory, "*.bin")).value("") ;
}


int Attach::getType() const
{
  return type;
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
