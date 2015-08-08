#ifndef ATTACHTABLEDATA_H
#define ATTACHTABLEDATA_H

#include <QList>
#include <QMetaType>

#include "Attach.h"
#include "models/recordTable/Record.h"

// class Record;
// class Attach;

class AttachTableData
{
public:
  AttachTableData();
  virtual ~AttachTableData();

  void setParentRecord(Record *iRecord);

  int size();

  QString getShortFileName(int row);
  QString getFullFileName(int row);
  qint64 getFileSize(int row);

protected:

  // Перечень файлов и их свойств
  QList< Attach > *attachTable;

  // Какой записи принадлежит таблица файлов
  Record *record;
};

// Регистрация в QVariant типа AttachTableData
Q_DECLARE_METATYPE(AttachTableData);

// Регистрация в QVariant типа *AttachTableData
typedef AttachTableData *AttachTableDataPointer;
Q_DECLARE_METATYPE(AttachTableDataPointer);

#endif // ATTACHTABLEDATA_H
