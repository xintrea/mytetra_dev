#ifndef __FIXEDPARAMETERS_H__
#define __FIXEDPARAMETERS_H__

#include <QObject>
#include <QMap>
#include <QStringList>

// Неизменяемые параметры, то есть параметры, которые заданы жестко в текущей версии MyTetra


class FixedParameters : public QObject
{
  Q_OBJECT

public:
  FixedParameters(QObject *parent=0);
  virtual ~FixedParameters();

  // Поля для записей
  const QStringList recordFieldAvailableList=       {"id", "name", "author", "url", "tags", "ctime", "dir", "file", "crypt", "block", "hasAttach", "attachCount"};
  const QStringList recordNaturalFieldAvailableList={"id", "name", "author", "url", "tags", "ctime", "dir", "file", "crypt", "block"};
  const QStringList recordCalculableFieldAvailableList={"hasAttach", "attachCount"};
  const QStringList recordFieldCryptedList={"name", "author", "url", "tags"};

  // Поля для веток
  const QStringList itemFieldAvailableList={"id", "name", "ctime", "crypt", "icon"};
  const QStringList itemFieldCryptedList={"name", "icon"};

  // Директория относительно XML-файла, в которой хранятся директории с иконками
  const QString iconsRelatedDirectory="icons";


  // Описание заголовка математического выражения в поле Description PNG-файла
  // Математическое выражение состоит из префикса и номера версии, выглядит следующим образом:
  // "mytetraMathExpression:v0001:математическое выражение в формате TeX"
  const QString mathExpDescriptionPrefix="mytetraMathExpression";
  const int mathExpVersion=1; // Текущая максимальная версия формата хранения формулы
  const int mathExpVersionNumberLen=4; // Сколько символов занимает номер версии (только цифры)
  const int mathExpHeaderLen=28; // Сколько символов занимает весь заголовок (префикс, номер версии, оба двоеточия)


  bool isRecordFieldAvailable(QString name) const;
  bool isRecordFieldNatural(QString name) const;
  bool isRecordFieldCalculable(QString name) const;

  QMap<QString, QString> recordFieldDescription(QStringList list) const;

signals:

public slots:

};

#endif // __FIXEDPARAMETERS_H__
