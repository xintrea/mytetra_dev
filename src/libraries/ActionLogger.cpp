#include "main.h"
#include "ActionLogger.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


ActionLogger::ActionLogger()
{
  // Заполняются допустимые названия действий и допустимые имена атрибутов этих действий
  actionStructure["createRecord"]         =(QStringList() << "recordId" << "recordName" << "branchId" << "branchName");
  actionStructure["editRecord"]           =(QStringList() << "recordId" << "recordName" );
  actionStructure["moveRecordUp"]         =(QStringList() << "recordId" << "recordName" );
  actionStructure["moveRecordDown"]       =(QStringList() << "recordId" << "recordName" );
  actionStructure["deleteRecord"]         =(QStringList() << "recordId" << "recordName" );
  actionStructure["copyRecordToBuffer"]   =(QStringList() << "recordId" << "recordName" );
  actionStructure["cutRecordToBuffer"]    =(QStringList() << "recordId" << "recordName" );
  actionStructure["pasteRecordFromBuffer"]=(QStringList() << "recordId" << "recordName" );
  actionStructure["startDragRecord"]      =(QStringList() << "recordId" << "recordName" << "branchId" << "branchName" );
  actionStructure["dropRecord"]           =(QStringList() << "recordId" << "recordName" << "branchId" << "branchName" );

  actionStructure["createBranch"]  =(QStringList() << "branchId" << "branchName" << "parentBranchId" << "parentBranchName");
  actionStructure["editBranch"]    =(QStringList() << "branchId" << "branchName" );
  actionStructure["moveBranchUp"]  =(QStringList() << "branchId" << "branchName" );
  actionStructure["moveBranchDown"]=(QStringList() << "branchId" << "branchName" );
  actionStructure["deleteBranch"]  =(QStringList() << "branchId" << "branchName" );

  // Подготавливается лог-файл
  logFileName=globalParameters.getActionLogFileName();
  logPrevFileName=globalParameters.getActionLogPrevFileName();

  logFile.setFileName(logFileName); // Открывается файл лога
  bool result=logFile.open(QIODevice::Append | QIODevice::Text);
  if(!result)
    criticalError("Cant open log file "+logFileName);
}


ActionLogger::~ActionLogger()
{
  logFile.close();
}


QString ActionLogger::getFullDescription(QString iName, QMap<QString, QString> iData)
{

}


void ActionLogger::addAction(QString iName, QMap<QString, QString> iData)
{

}

