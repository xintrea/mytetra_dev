
#include "main.h"
#include "ActionLogger.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


ActionLogger::ActionLogger()
{
  logFileName=globalParameters.getActionLogFileName();
  logPrevFileName=globalParameters.getActionLogPrevFileName();

  logFile.setFileName(logFileName);
  bool result=logFile.open(QIODevice::Append | QIODevice::Text);

  if(!result)
    criticalError("Cant open log file "+logFileName);
}


ActionLogger::~ActionLogger()
{
  logFile.close();
}


void ActionLogger::addActionCreateRecord(QString recordId, QString recordName, QString branchId, QString branchName)
{

}


void ActionLogger::addActionEditRecord(QString recordId, QString recordName)
{

}


void ActionLogger::addActionDeleteRecord(QString recordId, QString recordName)
{

}


void ActionLogger::addActionMoveUpRecord(QString recordId, QString recordName)
{

}


void ActionLogger::addActionMoveDownRecord(QString recordId, QString recordName)
{

}


void ActionLogger::addActionCutRecord(QString recordId, QString recordName)
{

}


void ActionLogger::addActionPasteRecord(QString recordId, QString recordName)
{

}


