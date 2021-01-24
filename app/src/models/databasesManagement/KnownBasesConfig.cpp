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
 QString configFileName=mytetraConfig.get_tetradir()+"/knownbases.ini";

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
  criticalError("In database config not found parameter " + name);

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
