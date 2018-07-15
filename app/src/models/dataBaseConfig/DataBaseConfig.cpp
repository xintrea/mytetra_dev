#include <QDir>
#include <QFile>
#include <QDebug>

#include "main.h"
#include "DataBaseConfig.h"

#include "models/appConfig/AppConfig.h"
#include "models/appConfig/AppConfigUpdater.h"
#include "libraries/GlobalParameters.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


// Объект для работы с конфигурацией (с настройками) базы данных
// Конфигурация базы данных хранится в файле database.ini

// Под базой данных понимается совокупность файлов - корневой mytetra.xml 
// и все директории и файлы с записями


// Конструктор объекта настройки БД
DataBaseConfig::DataBaseConfig(QObject *pobj)
{
 Q_UNUSED(pobj);

 is_init_flag=false;
}


// Деструктор объекта настройки БД
DataBaseConfig::~DataBaseConfig()
{
 if(is_init_flag)
  {
   qDebug() << "Save database config file";
   conf->sync();
  }
}


void DataBaseConfig::init(void)
{
 // Создается имя файла конфигурации
 // QString configFileName=globalParameters.getWorkDirectory()+"/"+mytetraConfig.get_tetradir()+"/database.ini";
 QString configFileName=mytetraConfig.get_tetradir()+"/database.ini";

 // Проверяется, есть ли файл конфигурации
 QFile confFile(configFileName);
 if(!confFile.exists())
  {
   // Если файла нет, создается конфигфайл с начальным содержимым
   QSettings tempConf(configFileName, QSettings::IniFormat);

   tempConf.setValue("version", 1);
   tempConf.setValue("crypt_mode", 0);
   tempConf.setValue("crypt_check_salt", "");
   tempConf.setValue("crypt_check_hash", "");
   tempConf.setValue("middle_hash_check_data", "");

   tempConf.sync();
  }

 // Создается указатель на объект хранилища конфигурации
 conf=new QSettings(configFileName, QSettings::IniFormat);

 conf->sync();

 is_init_flag=true;
}


bool DataBaseConfig::is_init(void)
{
 return is_init_flag;
}


// Получение параметра по имени в виде строки с проверкой его существования
QString DataBaseConfig::get_parameter(QString name)
{
 QString t=conf->value(name).toString();

 if(t.length()==0)
  criticalError("In database config not found parameter " + name);

 return t;
}


int DataBaseConfig::get_crypt_mode(void)
{
 return conf->value("crypt_mode", 0).toInt();
}


void DataBaseConfig::set_crypt_mode(int mode)
{
 conf->setValue("crypt_mode", mode);
}


QString DataBaseConfig::get_crypt_check_salt(void)
{
 return get_parameter("crypt_check_salt");
}


void DataBaseConfig::set_crypt_check_salt(QString salt)
{
 conf->setValue("crypt_check_salt", salt);
}


QString DataBaseConfig::get_crypt_check_hash(void)
{
 return get_parameter("crypt_check_hash");
}

void DataBaseConfig::set_crypt_check_hash(QString hash)
{
 conf->setValue("crypt_check_hash", hash);
}


QString DataBaseConfig::get_middle_hash_check_data(void)
{
 return get_parameter("middle_hash_check_data");
}


void DataBaseConfig::set_middle_hash_check_data(QString hash)
{
 conf->setValue("middle_hash_check_data", hash);
}


// --------------------
// Номер версии конфига
// --------------------

int DataBaseConfig::get_config_version(void)
{
 if(conf->contains("version"))
  return conf->value("version").toInt();
 else
  return 0;
}


void DataBaseConfig::set_config_version(int i)
{
 conf->setValue("version", i);
}
