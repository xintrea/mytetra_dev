#ifndef _CLIPBRECORDS_H_
#define _CLIPBRECORDS_H_

#include <QWidget>
#include <QMimeData>
#include <QMap>
#include <QList>


struct CLIPB_ONE_RECORD_STRUCT
{
 QMap<QString, QString> fields;
 QMap<QString, QByteArray> files;
};

 
// Определяется структура данных
// которая будет передаваться через буфер обмена
struct CLIPB_RECORDS_STRUCT
{
 // QList< QMap<QString, QString> > table;
 QList< CLIPB_ONE_RECORD_STRUCT > table;

};
Q_DECLARE_METATYPE(CLIPB_RECORDS_STRUCT);


class ClipboardRecords : public QMimeData
{
 Q_OBJECT
 
public:
 ClipboardRecords(void);
 ~ClipboardRecords(void);

 void init(void);
 void addRecord(QMap<QString, QString> fields, QMap<QString, QByteArray > files);
 void print(void) const;
 int getRecordsNum(void) const;
 QMap<QString, QString> getRecordFields(int n) const;
 QMap<QString, QByteArray > getRecordFiles(int n) const;
 
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
