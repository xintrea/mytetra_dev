#include <QRect>

#include "main.h"
#include "AppConfig.h"
#include "AppConfigUpdater.h"

#include "libraries/GlobalParameters.h"

extern GlobalParameters globalParameters;


// Конструктор объекта настройки программы
AppConfig::AppConfig(QObject *pobj)
{
 Q_UNUSED(pobj);

 is_init_flag=false;
}


// Деструктор объекта настройки программы
AppConfig::~AppConfig()
{
 if(is_init_flag)
  {
   qDebug() << "Save mytetra config file";
   conf->sync();
  }
}


void AppConfig::init(void)
{
 // Создается имя файла конфигурации
 QString configFileName=globalParameters.getWorkDirectory()+"/conf.ini";

 // Проверяется, есть ли файл конфигурации
 QFile conffile(configFileName);
 if(!conffile.exists())
  critical_error("File "+configFileName+" not found.");

 // Создается указатель на объект хранилища конфигурации
 conf=new QSettings(configFileName, QSettings::IniFormat);

 // conf->setPath(QSettings::IniFormat, QSettings::UserScope,"./");
 // conf->setPath(QSettings::IniFormat, QSettings::SystemScope,"./");

 update_version_process();

 conf->sync();

 is_init_flag=true;
}


bool AppConfig::is_init(void)
{
 return is_init_flag;
}


// Получение параметра по имени в виде строки с проверкой его существования
QString AppConfig::get_parameter(QString name)
{
 if(conf->contains(name))
  return conf->value(name).toString();
 else
  critical_error("In config not found parameter " + name);

 return QString();
}



// Установка имени рабочей директории
bool AppConfig::set_tetradir(QString dirName)
{
 QDir directory(dirName);

 if(directory.exists() && directory.isReadable())
  {
   conf->setValue("tetradir",dirName);
   return true;
  }
 else
  return false;
}


// Получение имени рабочей директории
QString AppConfig::get_tetradir(void)
{
 return get_parameter("tetradir");
}


// Установка имени директории с корзиной
bool AppConfig::set_trashdir(QString dirName)
{
 QDir directory(dirName);

 if(directory.exists() && directory.isReadable())
  {
   conf->setValue("trashdir",dirName);
   return true;
  }
 else
  return false;
}


// Получение имени директории с корзиной
QString AppConfig::get_trashdir(void)
{
 return get_parameter("trashdir");
}


// Получение максимального размера директории корзины в мегабайтах
unsigned int AppConfig::get_trashsize(void)
{
 return get_parameter("trashsize").toInt();
}


// Установка максимального размера директории корзины в мегабайтах
bool AppConfig::set_trashsize(unsigned int mbSize)
{
 if(mbSize>0)
  {
   conf->setValue("trashsize",mbSize);
   return true;
  }
 else
  return false;
}


// Получение максимально допустимого числа файлов в корзине
int AppConfig::get_trashmaxfilecount(void)
{
 return get_parameter("trashmaxfilecount").toInt();
}


// Установка максимально допустимого числа файлов в корзине
bool AppConfig::set_trashmaxfilecount(int count)
{
 if(count>0)
  {
   conf->setValue("trashmaxfilecount",count);
   return true;
  }
 else
  return false;
}


bool AppConfig::get_cutbranchconfirm(void)
{
 return conf->value("cutbranchconfirm").toBool();
}


bool AppConfig::set_cutbranchconfirm(bool confirm)
{
 conf->setValue("cutbranchconfirm", confirm);
 return true;
}


bool AppConfig::get_printdebugmessages(void)
{
 return conf->value("printdebugmessages").toBool();
}


bool AppConfig::set_printdebugmessages(bool isPrint)
{
 conf->setValue("printdebugmessages", isPrint);
 return true;
}


QString AppConfig::get_interfacelanguage(void)
{
 QString language=get_parameter("interfacelanguage");
 qDebug() << "Get interface language" << language;
 return language;
}


bool AppConfig::set_interfacelanguage(QString language)
{
 conf->setValue("interfacelanguage", language);
 return true;
}


QString AppConfig::get_addnewrecord_expand_info(void)
{
 return get_parameter("addnewrecord_expand_info");
}


void AppConfig::set_addnewrecord_expand_info(QString state)
{
 if(state=="0" || state=="1")
  conf->setValue("addnewrecord_expand_info",state);
 else
  critical_error("Set unavailable value for addnewrecord_expand_info "+state);
}


QRect AppConfig::get_mainwingeometry(void)
{
 QRect rect;
 QString rectString;
 QStringList rectParameter;

 rectString=conf->value("mainwingeometry", "0,0,500,400").toString();

 rectParameter=rectString.split(",");

 int x=rectParameter[0].toInt();
 int y=rectParameter[1].toInt();
 int w=rectParameter[2].toInt();
 int h=rectParameter[3].toInt();

 rect.setRect(x, y, w, h);

 return rect;
}


void AppConfig::set_mainwingeometry(int x, int y, int w, int h)
{
 qDebug() << "Save new main window geometry";

 QString result=QString::number(x)+","+QString::number(y)+","+QString::number(w)+","+QString::number(h);

 conf->setValue("mainwingeometry", result);
}


QList<int> AppConfig::get_vspl_size_list(void)
{
 return get_splitter_size_list("vspl");
}


void AppConfig::set_vspl_size_list(QList<int> list)
{
 set_splitter_size_list("vspl", list);
}


QList<int> AppConfig::get_hspl_size_list(void)
{
 return get_splitter_size_list("hspl");
}


void AppConfig::set_hspl_size_list(QList<int> list)
{
 set_splitter_size_list("hspl", list);
}


QList<int> AppConfig::get_findsplitter_size_list(void)
{
 return get_splitter_size_list("findsplitter");
}


void AppConfig::set_findsplitter_size_list(QList<int> list)
{
 qDebug() << "Config set find splitter list to " << list;
 set_splitter_size_list("findsplitter", list);
}


QList<int> AppConfig::get_splitter_size_list(QString name)
{
 QStringList line_list;
 QList<int> list;

 line_list=(conf->value(name+"_size_list","100,100")).toString().split(",");
 
 for(int i=0;i < line_list.size(); ++i)
  list.append( line_list.at(i).toInt() );
 
 return list;
}


void AppConfig::set_splitter_size_list(QString name, QList<int> list)
{
 QStringList line_list;
 
 for(int i=0;i < list.size(); ++i)
  line_list.append( QString::number(list.at(i)) );
 
 conf->setValue(name+"_size_list",line_list.join(","));
}
   

QStringList AppConfig::get_tree_position(void)
{
 return (conf->value("tree_position","1")).toString().split(",");
}


void AppConfig::set_tree_position(QStringList list)
{
 conf->setValue("tree_position",list.join(","));
}


int AppConfig::get_recordtable_position(void)
{
 return conf->value("recordtable_position",0).toInt();
}


void AppConfig::set_recordtable_position(int pos)
{
 conf->setValue("recordtable_position",pos);
}


int AppConfig::get_findscreen_wordregard(void)
{
 return conf->value("findscreen_wordregard",0).toInt();
}


void AppConfig::set_findscreen_wordregard(int pos)
{
 conf->setValue("findscreen_wordregard",pos);
}


int AppConfig::get_findscreen_howextract(void)
{
 return conf->value("findscreen_howextract",0).toInt();
}


void AppConfig::set_findscreen_howextract(int pos)
{
 conf->setValue("findscreen_howextract",pos);
}


int AppConfig::getFindScreenTreeSearchArea(void)
{
 return conf->value("findScreenTreeSearchArea",0).toInt();
}


void AppConfig::setFindScreenTreeSearchArea(int pos)
{
 conf->setValue("findScreenTreeSearchArea",pos);
}


bool AppConfig::get_findscreen_find_in_field(QString fieldName)
{
 return conf->value("findscreen_find_in"+fieldName,0).toBool();
}


void AppConfig::set_findscreen_find_in_field(QString fieldName, bool isChecked)
{
 conf->setValue("findscreen_find_in"+fieldName,isChecked);
}


bool AppConfig::get_findscreen_show(void)
{
 return conf->value("findscreen_show",0).toBool();
}


void AppConfig::set_findscreen_show(bool isShow)
{
 conf->setValue("findscreen_show",isShow);
}


QString AppConfig::get_howpassrequest(void)
{
 return get_parameter("howpassrequest");
}


void AppConfig::set_howpassrequest(QString mode)
{
 if(mode=="atClickOnCryptBranch" || mode=="atStartProgram")
  conf->setValue("howpassrequest", mode);
 else
  critical_error("Set unavailable value for howpassrequest "+mode);
}


bool AppConfig::get_autoClosePasswordEnable(void)
{
 return conf->value("autoClosePasswordEnable",0).toBool();
}


void AppConfig::set_autoClosePasswordEnable(bool flag)
{
 conf->setValue("autoClosePasswordEnable", flag);
}


int AppConfig::get_autoClosePasswordDelay(void)
{
 return conf->value("autoClosePasswordDelay",0).toInt();
}


void AppConfig::set_autoClosePasswordDelay(int delay)
{
 conf->setValue("autoClosePasswordDelay", delay);
}


bool AppConfig::get_runinminimizedwindow(void)
{
 return conf->value("runinminimizedwindow").toBool();
}


void AppConfig::set_runinminimizedwindow(bool flag)
{
 conf->setValue("runinminimizedwindow", flag);
}


QString AppConfig::get_synchrocommand(void)
{
 return get_parameter("synchrocommand");
}


void AppConfig::set_synchrocommand(QString command)
{
 conf->setValue("synchrocommand", command);
}


bool AppConfig::get_synchroonstartup(void)
{
 return conf->value("synchroonstartup").toBool();
}


void AppConfig::set_synchroonstartup(bool flag)
{
 conf->setValue("synchroonstartup", flag);
}


bool AppConfig::get_synchroonexit(void)
{
 return conf->value("synchroonexit").toBool();
}


void AppConfig::set_synchroonexit(bool flag)
{
 conf->setValue("synchroonexit", flag);
}


bool AppConfig::getSyncroConsoleDetails(void)
{
 return conf->value("syncroConsoleDetails").toBool();
}


void AppConfig::setSyncroConsoleDetails(bool flag)
{
 conf->setValue("syncroConsoleDetails", flag);
}


int AppConfig::getEditorCursorPosition(void)
{
 return conf->value("editorCursorPosition",0).toInt();
}


void AppConfig::setEditorCursorPosition(int n)
{
 conf->setValue("editorCursorPosition", n); 
}


int AppConfig::getEditorScrollBarPosition(void)
{
 return conf->value("editorScrollBarPosition",0).toInt();
}


void AppConfig::setEditorScrollBarPosition(int n)
{
 conf->setValue("editorScrollBarPosition", n);
}


QString AppConfig::getPasswordMiddleHash(void)
{
 return get_parameter("passwordMiddleHash");
}


void AppConfig::setPasswordMiddleHash(QString hash)
{
 conf->setValue("passwordMiddleHash", hash);
}


// Нужно ли локально хранить пароль (точнее, промежуточный хеш пароля)
bool AppConfig::getPasswordSaveFlag(void)
{
 return conf->value("passwordSaveFlag").toBool();
}


void AppConfig::setPasswordSaveFlag(bool flag)
{
 conf->setValue("passwordSaveFlag", flag);
}


bool AppConfig::getRememberCursorAtHistoryNavigation(void)
{
 return conf->value("rememberCursorAtHistoryNavigation").toBool();
}


void AppConfig::setRememberCursorAtHistoryNavigation(bool flag)
{
 conf->setValue("rememberCursorAtHistoryNavigation", flag);
}


bool AppConfig::getRememberCursorAtOrdinarySelection(void)
{
 return conf->value("rememberCursorAtOrdinarySelection").toBool();
}


void AppConfig::setRememberCursorAtOrdinarySelection(bool flag)
{
 conf->setValue("rememberCursorAtOrdinarySelection", flag);
}


// --------------------
// Номер версии конфига
// --------------------

int AppConfig::get_config_version(void)
{
 if(conf->contains("version"))
  return conf->value("version").toInt();
 else
  return 0;
}


void AppConfig::set_config_version(int i)
{
 conf->setValue("version", i);
}


QStringList AppConfig::remove_parameter_from_table(QString removeName, QStringList table)
{
 // Перебираются параметры в таблице
 for(int i=0; i<MYTETRA_CONFIG_PARAM_NUM; i++)
 {
  // Выясняется имя параметра
  QString name=table.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);

  // Если имя совпадает с удаляемым
  if(name==removeName)
   {
    // Удаляются данные об элементе (удаляется ячейка с одним и тем же номером
    // столько раз, сколько полей в таблице)
    for(int j=0; j<MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD; j++)
     table.removeAt(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);

    break;
   }
 }

 return table;
}


// ------------------------------------
// Методы для обновления версии конфига
// ------------------------------------

void AppConfig::update_version_process(void)
{
 QString configFileName=globalParameters.getWorkDirectory()+"/conf.ini";

 AppConfigUpdater updater;
 updater.set_config_file(configFileName);

 int fromVersion=get_config_version();

 // Последняя версия на данный момент - 14
 if(fromVersion<=1)
  updater.update_version(1,  2,  get_parameter_table_1(),  get_parameter_table_2());
 if(fromVersion<=2)
  updater.update_version(2,  3,  get_parameter_table_2(),  get_parameter_table_3());
 if(fromVersion<=3)
  updater.update_version(3,  4,  get_parameter_table_3(),  get_parameter_table_4());
 if(fromVersion<=4)
  updater.update_version(4,  5,  get_parameter_table_4(),  get_parameter_table_5());
 if(fromVersion<=5)
  updater.update_version(5,  6,  get_parameter_table_5(),  get_parameter_table_6());
 if(fromVersion<=6)
  updater.update_version(6,  7,  get_parameter_table_6(),  get_parameter_table_7());
 if(fromVersion<=7)
  updater.update_version(7,  8,  get_parameter_table_7(),  get_parameter_table_8());
 if(fromVersion<=8)
  updater.update_version(8,  9,  get_parameter_table_8(),  get_parameter_table_9());
 if(fromVersion<=9)
  updater.update_version(9,  10, get_parameter_table_9(),  get_parameter_table_10());
 if(fromVersion<=10)
  updater.update_version(10, 11, get_parameter_table_10(), get_parameter_table_11());
 if(fromVersion<=11)
  updater.update_version(11, 12, get_parameter_table_11(), get_parameter_table_12());
 if(fromVersion<=12)
  updater.update_version(12, 13, get_parameter_table_12(), get_parameter_table_13());
 if(fromVersion<=13)
  updater.update_version(13, 14, get_parameter_table_13(), get_parameter_table_14());
}


QStringList AppConfig::get_parameter_table_1(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 table << "addnewrecord_expand_info" << "int" << "1";
 table << "findscreen_find_inname"   << "bool" << "true";
 table << "findscreen_find_intags"   << "bool" << "true";
 table << "findscreen_find_intext"   << "bool" << "true";
 table << "findscreen_howextract"    << "int" << "1";
 table << "findscreen_show"          << "bool" << "false";
 table << "findscreen_wordregard"    << "int" << "1";
 table << "findsplitter_size_list"   << "QString" << "517,141";
 table << "hspl_size_list"           << "QString" << "237,621";
 table << "lastidnum"                << "int" << "3537";
 table << "lastnotenum"              << "int" << "3119";
 table << "lastprefixnum"            << "int" << "7540";
 table << "mainwingeometry"          << "QString" << "155,24,864,711)";
 table << "recordtable_position"     << "int" << "0";
 table << "tetradir"                 << "QString" << "/opt/mytetra/data";
 table << "trashdir"                 << "QString" << "/opt/mytetra/trash";
 table << "trashmaxfilecount"        << "int" << "200";
 table << "trashsize"                << "int" << "5";
 table << "tree_position"            << "QString" << "0,1818,1819";
 table << "vspl_size_list"           << "QString" << "171,487";
 table << "findscreen_find_inauthor" << "bool" << "true";
 table << "findscreen_find_inurl"    << "bool" << "false";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_2(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 1
 table << get_parameter_table_1(false);

 // Новый параметр
 table << "cutbranchconfirm"         << "bool" << "true";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_3(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 2
 table << get_parameter_table_2(false);

 // Новый параметр
 table << "printdebugmessages"         << "bool" << "false";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_4(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 3
 table << get_parameter_table_3(false);

 // Новые параметры
 table << "interfacelanguage" << "QString" << "en";
 table << "programm" << "QString" << "mytetra";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_5(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 4
 table << get_parameter_table_4(false);

 // Исключаются ненужные в новой версии параметры
 table=remove_parameter_from_table("lastidnum", table);
 table=remove_parameter_from_table("lastnotenum", table);
 table=remove_parameter_from_table("lastprefixnum", table);

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_6(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 5
 table << get_parameter_table_5(false);

 // Новые параметры
 table << "howpassrequest" << "QString" << "atClickOnCryptBranch";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_7(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 6
 table << get_parameter_table_6(false);

 // Новые параметры
 table << "runinminimizedwindow" << "bool" << "false";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_8(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 7
 table << get_parameter_table_7(false);

 // Новые параметры
 table << "synchrocommand" << "QString" << "";
 table << "synchroonstartup" << "bool" << "false";
 table << "synchroonexit" << "bool" << "false";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_9(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 8
 table << get_parameter_table_8(false);

 // Новые параметры
 table << "syncroConsoleDetails" << "bool" << "false";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_10(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 9
 table << get_parameter_table_9(false);

 // Новые параметры
 table << "autoClosePasswordEnable" << "bool" << "false";
 table << "autoClosePasswordDelay" << "int" << "10";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_11(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 10
 table << get_parameter_table_10(false);

 // Новые параметры
 table << "editorCursorPosition" << "int" << "0";
 table << "editorScrollBarPosition" << "int" << "0";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_12(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 11
 table << get_parameter_table_11(false);

 // Новые параметры
 table << "passwordMiddleHash" << "QString" << "";
 table << "passwordSaveFlag" << "bool" << "false";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_13(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 12
 table << get_parameter_table_12(false);

 // Новые параметры
 table << "rememberCursorAtHistoryNavigation" << "bool" << "true";
 table << "rememberCursorAtOrdinarySelection" << "bool" << "true";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_14(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 13
 table << get_parameter_table_13(false);

 // Новые параметры
 table << "findScreenTreeSearchArea" << "int" << "0"; // Область поиска. Искать во всем дереве - 0, искать в текущей ветке - 1

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}

