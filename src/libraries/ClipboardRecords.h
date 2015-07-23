#ifndef _CLIPBRECORDS_H_
#define _CLIPBRECORDS_H_

#include <QWidget>
#include <QMimeData>
#include <QMap>
#include <QList>

 
// Определяется структура данных набора записей
// которая будет передаваться через буфер обмена
struct CLIPB_RECORDS_STRUCT
{
  QList< RecordData > table;
};
Q_DECLARE_METATYPE(CLIPB_RECORDS_STRUCT);


class ClipboardRecords : public QMimeData
{
  Q_OBJECT

public:
  ClipboardRecords(void);
  ~ClipboardRecords(void);

  void init(void);
  void clear(void);
  void addRecord(QString text,
                 QMap<QString, QString> fieldList,
                 QMap<QString, QString> attachList,
                 QMap<QString, QByteArray> pictureFiles,
                 QMap<QString, QByteArray> attachFiles);
  void print(void) const;
  int getCount(void) const;

  QString getRecordText(int n) const;
  QMap<QString, QString> getRecordFieldList(int n) const;
  QMap<QString, QString> getRecordAttachList(int n) const;
  QMap<QString, QByteArray > getRecordPictureFiles(int n) const;
  QMap<QString, QByteArray > getRecordAttachFiles(int n) const;

  // Этот метод QMimeData надо переопределить, так как он виртуальный
  QStringList formats() const;

protected:
  // Этот метод QMimeData надо переопределить, так как он виртуальный
  QVariant retrieveData(const QString &format,QVariant::Type preferredType) const;

private:
  CLIPB_RECORDS_STRUCT records; // Данные, которые передаются через буфер обмена
  QStringList clipbRecordsFormat; // Строка с идентификатором формата

};

#endif // _CLIPBRECORDS_H_
