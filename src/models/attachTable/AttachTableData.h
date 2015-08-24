#ifndef ATTACHTABLEDATA_H
#define ATTACHTABLEDATA_H

#include <QList>
#include <QMetaType>
#include <QDomElement>

#include "models/attachTable/Attach.h"

class Record;
class AttachTableModel;

class AttachTableData
{
  friend class Attach;

public:
  AttachTableData(Record *iRecord);
  // AttachTableData(const AttachTableData &obj);
  AttachTableData();
  virtual ~AttachTableData();

  void setupDataFromDom(QDomElement iDomElement);
  QDomElement exportDataToDom(QDomDocument *doc) const;

  void setParentRecord(Record *iRecord);
  void setRelatedAttachTableModel(AttachTableModel *model);

  void clear();
  int size() const;

  void addAttach(Attach attach); // Добавление аттача в таблицу приаттаченных файлов

  QString getFileName(int row);
  QString getInnerFileName(int row);
  QString getFullInnerFileName(int row);
  QString getAbsoluteInnerFileName(int row);
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

  AttachTableModel *relatedAttachTableModel;
};

// Регистрация в QVariant типа AttachTableData
Q_DECLARE_METATYPE(AttachTableData);

// Регистрация в QVariant типа *AttachTableData
typedef AttachTableData *AttachTableDataPointer;
Q_DECLARE_METATYPE(AttachTableDataPointer);

#endif // ATTACHTABLEDATA_H
