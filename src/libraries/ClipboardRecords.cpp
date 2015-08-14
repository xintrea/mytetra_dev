#include <QBoxLayout>
#include <QToolButton>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QMimeData>
#include <QtDebug>

#include "main.h"
#include "models/recordTable/Record.h"
#include "models/attachTable/AttachTableData.h"
#include "ClipboardRecords.h"


ClipboardRecords::ClipboardRecords(void) : QMimeData()
{
 init();
}


ClipboardRecords::~ClipboardRecords(void)
{
 
}


// Подготовка объекта для загрузки данных
void ClipboardRecords::init(void)
{
  if(records.table.size()>0)
    records.table.clear();
  
  clipbRecordsFormat.clear();
  clipbRecordsFormat << "mytetra/records";
}


// Очистка объекта
void ClipboardRecords::clear(void)
{
  init();
  clipbRecordsFormat << "";
}


void ClipboardRecords::addRecord(Record record)
{
  records.table << record;
}


// Печать информации о содержимом записи
void ClipboardRecords::print(void) const
{
  QListIterator< Record > list(records.table);
  
  // Перебор записей
  while (list.hasNext()) 
  {
    Record record=list.next();

    qDebug() << record.getText();

    // Перебор полей в записи
    QMap<QString, QString> fieldList=record.getFieldList();
    QMapIterator<QString, QString> currentField(fieldList);
    while(currentField.hasNext())
    {  
      currentField.next();
      qDebug() << currentField.key() << ": " << currentField.value();
    }

    // Перебор информации о праттаченных файлах в записи
    record.getAttachTable()->print();
  }
}


// Количество хранимых записей
int ClipboardRecords::getCount(void) const
{
 return records.table.size();
}


Record ClipboardRecords::getRecord(int n) const
{
  if(n<records.table.size())
   return records.table.at(n);
  else
   {
    critical_error("In ClipboardRecords::getRecord() unavailable number "+QString::number(n));
    return Record();
   }
}


// Получение текста записи с указанным номером
QString ClipboardRecords::getRecordText(int n) const
{
 if(n<records.table.size())
  return records.table.at(n).getText();
 else
  {
   critical_error("In ClipboardRecords::getRecordText() unavailable number "+QString::number(n));
   return QString();
  }
}


// Получение полей записи с указанным номером
QMap<QString, QString> ClipboardRecords::getRecordFieldList(int n) const
{
 if(n<records.table.size())
  return records.table.at(n).getFieldList();
 else
  {
   critical_error("In ClipboardRecords::getRecordFieldTable() unavailable number "+QString::number(n));
   return QMap<QString, QString>();
  } 
}


// Получение информации о приаттаченных файлах для записи с указанным номером
AttachTableData *ClipboardRecords::getRecordAttachTable(int n) const
{
  if(n<records.table.size())
    return getRecord(n).getAttachTable();
  else
  {
    critical_error("In ClipboardRecords::getRecordAttachTable() unavailable number "+QString::number(n));
    return NULL;
  }
}


// Получение файлов картинок
QMap<QString, QByteArray> ClipboardRecords::getRecordPictureFiles(int n) const
{
 if(n<records.table.size())
  return records.table.at(n).getPictureFiles();
 else
  {
   critical_error("In ClipboardRecords::getRecordPictureFiles() unavailable number "+QString::number(n));
   return QMap<QString, QByteArray>();
  }
}


// Получение приаттаченных файлов
/*
AttachTableData *ClipboardRecords::getRecordAttachFiles(int n) const
{
 if(n<records.table.size())
  return records.table.at(n).getAttachFiles();
 else
  {
   critical_error("In ClipboardRecords::getRecordAttachFiles() unavailable number "+QString::number(n));
  }
}
*/

QStringList ClipboardRecords::formats() const
{
 return clipbRecordsFormat;
}


QVariant ClipboardRecords::retrieveData(const QString &format,QVariant::Type preferredType) const
{
 Q_UNUSED(preferredType);

 if(format==clipbRecordsFormat[0])
  {
   QVariant v;
   v.setValue(records);
   return v;
  }

 return 0;
} 

