#include <QDomElement>

#include "AttachTableData.h"
#include "Attach.h"
#include "models/recordTable/Record.h"


AttachTableData::AttachTableData(Record *iRecord)
{
  attachTable=new QList< Attach >();

  record=iRecord;
}


AttachTableData::~AttachTableData()
{
  delete attachTable;
}


// На вход этого метода подается тег <files>
void AttachTableData::setupDataFromDom(QDomElement iDomElement)
{
  // Первый файл
  QDomElement currentFile=iDomElement.firstChildElement("files").firstChildElement("file");

  // Если в таблице прикрепленых файлов есть хотя бы один первый файл
  if(currentFile!=NULL)
  {
    // Перебор тегов <file ...>
    while(!currentFile.isNull())
    {
      if(currentFile.tagName()=="file")
      {
        // Создается объект аттача
        Attach attach;
        attach.setupDataFromDom(currentFile);

        // Аттач добавляется в таблицу приаттаченных файлов
        attachTable->append(attach);
      }

      currentFile=currentFile.nextSiblingElement();
    }
  }

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
  for(int i=0; i<attachTable.size(); ++i)
  {
    qDebug() << "File: " << attachTable->at(i).getId() << " Type: " << attachTable->at(i).getType();
  }
}
