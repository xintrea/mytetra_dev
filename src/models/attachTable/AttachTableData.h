#ifndef ATTACHTABLEDATA_H
#define ATTACHTABLEDATA_H

#include <QList>
#include <QMetaType>
#include <QDomElement>

#include "models/attachTable/Attach.h"

class Record;

class AttachTableData
{
  friend class Attach;

public:
  AttachTableData(Record *iRecord);
  // AttachTableData(const AttachTableData &obj);
  AttachTableData();
  virtual ~AttachTableData();

  void setupDataFromDom(QDomElement iDomElement);

  void setParentRecord(Record *iRecord);

  void clear();
  int size();

  QString getShortFileName(int row);
  QString getFullFileName(int row);
  qint64 getFileSize(int row);

  bool isEmpty() const;
  bool isLite() const;

  void switchToLite();
  void switchToFat();

  void print();

  void encrypt();
  void decrypt();

  void saveAttachFilesToDirectory(QString dirName);

protected:

  bool liteFlag;

  // Перечень файлов и их свойств
  QList< Attach > attachTable;

  // Какой записи принадлежит таблица файлов
  Record *record;
};

// Регистрация в QVariant типа AttachTableData
Q_DECLARE_METATYPE(AttachTableData);

// Регистрация в QVariant типа *AttachTableData
typedef AttachTableData *AttachTableDataPointer;
Q_DECLARE_METATYPE(AttachTableDataPointer);

#endif // ATTACHTABLEDATA_H
