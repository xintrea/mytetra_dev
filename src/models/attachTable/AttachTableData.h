#ifndef ATTACHTABLEDATA_H
#define ATTACHTABLEDATA_H

#include <QList>
#include <QMetaType>


class Record;
class Attach;
class QDomElement;

class AttachTableData
{
public:
  AttachTableData(Record *iRecord);
  virtual ~AttachTableData();

  void setupDataFromDom(QDomElement iDomElement);

  void setParentRecord(Record *iRecord);

  int size();

  QString getShortFileName(int row);
  QString getFullFileName(int row);
  qint64 getFileSize(int row);

  print(void);

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
