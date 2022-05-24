#include <functional>
#include <QRect>

#include "main.h"
#include "AppConfig.h"

#include "libraries/GlobalParameters.h"
#include "libraries/helpers/DebugHelper.h"

extern GlobalParameters globalParameters;


// Конструктор объекта настройки программы
AppConfig::AppConfig(QObject *pobj)
{
    Q_UNUSED(pobj)

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
    {
        criticalError("File "+configFileName+" not found.");
    }

    // Создается указатель на объект хранилища конфигурации
    conf=new QSettings(configFileName, QSettings::IniFormat, this);

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


// Получение имени файла конфига, с которым происходит работа
QString AppConfig::getConfigFileName()
{
    return conf->fileName();
}


// Получение параметра по имени в виде строки с проверкой его существования
QString AppConfig::get_parameter(QString name)
{
    if(conf->contains(name))
    {
        return conf->value(name).toString();
    }
    else
    {
        criticalError("In config not found parameter " + name);
    }
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
    {
        return false;
    }
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
    {
        return false;
    }
}


// Получение имени директории с корзиной
QString AppConfig::get_trashdir(void)
{
    return get_parameter("trashdir");
}


// Получение максимального размера директории корзины в мегабайтах
unsigned int AppConfig::get_trashsize(void)
{
    return get_parameter("trashsize").toUInt();
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
    {
        return false;
    }
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
    {
        return false;
    }
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
    {
        conf->setValue("addnewrecord_expand_info",state);
    }
    else
    {
        criticalError("Set unavailable value for addnewrecord_expand_info "+state);
    }
}


QByteArray AppConfig::get_mainwingeometry(void)
{
    return QByteArray::fromBase64( conf->value("mainwingeometry", "").toString().toLatin1() );
}


void AppConfig::set_mainwingeometry(QByteArray dataGeometry)
{
    qDebug() << "Save main window geometry";

    conf->setValue( "mainwingeometry", QString( dataGeometry.toBase64().data() ) );
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
    {
        list.append( line_list.at(i).toInt() );
    }

    return list;
}


void AppConfig::set_splitter_size_list(QString name, QList<int> list)
{
    QStringList line_list;

    for(int i=0;i < list.size(); ++i)
    {
        line_list.append( QString::number(list.at(i)) );
    }

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
    {
        conf->setValue("howpassrequest", mode);
    }
    else
    {
        criticalError("Set unavailable value for howpassrequest "+mode);
    }
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


bool AppConfig::getSynchroConsoleDetails(void)
{
    return conf->value("synchroConsoleDetails").toBool();
}


void AppConfig::setSynchroConsoleDetails(bool flag)
{
    conf->setValue("synchroConsoleDetails", flag);
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
    {
        fields << "name";
    }

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
    {
        fields[0]="256";
    }

    // Не должно быть так, чтобы был пустой список
    if(fields.size()==0)
    {
        fields << "256";
    }

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



// Разрешено ли использовать собственный формат вывода даты и времени
bool AppConfig::getEnableCustomDateTimeFormat(void)
{
    return conf->value("enableCustomDateTimeFormat").toBool();
}

void AppConfig::setEnableCustomDateTimeFormat(bool state)
{
    conf->setValue("enableCustomDateTimeFormat", state);
}


// Строка собственного формата вывода даты и времени
QString AppConfig::getCustomDateTimeFormat(void)
{
    return get_parameter("customDateTimeFormat");
}

void AppConfig::setCustomDateTimeFormat(QString format)
{
    conf->setValue("customDateTimeFormat", format);
}


// Путь на диске по которому пользователь открывал файлы чтобы приаттачить их к записи
QString AppConfig::getAttachAppendDir(void)
{
    return get_parameter("attachAppendDir");
}

void AppConfig::setAttachAppendDir(QString dir)
{
    conf->setValue("attachAppendDir", dir);
}


// Путь на диске по которому пользователь сохранял (Сохранить как...)приаттаченные файлы
QString AppConfig::getAttachSaveAsDir(void)
{
    return get_parameter("attachSaveAsDir");
}

void AppConfig::setAttachSaveAsDir(QString dir)
{
    conf->setValue("attachSaveAsDir", dir);
}


// Разрешать ли для просмотра расшифровывать зашифрованные файлы в директорию корзины MyTetra
bool AppConfig::getEnableDecryptFileToTrashDirectory(void)
{
    return conf->value("enableDecryptFileToTrashDirectory").toBool();
}

void AppConfig::setEnableDecryptFileToTrashDirectory(bool state)
{
    conf->setValue("enableDecryptFileToTrashDirectory", state);
}


// Получение размера файла лога действий
unsigned int AppConfig::getActionLogMaximumSize(void)
{
    return get_parameter("actionLogMaximumSize").toUInt();
}


// Установка размера файла лога действий в мегабайтах
void AppConfig::setActionLogMaximumSize(unsigned int mbSize)
{
    conf->setValue("actionLogMaximumSize", mbSize);
}


// Разрешено ли логирование
bool AppConfig::getEnableLogging(void)
{
    return conf->value("enableLogging").toBool();
}

void AppConfig::setEnableLogging(bool state)
{
    conf->setValue("enableLogging", state);
}

// Разрешена ли подсветка записей с прикрепленными файлами
bool AppConfig::getEnableRecordWithAttachHighlight(void)
{
    return conf->value("enableRecordWithAttachHighlight").toBool();
}

void AppConfig::setEnableRecordWithAttachHighlight(bool state)
{
    conf->setValue("enableRecordWithAttachHighlight", state);
}


// Цвет подсветки записей с прикрепленными файлами
QString AppConfig::getRecordWithAttachHighlightColor(void)
{
    return get_parameter("recordWithAttachHighlightColor");
}


void AppConfig::setRecordWithAttachHighlightColor(QString color)
{
    QColor saveColor(color);

    // Если сохраняема строка действительно содержит закодированный цвет
    if(saveColor.isValid())
    {
        conf->setValue("recordWithAttachHighlightColor", color);
    }
}


// Разрешена ли периодическая проверка файла базы на предмет изменения сторонней программой
bool AppConfig::getEnablePeriodicCheckBase(void)
{
    return conf->value("enablePeriodicCheckBase").toBool();
}

void AppConfig::setEnablePeriodicCheckBase(bool state)
{
    conf->setValue("enablePeriodicCheckBase", state);
}


// Период проверки файла базы на предмет изменения сторонней программой
int AppConfig::getCheckBasePeriod(void)
{
    return get_parameter("checkBasePeriod").toInt();
}


void AppConfig::setCheckBasePeriod(int period)
{
    conf->setValue("checkBasePeriod", period);
}


// Необходимо ли выводить сообщение если база была изменена другой программой
bool AppConfig::getEnablePeriodicCheckMessage(void)
{
    return conf->value("enablePeriodicCheckMessage").toBool();
}

void AppConfig::setEnablePeriodicCheckMessage(bool state)
{
    conf->setValue("enablePeriodicCheckMessage", state);
}


int AppConfig::getPreviewIconSize(void)
{
    return conf->value("previewIconSize",0).toInt();
}


void AppConfig::setPreviewIconSize(int n)
{
    conf->setValue("previewIconSize", n);
}


QString AppConfig::getIconCurrentSectionName(void)
{
    return get_parameter("iconCurrentSectionName");
}

void AppConfig::setIconCurrentSectionName(QString name)
{
    conf->setValue("iconCurrentSectionName", name);
}



// Разрешена ли периодическая синхронизация
bool AppConfig::getEnablePeriodicSynchro(void)
{
    return conf->value("enablePeriodicSynchro").toBool();
}

void AppConfig::setEnablePeriodicSynchro(bool state)
{
    conf->setValue("enablePeriodicSynchro", state);
}


// Период автоматической периодической синхронизации
int AppConfig::getPeriodicSynchroPeriod(void)
{
    return get_parameter("periodicSynchroPeriod").toInt();
}


void AppConfig::setPeriodicSynchroPeriod(int period)
{
    conf->setValue("periodicSynchroPeriod", period);
}


// Разрешено ли создавать запись без текста
bool AppConfig::getEnableCreateEmptyRecord(void)
{
    return conf->value("enableCreateEmptyRecord").toBool();
}

void AppConfig::setEnableCreateEmptyRecord(bool state)
{
    conf->setValue("enableCreateEmptyRecord", state);
}


QString AppConfig::getDockableWindowsState(void)
{
    return get_parameter("dockableWindowsState");
}

void AppConfig::setDockableWindowsState(QString state)
{
    conf->setValue("dockableWindowsState", state);
}


QString AppConfig::getDockableWindowsBehavior(void)
{
    return get_parameter("dockableWindowsBehavior");
}

void AppConfig::setDockableWindowsBehavior(QString mode)
{
    if(mode=="single" || mode=="together")
    {
        conf->setValue("dockableWindowsBehavior", mode);
    }
    else
    {
        criticalError("Set unavailable value for dockableWindowsBehavior "+mode);
    }
}


// ------------------------------------
// Методы для обновления версии конфига
// ------------------------------------

void AppConfig::add_default_param(const QString name, const QVariant value) {
    if (!conf->contains(name))
        conf->setValue(name, value);
}

void AppConfig::set_version_1_default_params()
{
    add_default_param("addnewrecord_expand_info" , 1);
    add_default_param("findscreen_find_inname"   , true);
    add_default_param("findscreen_find_intags"   , true);
    add_default_param("findscreen_find_intext"   , true);
    add_default_param("findscreen_howextract"    , 1);
    add_default_param("findscreen_show"          , false);
    add_default_param("findscreen_wordregard"    , 1);
    add_default_param("findsplitter_size_list"   , "517,141");
    add_default_param("hspl_size_list"           , "237,621");
    add_default_param("lastidnum"                , 3537);
    add_default_param("lastnotenum"              , 3119);
    add_default_param("lastprefixnum"            , 7540);
    add_default_param("mainwingeometry"          , "155,24,864,711)");
    add_default_param("recordtable_position"     , 0);
    add_default_param("tetradir"                 , "/opt/mytetra/data");
    add_default_param("trashdir"                 , "/opt/mytetra/trash");
    add_default_param("trashmaxfilecount"        , 200);
    add_default_param("trashsize"                , 5);
    add_default_param("tree_position"            , "0,1818,1819");
    add_default_param("vspl_size_list"           , "171,487");
    add_default_param("findscreen_find_inauthor" , true);
    add_default_param("findscreen_find_inurl"    , false);
}


void AppConfig::update_version_process(void)
{
    switch(conf->value("version").toInt()) {
        case 1:  set_version_1_default_params(); [[fallthrough]];
        case 2:  add_default_param("cutbranchconfirm", true); [[fallthrough]];
        case 3:  add_default_param("printdebugmessages", false); [[fallthrough]];
        case 4:  add_default_param("interfacelanguage", "en");
                 add_default_param("programm", "mytetra"); [[fallthrough]];
        case 5:  conf->remove("lastidnum");
                 conf->remove("lastnotenum");
                 conf->remove("lastprefixnum"); [[fallthrough]];
        case 6:  add_default_param("howpassrequest", "atClickOnCryptBranch"); [[fallthrough]];
        case 7:  add_default_param("runinminimizedwindow", false); [[fallthrough]];
        case 8:  add_default_param("synchrocommand", "");
                 add_default_param("synchroonstartup", false);
                 add_default_param("synchroonexit", false); [[fallthrough]];
        case 9:  add_default_param("syncroConsoleDetails", false); [[fallthrough]];
        case 10: add_default_param("autoClosePasswordEnable", false);
                 add_default_param("autoClosePasswordDelay", 10); [[fallthrough]];
        case 11: add_default_param("editorCursorPosition", 0);
                 add_default_param("editorScrollBarPosition", 0); [[fallthrough]];
        case 12: add_default_param("passwordMiddleHash", "");
                 add_default_param("passwordSaveFlag", false); [[fallthrough]];
        case 13: add_default_param("rememberCursorAtHistoryNavigation", true);
                 add_default_param("rememberCursorAtOrdinarySelection", true); [[fallthrough]];
        case 14: add_default_param("findScreenTreeSearchArea", 0); [[fallthrough]];
        case 15: // Так как не все параметры можно стилизовать через QSS, здесь задается высота ячейки таблицы
                 add_default_param("uglyQssReplaceHeightForTableView", globalParameters.getTargetOs()=="android" ? 35 : 0); [[fallthrough]];
        case 16: add_default_param("recordTableShowFields", "name,tags");
                 add_default_param("recordTableFieldsWidth", "256,128");
                 add_default_param("recordTableShowHorizontalHeaders", true);
                 add_default_param("recordTableShowVerticalHeaders", false); [[fallthrough]];
        case 17: // В Android долгий запуск, нужно показывать сплешскрин, а на десктопе быстрый запуск, сплешскрин только мешает
                 add_default_param("showSplashScreen", globalParameters.getTargetOs()=="android" ? true : false); [[fallthrough]];
        case 18: add_default_param("interfaceMode", globalParameters.getTargetOs()=="android" ? "mobile" : "desktop"); [[fallthrough]];
        case 19: add_default_param("focusWidget", ""); [[fallthrough]];
        case 20: {
                    // В Android прячутся инструменты сложного форматирования текста
                    QString a = "italic,underline,monospace,alignleft,aligncenter,alignright,alignwidth,"
                                 "numericlist,dotlist,indentplus,indentminus,showformatting,showhtml,fontcolor,"
                                 "backgroundcolor,expand_edit_area,save,createtable,table_add_row,table_remove_row,"
                                 "table_add_col,table_remove_col,table_merge_cells,table_split_cell";
                    add_default_param("hideEditorTools", globalParameters.getTargetOs()=="android" ? a : "");
                    [[fallthrough]];
                 }
        case 21: add_default_param("findInBaseExpand", true); [[fallthrough]];
        case 22: add_default_param("recordtableSelectedRecordId", ""); [[fallthrough]];
        case 23: conf->remove("recordtable_position"); [[fallthrough]];
        case 24: add_default_param("enableCustomDateTimeFormat", false);
                 add_default_param("customDateTimeFormat", ""); [[fallthrough]];
        case 25: add_default_param("attachAppendDir", "");
                 add_default_param("attachSaveAsDir", ""); [[fallthrough]];
        case 26: add_default_param("enableDecryptFileToTrashDirectory", false); [[fallthrough]];
        case 27: add_default_param("actionLogMaximumSize", 1/*Мб*/); [[fallthrough]];
        case 28: add_default_param("enableLogging", false); [[fallthrough]];
        case 29: add_default_param("enableRecordWithAttachHighlight", true);
                 add_default_param("recordWithAttachHighlightColor", "#d1e3c5"); [[fallthrough]];
        case 30: add_default_param("enablePeriodicCheckBase", false);
                 add_default_param("checkBasePeriod", 20);
                 add_default_param("enablePeriodicCheckMessage", false); [[fallthrough]];
        case 31: add_default_param("previewIconSize", 24); [[fallthrough]];
        case 32: add_default_param("iconCurrentSectionName", "Essential"); [[fallthrough]];
        case 33: add_default_param("enablePeriodicSyncro", false);
                 add_default_param("periodicSyncroPeriod", 300); [[fallthrough]];
        case 34: add_default_param("enableCreateEmptyRecord", false); [[fallthrough]];
        case 35: add_default_param("findscreen_find_innameItem", false); [[fallthrough]];
        case 36: // Предполагаемый формат - "IDзаписи1,x1,y1,w1,h1;IDзаписи2,x2,y2,w2,h2..."
                 add_default_param("dockableWindowsState", ""); [[fallthrough]];
        case 37: add_default_param("dockableWindowsBehavior", "single"); [[fallthrough]];
        case 38: {
                    auto val = conf->value("dockableWindowsState").toString();
                    if(val.trimmed().size() > 1)
                    {
                        auto chunks = val.split(';');
                        for(auto & c : chunks)
                            c += ",0";

                        val = chunks.join(';');
                        conf->setValue("dockableWindowsState", val);
                    }
                    [[fallthrough]];
                 }
        case 39: {
                    // rename '*syncro*' to '*synchro*'
                    auto pv = conf->value("enablePeriodicSyncro");
                    conf->remove("enablePeriodicSyncro");
                    add_default_param("enablePeriodicSynchro", pv);

                    pv = conf->value("periodicSyncroPeriod");
                    conf->remove("periodicSyncroPeriod");
                    add_default_param("periodicSynchroPeriod", pv);

                    pv = conf->value("syncroConsoleDetails");
                    conf->remove("syncroConsoleDetails");
                    add_default_param("synchroConsoleDetails", pv);
                }
        } // end big switch

    // Устанавливается новый номер версии
    conf->setValue("version", 39);

    // Конфигурация записывается на диск
    conf->sync();
}
