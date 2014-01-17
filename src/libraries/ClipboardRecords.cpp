#include <QBoxLayout>
#include <QToolButton>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QMimeData>
#include <QtDebug>

#include "main.h"
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


void ClipboardRecords::addRecord(QMap<QString, QString> fields, QMap<QString, QByteArray > files)
{
 CLIPB_ONE_RECORD_STRUCT record;

 record.fields = fields;
 record.files = files;

 records.table << record;
}


void ClipboardRecords::print(void) const
{
 QListIterator< CLIPB_ONE_RECORD_STRUCT > list(records.table);

 // Перебор записей
 while (list.hasNext()) 
  {
   CLIPB_ONE_RECORD_STRUCT record=list.next();

   QMap<QString, QString> record_fields=record.fields;

   // Перебор строк в записи
   QMapIterator<QString, QString> line(record_fields);
   while(line.hasNext()) 
    {  
     line.next();
     qDebug() << line.key() << ": " << line.value();
    }
  
  }
}


// Количество хранимых записей
int ClipboardRecords::getRecordsNum(void) const
{
 return records.table.size();
}


// Получение записи с указанным номером
QMap<QString, QString> ClipboardRecords::getRecordFields(int n) const
{
 if(n<records.table.size())
  return records.table.at(n).fields;
 else
  {
   critical_error("In clipboardrecords get_record_fields() unavailable number "+QString::number(n));
   return QMap<QString, QString>();
  } 
}


QMap<QString, QByteArray> ClipboardRecords::getRecordFiles(int n) const
{
 if(n<records.table.size())
  return records.table.at(n).files;
 else
  {
   critical_error("In clipboardrecords get_record_files() unavailable number "+QString::number(n));
   return QMap<QString, QByteArray>();
  }
}


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

