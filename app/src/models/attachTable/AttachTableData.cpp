#include <QDomElement>
#include <QDebug>
#include <QFileInfo>

#include "main.h"
#include "Attach.h"
#include "AttachTableData.h"
#include "AttachTableModel.h"
#include "models/recordTable/Record.h"


AttachTableData::AttachTableData(Record *iRecord)
{
  liteFlag=true;
  attachTable.clear();
  record=iRecord;
  relatedAttachTableModel=NULL;
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
  attachTable.clear();
  record=NULL;
  relatedAttachTableModel=NULL;
}


AttachTableData::~AttachTableData()
{

}


// На вход этого метода подается тег <files>
void AttachTableData::setupDataFromDom(QDomElement iDomElement)
{
  // Первый файл
  QDomElement currentFile=iDomElement.firstChildElement("file");

  // Перебор тегов <file ...>
  while(!currentFile.isNull())
  {
    Attach attach(this);

    // Аттач добавляется в таблицу приаттаченных файлов и размещается в памяти
    attachTable.append(attach);

    // После размещения в памяти инициализируется начальными данными
    attachTable.last().setupDataFromDom(currentFile);

    currentFile=currentFile.nextSiblingElement("file");
  }
}


QDomElement AttachTableData::exportDataToDom(QDomDocument *doc) const
{
  // Если у записи нет таблицы приаттаченных файлов
  if(attachTable.size()==0)
    return QDomElement();

  QDomElement attachTableDom=doc->createElement("files");

  // Пробегаются все приаттаченные файлы
  for(int i=0; i<attachTable.size(); i++)
    attachTableDom.appendChild( attachTable.at(i).exportDataToDom( doc ) ); // К элементу files прикрепляются элементы file

  return attachTableDom;
}


void AttachTableData::exportDataToStreamWriter(QXmlStreamWriter *xmlWriter) const
{
  // Если у записи нет таблицы приаттаченных файлов
  if(attachTable.size()==0)
    return;

  xmlWriter->writeStartElement("files");

  // Пробегаются все приаттаченные файлы
  for(int i=0; i<attachTable.size(); i++)
    attachTable.at(i).exportDataToStreamWriter( xmlWriter ); // К элементу files прикрепляются элементы file

  xmlWriter->writeEndElement(); // Закрылся files
}


void AttachTableData::setRelatedAttachTableModel(AttachTableModel *model)
{
  // Запоминание указателя на модель
  relatedAttachTableModel=model;

  // В модели устанавливается указатель на текущие данные (перекрестная ссылка)
  relatedAttachTableModel->setData(QModelIndex(), QVariant::fromValue(this), ATTACHTABLE_ROLE_TABLE_DATA_ONLY);
}


void AttachTableData::setRelatedAttachTableModelOnly(AttachTableModel *model)
{
  relatedAttachTableModel=model;
}


bool AttachTableData::isEmpty() const
{
  if(attachTable.size()==0)
    return true;
  else
    return false;
}


bool AttachTableData::isLite() const
{
  return liteFlag;
}


void AttachTableData::setRecord(Record *iRecord)
{
  record=iRecord; // Запоминается ссылка на запись, которой принадлежит данная таблица файлов
}


void AttachTableData::clear()
{
  attachTable.clear();
  record=NULL;
  liteFlag=true;
}


int AttachTableData::size() const
{
  return attachTable.size();
}





// Получение объекта аттача
Attach AttachTableData::getAttach(QString id)
{
  int row=getRowById(id);

  if(row<0)
    criticalError("Attach with ID: "+id+" not found");

  return attachTable.at(row);
}


// Добавление аттача
void AttachTableData::addAttach(Attach attach)
{
  if(relatedAttachTableModel!=NULL)
    relatedAttachTableModel->setData(QModelIndex(), QVariant(), ATTACHTABLE_COMMAND_BEGIN_RESET_MODEL);

  // Аттач добавляется в таблицу приаттаченных файлов
  attachTable.append(attach);

  if(relatedAttachTableModel!=NULL)
    relatedAttachTableModel->setData(QModelIndex(), QVariant(), ATTACHTABLE_COMMAND_END_RESET_MODEL);
}


// Изменение данных аттача
void AttachTableData::modifyAttach(QString id, Attach iAttach)
{
  int row=getRowById(id);

  if(row<0)
    return;

  attachTable[row]=iAttach;
}


// Удаление аттача
void AttachTableData::deleteAttach(QString id)
{
  int row=getRowById(id);

  if(row<0)
    return;

  // Сначала уничтожается файл
  attachTable[row].removeFile();

  // Если связанной модели нет
  if(relatedAttachTableModel==NULL)
  {
    attachTable.removeAt(row); // Просто удаляется запись в данных
    return;
  }
  else
  {
    // Иначе связанная модель есть

    relatedAttachTableModel->setData(QModelIndex(), row, ATTACHTABLE_COMMAND_BEGIN_REMOVE_ROW);
    attachTable.removeAt(row); // Удаляется запись в данных
    relatedAttachTableModel->setData(QModelIndex(), QVariant(), ATTACHTABLE_COMMAND_END_REMOVE_ROW);
  }
}


int AttachTableData::getRowById(QString id)
{
  for(int i=0; i<attachTable.size(); i++)
    if(attachTable.at(i).getField("id")==id)
      return i;

  return -1;
}


QString AttachTableData::getIdByRow(int row)
{
  return attachTable.at(row).getField("id");
}


// Видимое имя файла без пути
QString AttachTableData::getFileName(int row)
{
  return attachTable.at(row).getField("fileName");
}


// Видимое имя файла без пути по Id
QString AttachTableData::getFileNameById(QString id)
{
  int row=getRowById(id);

  if(row>=0)
    return getFileName(row);
  else
    return "";
}


// Внутреннее имя файла без пути
QString AttachTableData::getInnerFileName(int row)
{
  return attachTable.at(row).getInnerFileName();
}


// Внутреннее имя файла без пути по Id
QString AttachTableData::getInnerFileNameById(QString id)
{
  int row=getRowById(id);

  if(row>=0)
    return getInnerFileName(row);
  else
    return "";
}


// Список внутренних имен файлов без пути (файлы типа "link" будут содержать видимое имя линка)
QStringList AttachTableData::getInnerFileNameList()
{
  QStringList list;

  for(int i=0; i<attachTable.count(); ++i)
    list.append( attachTable.at(i).getInnerFileName() );

  return list;
}


// Список внутренних имен файлов без пути, хранимых на диске (файлы линков в этот список не входят)
QStringList AttachTableData::getInnerFileNameOnDiskList()
{
  QStringList list;

  for(int i=0; i<attachTable.count(); ++i)
    if( attachTable.at(i).getField("type")!="link" )
      list.append( attachTable.at(i).getInnerFileName() );

  return list;
}


// Внутреннее имя файла с путем
QString AttachTableData::getFullInnerFileName(int row)
{
  return attachTable.at(row).getFullInnerFileName();
}


// Внутреннее имя файла с путем по Id
QString AttachTableData::getFullInnerFileNameById(QString id)
{
  int row=getRowById(id);

  if(row>=0)
    return getFullInnerFileName(row);
  else
    return "";
}


// Внутреннее имя файла с абсолютным путем
QString AttachTableData::getAbsoluteInnerFileName(int row)
{
  return attachTable.at(row).getAbsoluteInnerFileName();
}


// Внутреннее имя файла с абсолютным путем по Id
QString AttachTableData::getAbsoluteInnerFileNameById(QString id)
{
  int row=getRowById(id);

  if(row>=0)
    return getAbsoluteInnerFileName(row);
  else
    return "";
}


qint64 AttachTableData::getFileSize(int row)
{
  return attachTable.at(row).getFileSize();
}


// Пачать содержимого таблицы конечных файлов
void AttachTableData::print()
{
  for(int i=0; i<attachTable.size(); ++i)
  {
    qDebug() << "File: " << attachTable.at(i).getField("id") << " Type: " << attachTable.at(i).getField("type");
  }
}


void AttachTableData::switchToLite()
{
  // Переключение возможно только из полновесного состояния
  if(liteFlag==true)
    criticalError("Can't switch attach table to lite state");

  for(int i=0; i<attachTable.size(); ++i)
  {
    // Тяжелые данные сохраняются на диск
    if(attachTable[i].getField("type")=="file")
      attachTable[i].pushFatDataToDisk();

    attachTable[i].switchToLite();
  }

  liteFlag=true;
}


void AttachTableData::switchToFat()
{
  // Переключение возможно только из легкого состояния
  if(liteFlag!=true)
    criticalError("Unavailable switching attach table to fat state");

  for(int i=0; i<attachTable.size(); ++i)
  {
    attachTable[i].switchToFat();

    // Тяжелые данные засасываются с диска в память
    if(attachTable[i].getField("type")=="file")
      attachTable[i].popFatDataFromDisk();
  }

  liteFlag=false;
}


bool AttachTableData::isRecordCrypt()
{
  if( record->getField("crypt")=="1" )
    return true;
  else
    return false;
}


void AttachTableData::encrypt(unsigned int area)
{
  for(int i=0; i<attachTable.size(); ++i)
    attachTable[i].encrypt(area);
}


void AttachTableData::decrypt(unsigned int area)
{
  for(int i=0; i<attachTable.size(); ++i)
    attachTable[i].decrypt(area);
}


void AttachTableData::saveAttachFilesToDirectory(QString dirName)
{
  for(int i=0; i<attachTable.size(); ++i)
    if(attachTable.at(i).getField("type")=="file") // Сохраняются только файлы, не линки
      if(!attachTable.at(i).isLite())
        attachTable[i].pushFatDataToDirectory(dirName);
}


// Обновление ссылок на таблицу аттачей внутри объектов-аттачей
void AttachTableData::updateAttachTableBackLink()
{
  for(int i=0; i<attachTable.size(); ++i)
    attachTable[i].setParentTable(this);
}
