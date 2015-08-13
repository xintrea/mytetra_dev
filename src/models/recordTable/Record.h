#ifndef __RECORD_H__
#define __RECORD_H__

#include <QString>
#include <QMap>
#include <QByteArray>
#include <QDomElement>


// Класс одной записи в таблице записей

// В полях записей с crypt=0 хранятся незашифрованные данные
// В полях записей с crypt=1 хранятся зашифрованные данные
// Это необходимо, чтобы дерево знаний генерировалось в/из XML быстро и без шифрации

class AttachTableData;

class Record
{

  // К закрытым функциям может иметь доступ объекты приаттаченного файла
  friend class Attach;
  friend class AttachTableData;

public:
  Record();
  virtual ~Record();

  void setupDataFromDom(QDomElement iDomElement);

  QString getText() const;
  QString getTextDirect();
  void setText(QString iText);

  QString getField(QString name) const;
  void setField(QString name, QString value);
  bool isFieldExists(QString name) const;

  // Установка и чтение данных без преобразований. Используется при генерации/чтении XML
  QString getFieldSource(QString name) const;
  void setFieldSource(QString name, QString value);

  QMap<QString, QString> getFieldList() const;

  QMap<QString, QString> getAttachList() const;
  void setAttachList(QMap<QString, QString> list);

  QMap<QString, QByteArray> getPictureFiles() const;
  void setPictureFiles(QMap<QString, QByteArray> iPictureFiles);

  AttachTableData *getAttachTable() const;
  void setAttachTable(AttachTableData *iAttachTable);

  bool isNull() const;
  bool isLite() const;
  void switchToLite();
  void switchToFat();

  // Шифрация
  void switchToEncryptAndSaveLite(void);
  void switchToEncryptAndSaveFat(void);

  // Расшифровка
  void switchToDecryptAndSaveLite(void);
  void switchToDecryptAndSaveFat(void);

  void pushFatAttributes();

protected:

  bool liteFlag;

  // Установка содержимого свойств происходит в вышестоящем коде

  // Легкие свойства
  QMap<QString, QString> fieldList; // Перечень свойств записи (атрибутов) ИмяАтрибута - Значение

  // Полновесные свойства
  QString text; // Содержимое файла с текстом записи
  QMap<QString, QByteArray> pictureFiles; // Содержимое картинок, используемых в тексте записи (используется при переносе через буфер обмена, при DragAndDrop)

  // Таблица прикрепляемых файлов
  AttachTableData *attachTable;

  void saveTextDirect(QString iText);

  QString getIdAndNameAsString() const; // Внутренний метод для облегчения печати отладочной информации

  QString getFullDirName() const;
  QString getShortDirName() const;

  QString getFullTextFileName() const;
  QString getFullFileName(QString fileName) const;

  void switchToEncryptFields(void);
  void switchToDecryptFields(void);

  void checkAndFillFileDir(QString &nameDirFull, QString &nameFileFull);
  void checkAndCreateTextFile();

};

#endif // __RECORD_H__
