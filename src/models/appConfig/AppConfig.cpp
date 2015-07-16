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
 QFile confFile(configFileName);
 if(!confFile.exists())
  critical_error("File "+configFileName+" not found.");

 // Создается указатель на объект хранилища конфигурации
 conf=new QSettings(configFileName, QSettings::IniFormat);

 update_version_process();

 sync();

 is_init_flag=true;
}


bool AppConfig::is_init(void)
{
 return is_init_flag;
}


void AppConfig::sync(void)
{
 conf->sync();
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



// Установка имени директории с данными (в которой находится mytetra.xml)
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


// Получение имени директории с данными (в которой находится mytetra.xml)
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
  qDebug() << "AppConfig::set_tree_position() : " << list;
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


// ID записи в таблице конечных записей, которая выделена на экране
QString AppConfig::get_recordtable_selected_record_id(void)
{
  return conf->value("recordtableSelectedRecordId",0).toString();
}


// ID записи в таблице конечных записей, которая выделена на экране
void AppConfig::set_recordtable_selected_record_id(QString id)
{
  conf->setValue("recordtableSelectedRecordId", id);
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


int AppConfig::getUglyQssReplaceHeightForTableView(void)
{
 return conf->value("uglyQssReplaceHeightForTableView",0).toInt();
}

void AppConfig::setUglyQssReplaceHeightForTableView(int n)
{
 conf->setValue("uglyQssReplaceHeightForTableView", n);
}


// Перечень полей, отображаемых в таблице конечных записей
QStringList AppConfig::getRecordTableShowFields(void)
{
 return (conf->value("recordTableShowFields", "name")).toString().split(",");
}


void AppConfig::setRecordTableShowFields(QStringList fields)
{
 // Не должно быть так, чтобы был пустой список
 if(fields.size()==0)
   fields << "name";

 conf->setValue("recordTableShowFields",fields.join(","));
}


bool AppConfig::getRecordTableShowHorizontalHeaders(void)
{
 return conf->value("recordTableShowHorizontalHeaders").toBool();
}


void AppConfig::setRecordTableShowHorizontalHeaders(bool flag)
{
 conf->setValue("recordTableShowHorizontalHeaders", flag);
}


bool AppConfig::getRecordTableShowVerticalHeaders(void)
{
 return conf->value("recordTableShowVerticalHeaders").toBool();
}


void AppConfig::setRecordTableShowVerticalHeaders(bool flag)
{
 conf->setValue("recordTableShowVerticalHeaders", flag);
}


// Ширина полей, отображаемых в таблице конечных записей
QStringList AppConfig::getRecordTableFieldsWidth(void)
{
 return (conf->value("recordTableFieldsWidth", "256")).toString().split(",");
}


void AppConfig::setRecordTableFieldsWidth(QStringList fields)
{
 // В списке с одним элементом должна стоять стандартная величина (всеравно она не влияет, и начнет влиять только после появления второй колонки)
 if(fields.size()==1)
   fields[0]="256";

 // Не должно быть так, чтобы был пустой список
 if(fields.size()==0)
   fields << "256";

 conf->setValue("recordTableFieldsWidth",fields.join(","));
}


// Показывать ли сплешскрин при старте программы
bool AppConfig::getShowSplashScreen(void)
{
 return conf->value("showSplashScreen").toBool();
}


void AppConfig::setShowSplashScreen(bool isShow)
{
 conf->setValue("showSplashScreen", isShow);
}


// Режим интерфейса, возможные значения "desktop" и "mobile"
QString AppConfig::getInterfaceMode(void)
{
 QString mode=get_parameter("interfaceMode");
 return mode;
}


void AppConfig::setInterfaceMode(QString mode)
{
 conf->setValue("interfaceMode", mode);
}

// Имя последнего активного виджета
QString AppConfig::getFocusWidget(void)
{
 QString widgetName=get_parameter("focusWidget");
 return widgetName;
}


void AppConfig::setFocusWidget(QString widgetName)
{
 qDebug() << "AppConfig::setFocusWidget() : " << widgetName;
 conf->setValue("focusWidget", widgetName);
}


QStringList AppConfig::getHideEditorTools(void)
{
 return (conf->value("hideEditorTools", "")).toString().split(",");
}


void AppConfig::setHideEditorTools(QStringList toolsNames)
{
 conf->setValue("hideEditorTools",toolsNames.join(","));
}


bool AppConfig::getFindInBaseExpand(void)
{
 return conf->value("findInBaseExpand").toBool();
}


void AppConfig::setFindInBaseExpand(bool state)
{
 conf->setValue("findInBaseExpand", state);
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


QStringList AppConfig::removeParameterFromTable(QString removeName, QStringList table)
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


// Получение типа параметра в виде строки
QString AppConfig::getParameterTypeFromTable(QString parameterName, QStringList table)
{
  // Перебираются параметры в таблице
  for(int i=0; i<MYTETRA_CONFIG_PARAM_NUM; i++)
  {
    // Выясняется имя параметра
    QString name=table.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);

    if(name==parameterName)
      return table.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+1);
  }

  return "";
}


// Получение значения параметра в виде строки
QString AppConfig::getParameterValueFromTable(QString parameterName, QStringList table)
{
  // Перебираются параметры в таблице
  for(int i=0; i<MYTETRA_CONFIG_PARAM_NUM; i++)
  {
    // Выясняется имя параметра
    QString name=table.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);

    if(name==parameterName)
      return table.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+2);
  }

  return "";
}


// Замена типа и значения параметра
QStringList AppConfig::replaceParameterInTable(QString replaceName, QString replaceType, QString replaceValue, QStringList table)
{
  // Перебираются параметры в таблице
  for(int i=0; i<MYTETRA_CONFIG_PARAM_NUM; i++)
  {
    // Выясняется имя параметра
    QString name=table.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);

    // Если имя совпадает с заменяемым
    if(name==replaceName)
    {
      table[i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+1]=replaceType;
      table[i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+2]=replaceValue;

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

 // Последняя версия на данный момент - 22
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
 if(fromVersion<=14)
  updater.update_version(14, 15, get_parameter_table_14(), get_parameter_table_15());
 if(fromVersion<=15)
  updater.update_version(15, 16, get_parameter_table_15(), get_parameter_table_16());
 if(fromVersion<=16)
  updater.update_version(16, 17, get_parameter_table_16(), get_parameter_table_17());
 if(fromVersion<=17)
  updater.update_version(17, 18, get_parameter_table_17(), get_parameter_table_18());
 if(fromVersion<=18)
  updater.update_version(18, 19, get_parameter_table_18(), get_parameter_table_19());
 if(fromVersion<=19)
  updater.update_version(19, 20, get_parameter_table_19(), get_parameter_table_20());
 if(fromVersion<=20)
  updater.update_version(20, 21, get_parameter_table_20(), get_parameter_table_21());
 if(fromVersion<=21)
  updater.update_version(21, 22, get_parameter_table_21(), get_parameter_table_22());
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
 table=removeParameterFromTable("lastidnum", table);
 table=removeParameterFromTable("lastnotenum", table);
 table=removeParameterFromTable("lastprefixnum", table);

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


QStringList AppConfig::get_parameter_table_15(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 14
 table << get_parameter_table_14(false);

 // Новые параметры
 if(globalParameters.getTargetOs()=="android")
  table << "uglyQssReplaceHeightForTableView" << "int" << "35"; // Так как не все параметры можно стилизовать через QSS, здесь задается высота ячейки таблицы
 else
  table << "uglyQssReplaceHeightForTableView" << "int" << "0";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_16(bool withEndSignature)
{
 // Таблица параметров
 // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
 QStringList table;

 // Старые параметры, аналогичные версии 15
 table << get_parameter_table_15(false);

 // Новые параметры
 table << "recordTableShowFields" << "QString" << "name,tags";
 table << "recordTableFieldsWidth" << "QString" << "256,128";
 table << "recordTableShowHorizontalHeaders" << "bool" << "true";
 table << "recordTableShowVerticalHeaders" << "bool" << "false";

 if(withEndSignature)
  table << "0" << "0" << "0";

 return table;
}


QStringList AppConfig::get_parameter_table_17(bool withEndSignature)
{
  // Таблица параметров
  // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
  QStringList table;

  // Старые параметры, аналогичные версии 16
  table << get_parameter_table_16(false);

  // Новые параметры
  if(globalParameters.getTargetOs()=="android")
    table << "showSplashScreen" << "bool" << "true"; // В Андроид долгий запуск, нужно показывать сплешскрин
  else
    table << "showSplashScreen" << "bool" << "false"; // На десктопе быстрый запуск, сплешскрин только мешает

  if(withEndSignature)
    table << "0" << "0" << "0";

  return table;
}


QStringList AppConfig::get_parameter_table_18(bool withEndSignature)
{
  // Таблица параметров
  // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
  QStringList table;

  // Старые параметры, аналогичные версии 17
  table << get_parameter_table_17(false);

  // Новые параметры
  if(globalParameters.getTargetOs()=="android")
    table << "interfaceMode" << "QString" << "mobile"; // В Андроид должен быть мобильный интерфейс
  else
    table << "interfaceMode" << "QString" << "desktop"; // На десктопе должен быть интерфейс адоптированный для работы на рабочем столе

  if(withEndSignature)
    table << "0" << "0" << "0";

  return table;
}


QStringList AppConfig::get_parameter_table_19(bool withEndSignature)
{
  // Таблица параметров
  // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
  QStringList table;

  // Старые параметры, аналогичные версии 18
  table << get_parameter_table_18(false);

  // Новые параметры
  table << "focusWidget" << "QString" << "";

  if(withEndSignature)
    table << "0" << "0" << "0";

  return table;
}


QStringList AppConfig::get_parameter_table_20(bool withEndSignature)
{
  // Таблица параметров
  // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
  QStringList table;

  // Старые параметры, аналогичные версии 19
  table << get_parameter_table_19(false);

  // Новые параметры
  if(globalParameters.getTargetOs()=="android")
    table << "hideEditorTools" << "QString" << "italic,underline,monospace,alignleft,aligncenter,alignright,alignwidth,numericlist,dotlist,indentplus,indentminus,showformatting,showhtml,fontcolor,expand_edit_area,save,createtable,table_add_row,table_remove_row,table_add_col,table_remove_col,table_merge_cells,table_split_cell"; // В Андроид прячутся инструменты сложного форматирования текста
  else
    table << "hideEditorTools" << "QString" << ""; // На десктопе скрываемых кнопок редактора нет

  if(withEndSignature)
    table << "0" << "0" << "0";

  return table;
}


QStringList AppConfig::get_parameter_table_21(bool withEndSignature)
{
  // Таблица параметров
  // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
  QStringList table;

  // Старые параметры, аналогичные версии 20
  table << get_parameter_table_20(false);

  table << "findInBaseExpand" << "bool" << "true";
  if(withEndSignature)
    table << "0" << "0" << "0";

  return table;
}


QStringList AppConfig::get_parameter_table_22(bool withEndSignature)
{
  // Таблица параметров
  // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
  QStringList table;

  // Старые параметры, аналогичные версии 21
  table << get_parameter_table_21(false);

  table << "recordtableSelectedRecordId" << "QString" << "";
  if(withEndSignature)
    table << "0" << "0" << "0";

  return table;
}
