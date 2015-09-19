#ifndef __ATTACH_H__
#define __ATTACH_H__

#include <QString>
#include <QMap>
#include <QByteArray>
#include <QDomElement>


class AttachTableData;

class Attach
{
  friend class AttachTableData;

public:
  Attach(AttachTableData *iParentTable);
  Attach(int iType, AttachTableData *iParentTable);
  virtual ~Attach();

  void setupDataFromDom(QDomElement iDomElement);
  QDomElement exportDataToDom(QDomDocument *doc) const;

  QString getField(QString iFieldName);
  QString setField(QString name, QString value);

  // Работа с именем файла
  void setFileName(QString iFileName);
  QString getInnerFileName() const;
  QString getFullInnerFileName() const;
  QString getFullInnerDirName() const;
  QString getAbsoluteInnerFileName() const;

  bool setLink(QString iLink);

  qint64 getFileSize() const;

  // todo: подумать, может унаследовать Attach и Record от общего класса LiteFatObject
  bool isEmpty() const;
  bool isLite() const;
  void switchToLite();
  void switchToFat();

  void pushFatDataToDisk();
  void pushFatDataToDirectory(QString dirName);
  void popFatDataFromDisk();

  bool copyFileToBase(QString iFileName);
  void removeFile();

  void encrypt();
  void decrypt();


protected:

  void init(AttachTableData *iParentTable);

  QStringList fieldAvailableList(void) const;
  QStringList fieldCryptedList(void) const;
  QStringList typeAvailableList(void) const;

  bool liteFlag;

  AttachTableData *parentTable; // Указатель на таблицу приаттаченных файлов, которой принадлежит данный аттач

  QMap<QString, QString> fields;

  /*
  QString type; // Тип аттача ("file" или "link")
  QString id; // Идентификатор (служит так же техническим именем файла в базе, без расширения .bin)
  QString fileName; // Имя файла. Так файл отображается в интерфейсе, при экспорте назначается это имя
  QString link; // Линк на файл
  QString crypt; // Зашифрованный ли это аттач ("0" или "1")
  */

  QByteArray fileContent; // Содержимое файла, используется в режиме полных данных
};

#endif // __ATTACH_H__
