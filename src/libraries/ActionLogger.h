#ifndef ACTIONLOGGER_H
#define ACTIONLOGGER_H

#include <QString>
#include <QFile>

class ActionLogger
{
public:

  ActionLogger();
  virtual ~ActionLogger();

  void addAction(QString iName, QMap<QString, QString> iData);
  QString getFullDescription(QString iName, QMap<QString, QString> iData);

protected:

  // Допустимая структура имен действий и их параметров
  // Ключ - имя дествия
  // Значение - список параметров
  QMap<QString, QStringList> actionStructure;

  QString logFileName;
  QString logPrevFileName;

  QFile logFile;

};

#endif // ACTIONLOGGER_H
