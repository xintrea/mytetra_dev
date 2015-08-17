#include <QDomElement>
#include <QDebug>

#include "main.h"
#include "AttachTableData.h"
#include "Attach.h"
#include "models/recordTable/Record.h"


AttachTableData::AttachTableData(Record *iRecord)
{
  liteFlag=true;
  attachTable=new QList< Attach >();
  record=iRecord;
}


// Конструктор копирования
/*
AttachTableData::AttachTableData(const AttachTableData &obj)
{
  attachTable=new QList< Attach >(obj.attachTable); // "Глубокое" копирование таблицы прикрепляемых файлов
  record=obj.record; // А ссылка на запись просто копируется
}
*/


// Пустой конструктор, он требуется для Q_DECLARE_METATYPE в QMimeData
AttachTableData::AttachTableData()
{
  liteFlag=true;
  attachTable=new QList< Attach >();
  record=NULL;
}


AttachTableData::~AttachTableData()
{
  attachTable->clear();
  delete attachTable;
}


// На вход этого метода подается тег <files>
void AttachTableData::setupDataFromDom(QDomElement iDomElement)
{
  // Первый файл
  QDomElement currentFile=iDomElement.firstChildElement("files").firstChildElement("file");

  // Перебор тегов <file ...>
  while(!currentFile.isNull())
  {
    if(currentFile.tagName()=="file")
    {
      // Создается объект аттача
      Attach attach(this);
      attach.setupDataFromDom(currentFile);

      // Аттач добавляется в таблицу приаттаченных файлов
      attachTable->append(attach);
    }

    currentFile=currentFile.nextSiblingElement();
  }


}


bool AttachTableData::isEmpty() const
{
  if(attachTable==NULL)
    return true;
  else
    if(attachTable->size()==0)
      return true;
    else
      return false;
}


bool AttachTableData::isLite() const
{
  return liteFlag;
}


// todo: Подумать, может быть этот метод не нужен
void AttachTableData::setParentRecord(Record *iRecord)
{
  record=iRecord; // Запоминается ссылка на запись, которой принадлежит данная таблица файлов
}


int AttachTableData::size()
{
  return attachTable->size();
}


// Имя файла без пути
QString AttachTableData::getShortFileName(int row)
{
  return attachTable->at(row).getFileName();
}


// Имя файла с путем
QString AttachTableData::getFullFileName(int row)
{
  return "/catalog.../"+attachTable->at(row).getFileName(); // todo: Доделать
}


qint64 AttachTableData::getFileSize(int row)
{
  return attachTable->at(row).getFileSize();
}


// Пачать содержимого таблицы конечных файлов
void AttachTableData::print()
{
  for(int i=0; i<attachTable->size(); ++i)
  {
    qDebug() << "File: " << attachTable->at(i).getId() << " Type: " << attachTable->at(i).getType();
  }
}


void AttachTableData::switchToLite()
{
  // Переключение возможно только из полновесного состояния
  if(liteFlag==true)
    critical_error("Can't switch attach table to lite state");

  for(int i=0; i<attachTable->size(); ++i)
  {
    (*attachTable)[i].pushFatDataToDisk();
    (*attachTable)[i].switchToLite();
  }

  liteFlag=true;
}


void AttachTableData::switchToFat()
{
  // Переключение возможно только из легкого состояния
  if(liteFlag!=true)
    critical_error("Unavailable switching attach table to fat state");

  for(int i=0; i<attachTable->size(); ++i)
  {
    (*attachTable)[i].switchToFat();
    (*attachTable)[i].popFatDataFromDisk();
  }

  liteFlag=false;
}


void AttachTableData::encrypt()
{
  for(int i=0; i<attachTable->size(); ++i)
    (*attachTable)[i].encrypt();
}


void AttachTableData::decrypt()
{
  for(int i=0; i<attachTable->size(); ++i)
    (*attachTable)[i].decrypt();
}


void AttachTableData::saveAttachFilesToDirectory(QString dirName)
{
  for(int i=0; i<attachTable->size(); ++i)
    if(attachTable->at(i).getType()==Attach::typeFile) // Сохраняются только файлы, не линки
      if(!attachTable->at(i).isLite())
        (*attachTable)[i].pushFatDataToDirectory(dirName);
}
