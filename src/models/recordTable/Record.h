#ifndef __RECORD_H__
#define __RECORD_H__

#include <QString>
#include <QMap>
#include <QByteArray>


// Класс одной записи в таблице записей

// Структура данных одной записи без содержимого файлов картинок и приаттаченных файлов
// Нужна для особых случаев для ускорения работы
/*
struct RecordExemplar
{
  QString text;
  QMap<QString, QString> fieldList; // Хранятся пары: Имя поля (атрибута XML) - Значение
  QMap<QString, QString> attachList; // Хранятся пары: id - Имя файла
};
*/


class Record
{
public:
  Record();
  virtual ~Record();

  QString getText() const;
  QString getTextDirect() const;
  void setText(QString iText);

  QString getField(QString name) const;
  void setField(QString name, QString value);

  QMap<QString, QString> getFieldList() const;

  QMap<QString, QString> getAttachList() const;
  void setAttachList(QMap<QString, QString> list);

  QMap<QString, QByteArray> getPictureFiles() const;
  void setPictureFiles(QMap<QString, QByteArray> iPictureFiles);

  QMap<QString, QByteArray> getAttachFiles() const;
  void setAttachFiles(QMap<QString, QByteArray> iAttachFiles);

  bool isNull() const;
  bool isLite() const;
  void switchToLite();
  void switchToFat();

  void switchToEncryptLite(void);
  void switchToEncryptFat(void);

  void pushFatAttributes();

protected:

  bool liteFlag;

  // Установка содержимого свойств происходит в вышестоящем коде

  // Легкие свойства
  QMap<QString, QString> fieldList; // Перечень свойств записи (атрибутов)
  QMap<QString, QString> attachList; // Перечень прикрепляемых файлов

  // Полновесные свойства
  QString text; // Содержимое файла с текстом записи
  QMap<QString, QByteArray> pictureFiles; // Содержимое картинок, используемых в тексте записи (используется при переносе через буфер обмена, при DragAndDrop)
  QMap<QString, QByteArray> attachFiles; // Содержимое прикрепляемых файлов (используется при переносе через буфер обмена, при DragAndDrop)

  void saveText(QString iText);

  QString getIdAndNameAsString() const; // Внутренний метод для облегчения печати отладочной информации

  QString getFullDirName() const;
  QString getFullTextFileName() const;
  QString getFullFileName(QString fileName) const;

  void switchToEncryptFields(void);

  void checkAndFillFileDir(QString &nameDirFull, QString &nameFileFull);
  void checkAndCreateTextFile();

};

#endif // __RECORD_H__
