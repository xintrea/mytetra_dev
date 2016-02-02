#ifndef ACTIONLOGGER_H
#define ACTIONLOGGER_H

#include <QString>


class ActionLogger
{
public:
  ActionLogger();

  void addActionCreateRecord(QString recordId, QString recordName, QString branchId, QString branchName);
  void addActionEditRecord(QString recordId, QString recordName);
  void addActionDeleteRecord(QString recordId, QString recordName);
  void addActionMoveUpRecord(QString recordId, QString recordName);
  void addActionMoveDownRecord(QString recordId, QString recordName);
  void addActionCutRecord(QString recordId, QString recordName);
  void addActionPasteRecord(QString recordId, QString recordName);

protected:

  QString logFileName;

};

#endif // ACTIONLOGGER_H
