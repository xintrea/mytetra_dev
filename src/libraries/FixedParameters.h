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
  QStringList recordFieldAvailableList;
  QStringList recordNaturalFieldAvailableList;
  QStringList recordCalculableFieldAvailableList;
  QStringList recordFieldCryptedList;

  // Поля для веток
  QStringList itemFieldAvailableList;
  QStringList itemFieldCryptedList;

  bool isRecordFieldAvailable(QString name) const;
  bool isRecordFieldNatural(QString name) const;
  bool isRecordFieldCalculable(QString name) const;

  QMap<QString, QString> recordFieldDescription(QStringList list) const;

  // Директория относительно XML-файла, в которой хранятся директории с иконками
  QString iconsRelatedDirectory;

signals:

public slots:

};

#endif // __FIXEDPARAMETERS_H__
