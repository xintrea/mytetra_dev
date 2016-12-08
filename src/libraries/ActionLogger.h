#ifndef ACTIONLOGGER_H
#define ACTIONLOGGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QMap>
#include <QDomElement>


class ActionLogger : public QObject
{
  Q_OBJECT

public:

  ActionLogger(QObject *pobj=0);
  ~ActionLogger();

  void init();
  void setEnableLogging(bool flag);
  QString getXml();
  void addAction(QString iName, QMap<QString, QString> iData = QMap<QString, QString>());

  QString getFullDescription(QDomElement element);
  QString getFullDescription(QMap<QString, QString> iData);

protected:

  int version; // Версия формата, используется в каждой строке

  // Допустимая структура имен действий и их параметров
  // Ключ - имя действия
  // Значение - список параметров
  QMap<QString, QStringList> actionStructure;

  bool enableLogging;

  QString logFileName;
  QString logPrevFileName;

  QFile logFile;

  void openLogFileForWrite();
  void openLogFileForRead();
  void closeLogFile();

};

#endif // ACTIONLOGGER_H
