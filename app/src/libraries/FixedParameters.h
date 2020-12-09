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
  FixedParameters(QObject *parent=nullptr);
  virtual ~FixedParameters();

  // Поля для записей
  static const QStringList recordFieldAvailableList;
  static const QStringList recordNaturalFieldAvailableList;
  static const QStringList recordCalculableFieldAvailableList;
  static const QStringList recordFieldCryptedList;

  // Поля для веток
  static const QStringList itemFieldAvailableList;
  static const QStringList itemFieldCryptedList;

  // Директория относительно XML-файла, в которой хранятся директории с иконками
  static const QString iconsRelatedDirectory;

  // Текстовый идентификатор приложения
  static const QString appTextId;

  // Описание заголовка математического выражения в поле Description PNG-файла
  // Математическое выражение состоит из текстового идентификатора приложения, типа хранимого значения
  // и номера версии формата хранимого значения, выглядит следующим образом:
  // "mytetra:mathExpression:v0001:математическое выражение в формате TeX"
  static const QString mathExpDescriptionType;
  static const int mathExpVersion; // Текущая максимальная версия формата хранения формулы
  static const int mathExpVersionNumberLen; // Сколько символов занимает номер версии (только цифры)
  static const int mathExpHeaderLen; // Сколько символов занимает весь заголовок (префикс, номер версии, три двоеточия)

  static bool isRecordFieldAvailable(QString name);
  static bool isRecordFieldNatural(QString name);
  static bool isRecordFieldCalculable(QString name);

  static QMap<QString, QString> recordFieldDescription(QStringList list);

};

#endif // __FIXEDPARAMETERS_H__
