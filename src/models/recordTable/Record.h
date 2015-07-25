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

  // Основные текстовые данные открыты, чтобы с ними можно было работать напрямую,
  // чтобы не тратитить производительность на геттеры-сеттеры
  QMap<QString, QString> fieldList; // Перечень свойств записи (атрибутов)
  QMap<QString, QString> attachList; // Перечень прикрепляемых файлов

  QString getText() const;
  void setText(QString iText);

  QMap<QString, QByteArray> getPictureFiles() const;
  void setPictureFiles(QMap<QString, QByteArray> iPictureFiles);

  QMap<QString, QByteArray> getAttachFiles() const;
  void setAttachFiles(QMap<QString, QByteArray> iAttachFiles);

  bool isNull() const;
  bool isLite() const;
  void switchToLite();
  void switchToFat();

protected:

  bool liteFlag;

  // Установка содержимого свойств происходит в вышестоящем коде
  QString text; // Содержимое файла с текстом записи
  QMap<QString, QByteArray> pictureFiles; // Содержимое картинок, используемых в тексте записи (используется при переносе через буфер обмена, при DragAndDrop)
  QMap<QString, QByteArray> attachFiles; // Содержимое прикрепляемых файлов (используется при переносе через буфер обмена, при DragAndDrop)

  QString getIdAndNameAsString() const; // Внутренний метод для облегчения печати отладочной информации

};

#endif // __RECORD_H__
