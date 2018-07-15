#ifndef ATTACHTABLEDATA_H
#define ATTACHTABLEDATA_H

#include <QList>
#include <QMetaType>
#include <QDomElement>
#include <QXmlStreamWriter>

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
  void exportDataToStreamWriter(QXmlStreamWriter *xmlWriter) const;

  void setRecord(Record *iRecord);
  void setRelatedAttachTableModel(AttachTableModel *model);
  void setRelatedAttachTableModelOnly(AttachTableModel *model);

  void clear();
  int size() const;

  Attach getAttach(QString id); // Получение объекта аттача
  void addAttach(Attach attach); // Добавление аттача в таблицу приаттаченных файлов
  void modifyAttach(QString id, Attach iAttach); // Изменение данных аттача
  void deleteAttach(QString id); // Удаление аттача по идентификатору

  int getRowById(QString id);
  QString getIdByRow(int row);

  QString getFileName(int row);
  QString getFileNameById(QString id);

  QString getInnerFileName(int row);
  QString getInnerFileNameById(QString id);
  QStringList getInnerFileNameList();
  QStringList getInnerFileNameOnDiskList();

  QString getFullInnerFileName(int row);
  QString getFullInnerFileNameById(QString id);

  QString getAbsoluteInnerFileName(int row);
  QString getAbsoluteInnerFileNameById(QString id);

  qint64 getFileSize(int row);

  bool isEmpty() const;
  bool isLite() const;

  void switchToLite();
  void switchToFat();

  void print();

  bool isRecordCrypt();
  void encrypt(unsigned int area);
  void decrypt(unsigned int area);

  void saveAttachFilesToDirectory(QString dirName);

  // Обновление ссылок на таблицу аттачей внутри аттачей
  void updateAttachTableBackLink();

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
