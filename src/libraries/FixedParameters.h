#ifndef __FIXEDPARAMETERS_H__
#define __FIXEDPARAMETERS_H__

#include <QObject>
#include <QMap>

// Неизменяемые параметры, то есть параметры, которые заданы жестко в текущей версии MyTetra


class FixedParameters : public QObject
{
 Q_OBJECT

public:
 FixedParameters(QObject *parent=0);
 virtual ~FixedParameters();

 QStringList recordFieldAvailableList(void) const;
 bool isRecordFieldAvailable(QString name) const;
 QStringList recordFieldCryptedList(void) const;
 QMap<QString, QString> recordFieldDescription(QStringList list) const;

signals:

public slots:

};

#endif // __FIXEDPARAMETERS_H__
