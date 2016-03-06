#include <QDateTime>

#include "main.h"
#include "ActionLogger.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


ActionLogger::ActionLogger(QObject *pobj)
{
  Q_UNUSED(pobj);

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


// Получение текстового описания действия
QString ActionLogger::getFullDescription(QMap<QString, QString> iData)
{
  QString line;

  // В зависимости от имени действия выбирается генерируемая строка описания действия
  if( iData["a"] == "startProgram" )
    line=tr("Program started");

  else if( iData["a"] == "stopProgram")
    line=tr("Program stop");

  else if( iData["a"] == "createRecord")
    line=tr("Create record \"%1\" with ID %2 in branch \"%3\" with ID %4").arg( iData["recordName"] ).
                                                                           arg( iData["recordId"] ).
                                                                           arg( iData["brachName"] ).
                                                                           arg( iData["branchId"] );

  else if( iData["a"] == "editRecord")
    line=tr("Edit record \"%1\" with ID %2").arg( iData["recordName"] ).
                                             arg( iData["recordId"]);

  else if( iData["a"] == "moveRecordUp")
    line=tr("Move up record \"%1\" with ID %2").arg( iData["recordName"] ).
                                                arg( iData["recordId"]);

  else if( iData["a"] == "moveRecordDown")
    line=tr("Move down record \"%1\" with ID %2").arg( iData["recordName"] ).
                                                  arg( iData["recordId"]);

  else if( iData["a"] == "deleteRecord")
    line=tr("Delete record \"%1\" with ID %2").arg( iData["recordName"] ).
                                               arg( iData["recordId"]);

  else if( iData["a"] == "copyRecordToBuffer")
    line=tr("Copy record \"%1\" with ID %2 to clipboard").arg( iData["recordName"] ).
                                                          arg( iData["recordId"]);

  else if( iData["a"] == "cutRecordToBuffer")
    line=tr("Cut record \"%1\" with ID %2 to clipboard").arg( iData["recordName"] ).
                                                          arg( iData["recordId"]);

  else if( iData["a"] == "pasteRecordFromBuffer") // Добавить, какой новый ID у вставленной записи получился
    line=tr("Paste record \"%1\" with ID %2 from clipboard").arg( iData["recordName"] ).
                                                             arg( iData["recordId"]);

  else if( iData["a"] == "startDragRecord")
    line=tr("Start drag record \"%1\" with ID %2 from branch \"%3\" with ID %4").arg( iData["recordName"] ).
                                                                                arg( iData["recordId"] ).
                                                                                arg( iData["brachName"] ).
                                                                                arg( iData["branchId"] );

  else if( iData["a"] == "dropRecord")
    line=tr("Drop record \"%1\" with ID %2 to branch \"%3\" with ID %4").arg( iData["recordName"] ).
                                                                         arg( iData["recordId"] ).
                                                                         arg( iData["brachName"] ).
                                                                         arg( iData["branchId"] );

  else
    line=tr("Unavailable action %1").arg( iData["a"] );

  return line;
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

