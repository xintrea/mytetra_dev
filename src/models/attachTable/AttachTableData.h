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

  void setRecord(Record *iRecord);
  void setRelatedAttachTableModel(AttachTableModel *model);
  void setRelatedAttachTableModelOnly(AttachTableModel *model);

  int size() const;

  Attach getAttach(QString id) const; // Получение объекта аттача

  void clear(); // Очистка всей таблицы аттачей
  void addAttach(Attach attach); // Добавление аттача в таблицу приаттаченных файлов
  void modifyAttach(QString id, Attach iAttach); // Изменение данных аттача
  void deleteAttach(QString id); // Удаление аттача по идентификатору

  int getRowById(QString id) const;
  QString getIdByRow(int row) const;

  QString getFileName(int row) const;
  QString getFileNameById(QString id) const;

  QString getInnerFileName(int row) const;
  QString getInnerFileNameById(QString id) const;

  QString getFullInnerFileName(int row) const;
  QString getFullInnerFileNameById(QString id) const;

  QString getAbsoluteInnerFileName(int row) const;
  QString getAbsoluteInnerFileNameById(QString id) const;

  qint64 getFileSize(int row) const;

  bool isEmpty() const;
  bool isLite() const;

  void switchToLite();
  void switchToFat();

  void print() const;

  bool isRecordCrypt() const;
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
