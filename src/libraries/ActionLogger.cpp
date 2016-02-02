#include "ActionLogger.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


ActionLogger::ActionLogger()
{
  logFileName=globalParameters.getActionLogFileName();

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


