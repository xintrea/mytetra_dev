#ifndef ACTIONLOGGER_H
#define ACTIONLOGGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QMap>


class ActionLogger : public QObject
{
  Q_OBJECT

public:

  ActionLogger(QObject *pobj=0);
  ~ActionLogger();

  void addAction(QString iName, QMap<QString, QString> iData);
  QString getFullDescription(QMap<QString, QString> iData);

protected:

  int version; // Версия формата, используется в каждой строке

  // Допустимая структура имен действий и их параметров
  // Ключ - имя действия
  // Значение - список параметров
  QMap<QString, QStringList> actionStructure;

  QString logFileName;
  QString logPrevFileName;

  QFile logFile;

};

#endif // ACTIONLOGGER_H
