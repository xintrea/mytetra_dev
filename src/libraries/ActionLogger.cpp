#include <QDateTime>

#include "main.h"
#include "ActionLogger.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


ActionLogger::ActionLogger()
{
  // Текущий номер версии формата строк лога
  version=1;

  // Допустимые названия действий и допустимые имена атрибутов этих действий
  actionStructure["startProgram"];
  actionStructure["stopProgram"];

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
  // Проверка допустимости имени действия
  if(!actionStructure.contains(iName))
  {
    qWarning() << "Warning! Unavailable action name " << iName;
    return;
  }

  // Проверка допустимости наименований атрибутов действия
  QStringList unavailableAttName;
  foreach(const QString &attributeName, iData.keys()) // Пекребираются имена переданных атрибутов
    if(!actionStructure.value(iName).contains( attributeName )) // Если в имени атрибута нет в списке допустимых для данного действия
    {
      qWarning() << "Warning! Unavailable attribute name " << attributeName << " in action name " << iName;

      // Недопустимое имя атрибута запоминается (атрибут с недопстимым именем нельзя удалить прямо в этом цикле)
      unavailableAttName << attributeName;
    }

  // Атрибуты с недопустимым именем убираются из списка сохранения
  foreach (QString excludeAttName, unavailableAttName)
    iData.remove(excludeAttName);


  // Конструируется строка лога
  QString line="<r ";

  // Добавляется атрибут с версией формата
  line+="v=\"" + QString().setNum(version) +"\" ";

  // Добавляется атрибут со временем совершения данного действия (timestamp)
  uint timestamp=QDateTime::currentDateTime().toTime_t();
  line+="t=\"" + QString().setNum(timestamp) +"\" ";

  // Добавляется атрибут с именем действия
  line+="a=\"" + iName +"\" ";

  // Добавляются атрибуты действия
  foreach(const QString &attributeName, iData.keys()) // Перебираются имена переданных атрибутов
    line+=attributeName +"=\"" + htmlSpecialChars( iData.value(attributeName) ) +"\" ";

  // Завершается строка лога
  line+="/>";


  // Строка записывается в лог
  QTextStream out(&logFile);
  out << line << "\n";
}

