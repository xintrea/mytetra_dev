#ifndef __ATTACH_H__
#define __ATTACH_H__

#include <QString>
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

  // Возможные типы аттача - файл или линк на файл
  enum attachType{typeFile, typeLink};

  void setupDataFromDom(QDomElement iDomElement);
  QDomElement exportDataToDom(QDomDocument *doc) const;

  int getType() const;
  QString getTypeAsName() const;

  void setId(QString iId);
  QString getId() const;

  // Работа с именем файла
  void setFileName(QString iFileName);
  QString getFileName() const;
  QString getInnerFileName() const;
  QString getFullInnerFileName() const;
  QString getFullInnerDirName() const;
  QString getAbsoluteInnerFileName() const;

  bool setLink(QString iLink);
  QString getLink() const;

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

  QStringList fieldAvailableList(void);

  bool liteFlag;

  AttachTableData *parentTable; // Указатель на таблицу приаттаченных файлов, которой принадлежит данный аттач
  int     type; // Тип аттача (файл или линк на файл)

  QString id; // Идентификатор (служит так же техническим именем файла в базе, без расширения .bin)
  QString fileName; // Имя файла. Так файл отображается в интерфейсе, при экспорте назначается это имя
  QString link; // Линк на файл

  QByteArray fileContent; // Содержимое файла, используется в режиме полных данных
};

#endif // __ATTACH_H__
