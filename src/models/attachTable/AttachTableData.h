#ifndef ATTACHTABLEDATA_H
#define ATTACHTABLEDATA_H


class AttachTableData
{
public:
  AttachTableData(Record *iRecord);
  vitual ~AttachTableData();

  QString getShortFileName(row);
  QString getFullFileName(row);
  qint64 getFileSize(row);

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
