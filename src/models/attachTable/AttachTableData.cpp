#include "AttachTableData.h"

AttachTableData::AttachTableData(Record *iRecord)
{
  attachTable=new QList< Attach >();

  record=iRecord; // Запоминается ссылка на запись, которой принадлежит данная таблица файлов
}


AttachTableData::~AttachTableData()
{
  delete attachTable;
}


// Имя файла без пути
QString AttachTableData::getShortFileName(row)
{
  return attachTable->at(row).getFileName();
}


// Имя файла с пути
QString AttachTableData::getFullFileName(row)
{
  return "/catalog.../"+attachTable->at(row).getFileName(); // todo: Доделать
}


qint64 AttachTableData::getFileSize(row)
{
  return attachTable->at(row).getFileSize();
}

