#include "AttachTableData.h"
#include "Attach.h"
#include "models/recordTable/Record.h"


AttachTableData::AttachTableData()
{
  attachTable=new QList< Attach >();
}


AttachTableData::~AttachTableData()
{
  delete attachTable;
}


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


// Имя файла с пути
QString AttachTableData::getFullFileName(int row)
{
  return "/catalog.../"+attachTable->at(row).getFileName(); // todo: Доделать
}


qint64 AttachTableData::getFileSize(int row)
{
  return attachTable->at(row).getFileSize();
}

