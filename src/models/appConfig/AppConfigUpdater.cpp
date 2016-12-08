#include <QSettings>
#include <QFile>

#include "AppConfigUpdater.h"
#include "main.h"

#define APPCONFIGUPDATER_VERSION "APPCONFIGUPDATER v.1.0 Build 29.10.2010"


AppConfigUpdater::AppConfigUpdater(QObject *pobj) : maxParameterCount(100)
{
 Q_UNUSED(pobj);

 conf=NULL;
}


AppConfigUpdater::~AppConfigUpdater()
{
 delete conf;
}


void AppConfigUpdater::set_config_file(QString fileName)
{
 // Проверяется, есть ли файл конфигурации
 QFile conffile(fileName);
 if(!conffile.exists())
  criticalError("appconfigupdater::set_config_file() - File "+fileName+" not found.");

 // Создается рабочий объект для работы с конфигурацией
 conf=new QSettings(fileName, QSettings::IniFormat);
 conf->setPath(QSettings::IniFormat, QSettings::UserScope,"./");
 conf->setPath(QSettings::IniFormat, QSettings::SystemScope,"./");
}


// Метод разрешения конфликтов
// Должен включать в себя логику обработки только тех параметров
// и только для тех версий конфигов, которые действительно
// должны поменять тип. Если для принятых параметров не будет
// обработки, это значит что что-то сделано программистом не так
// и нужно дорабатывать код
QString AppConfigUpdater::update_version_allowcollision(int versionFrom,
                                                        int versionTo,
                                                        QString name,
                                                        QString fromType,
                                                        QString fromValue,
                                                        QString toType,
                                                        QString toValue)
{

 // todo: Прописать сюда правила перевода int в bool и обратно

 criticalError("Error while update config version \nFrom: "+(QString::number(versionFrom))+
                "\nTo: "+(QString::number(versionTo))+
                "\nName: "+name+
                "\nFrom type: "+fromType+
                "\nFrom value: "+fromValue+
                "\nTo type: "+toType+
                "\nTo value: "+toValue);

 return QString();
}


// Основной метод обновления версий конфига
void AppConfigUpdater::update_version(int versionFrom,
                                      int versionTo,
                                      QStringList baseTable,
                                      QStringList finalTable)
{
  // Таблица исходных параметров преобразуется к более удобному для работы виду
  // И параллельно заполняется значениями из конфига
  QMap< QString, QMap< QString, QString > > fromTable;
  for(int i=0; i<maxParameterCount; i++)
  {
    QString name=    baseTable.at(i*3+0);
    QString type=    baseTable.at(i*3+1);
    QString defValue=baseTable.at(i*3+2);

    // Если достигнут конец массива
    if(name=="0" && type=="0" && defValue=="0")
      break;

    // Подготовка массива для текущего параметра
    QMap< QString, QString > line;
    line.clear();
    line["type"]=type;
    if(conf->contains(name))
      line["value"]=conf->value(name).toString(); // Значение из конфига
    else
      line["value"]=defValue; // Дефолтное значение

    // Для текущего имени параметра запоминается массив
    fromTable[name]=line;
  }


 // Таблица конечных параметров преобразуется к более удобному для работы виду
 // Параллельно создаётся список контролирующий обработанные параметры
 // Далее из контролирующего списка будут убираться обработанные параметры
 // Контролирующий список нужен для того, чтобы не удалять записи
 // из конечного массива во время обхода через итератор
 QMap< QString, QMap< QString, QString > > toTable;
 QList<QString> controlList;
 for(int i=0; i<MYTETRA_CONFIG_PARAM_NUM; i++)
 {
  QString name=    finalTable.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+0);
  QString type=    finalTable.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+1);
  QString defValue=finalTable.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+2);

  // Если достигнут конец массива
  if(name=="0" && type=="0" && defValue=="0") break;

  // Подготовка массива для текущего параметра
  QMap< QString, QString > line;
  line.clear();
  line["type"]=type;
  line["value"]=defValue; // Дефолтное значение

  // Для текущего имени параметра запоминается массив
  toTable[name]=line;
  controlList << name; // Имя заносится в контролирующий список
 }

 qDebug() << "From table";
 qDebug() << fromTable;
 qDebug() << "To table";
 qDebug() << toTable;

 // Перебирается конечный массив
 QMapIterator< QString, QMap< QString, QString > > i(toTable);
 while(i.hasNext())
 {
   i.next();

   // Данные для новой версии конфига
   QString toName=i.key();
   QMap< QString, QString > line=i.value();
   QString toType=line["type"];
   QString toValue=line["value"];

   qDebug() << "To name: " << toName;
   qDebug() << "To type: " << toType;
   qDebug() << "To value: " << toValue;

   // Определяется, есть ли полный аналог параметра в предыдущей версии конфига
   int beforeParamFlag=0;
   QMap< QString, QString > line2;
   QString fromType;
   QString fromValue;
   if(fromTable.contains(toName))
   {
    line2=fromTable[toName];
    fromType=line2["type"];
    fromValue=line2["value"];

    qDebug() << "Line2: " << line2;
    qDebug() << "From type: " << fromType;
    qDebug() << "From value: " << fromValue;

    if(toType==fromType)
     beforeParamFlag=1; // Параметр есть, и типы совпадают
    else
     beforeParamFlag=2; // Параметр есть, но типы не совпадают
   }


   // Параметра в предыдущей версии конфига небыло
   if(beforeParamFlag==0)
   {
    // Будет просто сохранено новое дефолтное значение
    // Ничего с конечным параметром делать ненужно
    // Параметр из контролирующего массива исключается
    controlList.removeOne(toName);
   }


   // Параметр в предыдущей версии конфига есть, и типы совпадают
   if(beforeParamFlag==1)
   {
    // Будет сохранено предыдущее значение
    toTable[toName]["value"]=fromValue;

    // Параметр из контролирующего массива исключается
    controlList.removeOne(toName);
   }


   // Параметр в предыдущей версии конфига есть, но типы не совпадают
   if(beforeParamFlag==2)
   {
    // Будет возвращено высчитанное значение
    toTable[toName]["value"]=update_version_allowcollision(versionFrom,versionTo,toName,
                                                             fromType,fromValue,
                                                             toType,toValue);

    // Параметр из контролирующего массива исключается
    controlList.removeOne(toName);
   }

 } // Закрылся перебор конечного массива


 // Если в контролирующем массиве остались необработанные параметры
 if(controlList.size()>0)
 {
  // Программа завершается
  qDebug() << "Can not compute parameter " << controlList;
  criticalError("Error while update config from "+(QString::number(versionFrom))+" to "+(QString::number(versionTo)) );
 }

 // Конфиг обнуляется
 conf->clear();

 // Конечный массив записывается в конфиг
 QMapIterator< QString, QMap< QString, QString > > j(toTable);
 while(j.hasNext())
 {
  j.next();

  QString toName=j.key();

  QMap< QString, QString > line=j.value();
  QString toType=line["type"];
  QString toValue=line["value"];

  if(toType=="QString")
   conf->setValue(toName,toValue);
  else if(toType=="int")
   conf->setValue(toName,toValue.toInt());
  else if(toType=="bool")
   {
    if(toValue=="true") conf->setValue(toName, true);
    else conf->setValue(toName, false);
   }
 }

 // Устанавливается новый номер версии
 conf->setValue("version", versionTo);

 // Конфигурация записывается на диск
 conf->sync();
}

