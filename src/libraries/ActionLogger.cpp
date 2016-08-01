#include <QDateTime>

#include "main.h"
#include "ActionLogger.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "libraries/crypt/CryptService.h"

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
  actionStructure["createCryptRecord"]    =(QStringList() << "recordId" << "recordName" << "branchId" << "branchName");

  actionStructure["editRecord"]           =(QStringList() << "recordId" << "recordName" );
  actionStructure["editCryptRecord"]      =(QStringList() << "recordId" << "recordName" );

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

  actionStructure["startSyncro"];
  actionStructure["stopSyncro"];
  actionStructure["syncroProcessError"] = (QStringList() << "errCode" );
  actionStructure["syncroError"];

  actionStructure["criticalError"] =(QStringList() << "errorMessage" );

  // По-умолчанию логирование запрещено
  enableLogging=false;
}


ActionLogger::~ActionLogger()
{
  closeLogFile();
}


void ActionLogger::init()
{
  openLogFileForWrite();
}


void ActionLogger::openLogFileForWrite()
{
  // Подготавливается лог-файл
  logFileName=globalParameters.getActionLogFileName();
  logPrevFileName=globalParameters.getActionLogPrevFileName();

  logFile.setFileName(logFileName); // Открывается файл лога
  bool result=logFile.open(QIODevice::Append | QIODevice::Text);
  if(!result)
    criticalError("Cant open log file "+logFileName+" for write data");
}


void ActionLogger::openLogFileForRead()
{
  // Подготавливается лог-файл
  logFileName=globalParameters.getActionLogFileName();
  logPrevFileName=globalParameters.getActionLogPrevFileName();

  logFile.setFileName(logFileName); // Открывается файл лога
  bool result=logFile.open(QIODevice::ReadOnly | QIODevice::Text);
  if(!result)
    criticalError("Cant open log file "+logFileName+" for readind data");
}


void ActionLogger::closeLogFile()
{
  logFile.close();
}


void ActionLogger::setEnableLogging(bool flag)
{
  enableLogging=flag;
}


// Получение текста лог-файла в виде XML-кода
QString ActionLogger::getXml()
{
  closeLogFile();
  openLogFileForRead();

  QString xmlText="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE mytetralog>\n<root>\n";

  // Получение текста лог-файла
  QTextStream in(&logFile);
  while (!in.atEnd())
  {
    QString line = in.readLine();
    xmlText+=line;
  }

  xmlText+="</root>";

  closeLogFile();
  openLogFileForWrite();

  return xmlText;
}


QString ActionLogger::getFullDescription(QDomElement element)
{
  QMap<QString, QString> data;
  QDomNamedNodeMap map = element.attributes();

  for (int i = 0; i < map.count(); ++i)
  {
    QDomNode attribute = map.item(i);
    data[ attribute.nodeName() ]= attribute.nodeValue();
  }

  return getFullDescription(data);
}


// Получение текстового описания действия
// iData содержит полные атрибуты записи, взятые из лога, включая "v" - версия, "t" - время, "a" - наименование действия
QString ActionLogger::getFullDescription(QMap<QString, QString> iData)
{
  QString line;

  // В зависимости от имени действия выбирается генерируемая строка описания действия
  if( iData["a"] == "startProgram" )
    line=tr("Program started");

  else if( iData["a"] == "stopProgram")
    line=tr("Program stop");

  else if( iData["a"] == "createRecord")
    line=tr("Create note \"%1\" with ID %2 in tree item \"%3\" with ID %4").arg( iData["recordName"] ).
                                                                            arg( iData["recordId"] ).
                                                                            arg( iData["branchName"] ).
                                                                            arg( iData["branchId"] );

  else if( iData["a"] == "createCryptRecord")
  {
    // Если пароль не введен, зашифрованные данные не расшифровываются и не показываются
    if(globalParameters.getCryptKey().length()==0)
    {
      iData["recordName"]="***";
      iData["branchName"]="***";
    }
    else
    {
      iData["recordName"]=CryptService::decryptString(globalParameters.getCryptKey(), iData["recordName"]);
      iData["branchName"]=CryptService::decryptString(globalParameters.getCryptKey(), iData["branchName"]);
    }

    line=tr("Create crypt note \"%1\" with ID %2 in tree item \"%3\" with ID %4").arg( iData["recordName"] ).
                                                                                  arg( iData["recordId"] ).
                                                                                  arg( iData["branchName"] ).
                                                                                  arg( iData["branchId"] );
  }

  else if( iData["a"] == "editRecord")
    line=tr("Edit fields of note \"%1\" with ID %2").arg( iData["recordName"] ).
                                                     arg( iData["recordId"]);

  else if( iData["a"] == "editCryptRecord")
  {
    // Если пароль не введен, зашифрованные данные не расшифровываются и не показываются
    if(globalParameters.getCryptKey().length()==0)
      iData["recordName"]="***";
    else
      iData["recordName"]=CryptService::decryptString(globalParameters.getCryptKey(), iData["recordName"]);

    line=tr("Edit fields of crypt note \"%1\" with ID %2").arg( iData["recordName"] ).
                                                           arg( iData["recordId"]);
  }

  else if( iData["a"] == "moveRecordUp")
    line=tr("Move up note \"%1\" with ID %2").arg( iData["recordName"] ).
                                              arg( iData["recordId"]);

  else if( iData["a"] == "moveRecordDown")
    line=tr("Move down note \"%1\" with ID %2").arg( iData["recordName"] ).
                                                arg( iData["recordId"]);

  else if( iData["a"] == "deleteRecord")
    line=tr("Delete note \"%1\" with ID %2").arg( iData["recordName"] ).
                                             arg( iData["recordId"]);

  else if( iData["a"] == "copyRecordToBuffer")
    line=tr("Copy note \"%1\" with ID %2 to clipboard").arg( iData["recordName"] ).
                                                        arg( iData["recordId"]);

  else if( iData["a"] == "cutRecordToBuffer")
    line=tr("Cut note \"%1\" with ID %2 to clipboard").arg( iData["recordName"] ).
                                                       arg( iData["recordId"]);

  else if( iData["a"] == "pasteRecordFromBuffer") // Добавить, какой новый ID у вставленной записи получился
    line=tr("Paste note \"%1\" with ID %2 from clipboard").arg( iData["recordName"] ).
                                                           arg( iData["recordId"]);

  else if( iData["a"] == "startDragRecord")
    line=tr("Start drag note \"%1\" with ID %2 from tree item \"%3\" with ID %4").arg( iData["recordName"] ).
                                                                                  arg( iData["recordId"] ).
                                                                                  arg( iData["branchName"] ).
                                                                                  arg( iData["branchId"] );

  else if( iData["a"] == "dropRecord")
    line=tr("Drop note \"%1\" with ID %2 to tree item \"%3\" with ID %4").arg( iData["recordName"] ).
                                                                          arg( iData["recordId"] ).
                                                                          arg( iData["branchName"] ).
                                                                          arg( iData["branchId"] );

  else if( iData["a"] == "startSyncro")
    line=tr("Start synchronization");

  else if( iData["a"] == "stopSyncro")
    line=tr("Stop synchronization");

  else if( iData["a"] == "syncroProcessError")
    line=tr("Synchronization process error detected. Error code: %1").arg( iData["errCode"]);

  else if( iData["a"] == "syncroError")
    line=tr("Synchronization error");

  else if( iData["a"] == "criticalError")
    line=tr("Critical error: %1").arg( iData["errorMessage"] );

  else
    line=tr("Unavailable action %1").arg( iData["a"] );

  return line;
}


// Добавление действия в лог
// iName - наименоание действия
// iData - атрибуты действия (в них НЕ входят v, t, a, эти атрибуты генерируется в теле этого метода на лету)
void ActionLogger::addAction(QString iName, QMap<QString, QString> iData)
{
  // Если логирование не разрешено
  if(!enableLogging)
    return;

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

  qDebug() << "Action log: " << line;

  // Строка записывается в лог
  QTextStream out(&logFile);
  out << line << "\n";
}

