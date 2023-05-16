#include <QDir>
#include <QFile>
#include <QDebug>

#include "main.h"
#include "KnownBasesConfig.h"

#include "models/appConfig/AppConfig.h"
#include "models/appConfig/AppConfigUpdater.h"
#include "libraries/GlobalParameters.h"
#include "libraries/helpers/DebugHelper.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


const QString KnownBasesConfig::sectionPrefix="num";
const QStringList KnownBasesConfig::fieldList={"dbPath", "trashPath"};


// Конструктор объекта настройки БД
KnownBasesConfig::KnownBasesConfig(QObject *pobj)
{
 Q_UNUSED(pobj)

 isInitFlag=false;
}


// Деструктор объекта настройки БД
KnownBasesConfig::~KnownBasesConfig()
{
 if(isInitFlag)
  {
   qDebug() << "Save known bases config file";
   conf->sync();
  }
}


void KnownBasesConfig::init(void)
{
 // Создается имя файла конфигурации
 // QString configFileName=globalParameters.getWorkDirectory()+"/"+mytetraConfig.get_tetradir()+"/database.ini";
 QString configFileName=globalParameters.getWorkDirectory()+"/knownbases.ini";

 // Проверяется, есть ли файл конфигурации
 QFile confFile(configFileName);
 if(!confFile.exists())
  {
   // Если файла нет, создается конфигфайл с начальным содержимым
   QSettings tempConf(configFileName, QSettings::IniFormat);

   tempConf.setValue("version", 1);

   tempConf.sync();
  }

 // Создается указатель на объект хранилища конфигурации
 conf=new QSettings(configFileName, QSettings::IniFormat, this);

 conf->sync();

 isInitFlag=true;
}


bool KnownBasesConfig::isInit(void)
{
 return isInitFlag;
}


// Получение параметра по имени в виде строки с проверкой его существования
QString KnownBasesConfig::getParameter(QString name)
{
 QString t=conf->value(name).toString();

 if(t.length()==0)
  criticalError("In known bases config not found parameter " + name);

 return t;
}


/*
int KnownBasesConfig::get_crypt_mode(void)
{
 return conf->value("crypt_mode", 0).toInt();
}


void KnownBasesConfig::set_crypt_mode(int mode)
{
 conf->setValue("crypt_mode", mode);
}
*/


// --------------------
// Номер версии конфига
// --------------------

int KnownBasesConfig::getConfigVersion(void)
{
 if(conf->contains("version"))
  return conf->value("version").toInt();
 else
  return 0;
}


void KnownBasesConfig::setConfigVersion(int i)
{
    conf->setValue("version", i);
}


int KnownBasesConfig::getDbCount()
{
    for(int i=0; i<KNOWN_BASES_MAX_COUNT; ++i)
    {
        if( !conf->contains(sectionPrefix+QString::number(i)+"/"+fieldList[0]) )
        {
            return i;
        }
    }

    return 0;
}


QString KnownBasesConfig::getDbParameter(const int &num, const QString &name)
{
    QVariant value=conf->value(sectionPrefix+QString::number(num)+"/"+name);

    if(value.isValid())
    {
        return value.toString();
    }
    else
    {
        return "";
    }
}


void KnownBasesConfig::setDbParameter(const int &num, const QString &name, const QString &value)
{
    conf->setValue(sectionPrefix+QString::number(num)+"/"+name, value);
}


bool KnownBasesConfig::isDbParameterExists(const QString &name, const QString &value)
{
    for(int i=0; i<KNOWN_BASES_MAX_COUNT; ++i)
    {
        QVariant currentValue=conf->value( sectionPrefix+QString::number(i)+"/"+name );

        if( currentValue.isValid() && currentValue.toString()==value )
        {
            return true;
        }
    }

    return false;
}


int KnownBasesConfig::getExistsParameterNum(const QString &name, const QString &value)
{
    for(int i=0; i<KNOWN_BASES_MAX_COUNT; ++i)
    {
        QVariant currentValue=conf->value( sectionPrefix+QString::number(i)+"/"+name );

        if( currentValue.isValid() && currentValue.toString()==value )
        {
            return i;
        }
    }

    return -1;
}
