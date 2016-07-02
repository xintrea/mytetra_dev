#ifndef __ATTACH_H__
#define __ATTACH_H__

#include <QString>
#include <QMap>
#include <QByteArray>
#include <QDomElement>
#include <QXmlStreamWriter>


class AttachTableData;

class Attach
{
  friend class AttachTableData;

public:

  enum EncryptDecryptArea{areaMemory=0x1, areaFile=0x2, areaAll=0xFF};

  Attach(AttachTableData *iParentTable);
  Attach(QString iType, AttachTableData *iParentTable);
  virtual ~Attach();

  void setupDataFromDom(QDomElement iDomElement);
  QDomElement exportDataToDom(QDomDocument *doc) const;
  void exportDataToStreamWriter(QXmlStreamWriter *xmlWriter) const;

  QString getField(QString name) const;
  void setField(QString name, QString value);

  // Работа с именем файла
  static QString constructFileName(const QString type, const QString id, const QString fileName);
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

  void encrypt(unsigned int area=areaAll);
  void decrypt(unsigned int area=areaAll);

  // Расшифровка переданного DOM-элемента, полученного из тега <file> и его атрибутов
  static void decryptDomElement(QDomElement &iDomElement);

protected:

  void init(AttachTableData *iParentTable);
  void setParentTable(AttachTableData *iParentTable); // Защищенный метод, который может вызвать только этот класс и AttachTableData

  void setFieldSource(QString name, QString value);

  static QStringList fieldAvailableList(void);
  static QStringList fieldCryptedList(void);
  static QStringList typeAvailableList(void);

  bool liteFlag;

  AttachTableData *parentTable; // Указатель на таблицу приаттаченных файлов, которой принадлежит данный аттач

  QMap<QString, QString> fields;

  QByteArray fileContent; // Содержимое файла, используется в режиме полных данных
};

#endif // __ATTACH_H__
