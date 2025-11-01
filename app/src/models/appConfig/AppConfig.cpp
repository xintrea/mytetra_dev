#include <functional>
#include <QRect>
#include <QTextCodec>

#include "main.h"
#include "AppConfig.h"
#include "AppConfigUpdater.h"

#include "libraries/GlobalParameters.h"
#include "libraries/FixedParameters.h"
#include "libraries/helpers/DebugHelper.h"

extern GlobalParameters globalParameters;
extern FixedParameters fixedParameters;


// Конструктор объекта настройки программы
AppConfig::AppConfig(QObject *pobj)
{
    Q_UNUSED(pobj)

    m_isInit=false;
}


// Деструктор объекта настройки программы
AppConfig::~AppConfig()
{
    if (m_isInit)
    {
        qDebug() << "Save mytetra config file";
        m_conf->sync();
    }
}


void AppConfig::init(void)
{
    // Создается имя файла конфигурации
    QString configFileName = globalParameters.getWorkDirectory()+"/conf.ini";

    // Проверяется, есть ли файл конфигурации
    QFile confFile(configFileName);
    if (!confFile.exists())
    {
        criticalError("File "+configFileName+" not found.");
    }

    // Создается указатель на объект хранилища конфигурации
    m_conf = new QSettings(configFileName, QSettings::IniFormat, this);
    m_conf->setIniCodec( QTextCodec::codecForName("UTF-8") );

    update_version_process();

    sync();

    m_isInit = true;
}


bool AppConfig::is_init(void)
{
    return m_isInit;
}


void AppConfig::sync(void)
{
    m_conf->sync();
}


// Получение имени файла конфига, с которым происходит работа
QString AppConfig::getConfigFileName()
{
    return m_conf->fileName();
}


// Получение параметра по имени в виде строки с проверкой его существования
QString AppConfig::get_parameter(QString name)
{
    if (m_conf->contains(name))
    {
        return m_conf->value(name).toString();
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

    if (directory.exists() && directory.isReadable())
    {
        m_conf->setValue("tetradir", dirName);
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
    return this->get_parameter("tetradir");
}


// Установка имени директории с корзиной
bool AppConfig::set_trashdir(QString dirName)
{
    QDir directory(dirName);

    if (directory.exists() && directory.isReadable())
    {
        m_conf->setValue("trashdir", dirName);
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
    return this->get_parameter("trashdir");
}


// Получение максимального размера директории корзины в мегабайтах
unsigned int AppConfig::get_trashsize(void)
{
    return this->get_parameter("trashsize").toUInt();
}


// Установка максимального размера директории корзины в мегабайтах
bool AppConfig::set_trashsize(unsigned int mbSize)
{
    if (mbSize>0)
    {
        m_conf->setValue("trashsize", mbSize);
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
    return this->get_parameter("trashmaxfilecount").toInt();
}


// Установка максимально допустимого числа файлов в корзине
bool AppConfig::set_trashmaxfilecount(int count)
{
    if (count>0)
    {
        m_conf->setValue("trashmaxfilecount", count);
        return true;
    }
    else
    {
        return false;
    }
}


bool AppConfig::get_cutbranchconfirm(void)
{
    return m_conf->value("cutbranchconfirm").toBool();
}


bool AppConfig::set_cutbranchconfirm(bool confirm)
{
    m_conf->setValue("cutbranchconfirm", confirm);
    return true;
}


bool AppConfig::get_printdebugmessages(void)
{
    return m_conf->value("printdebugmessages").toBool();
}


bool AppConfig::set_printdebugmessages(bool isPrint)
{
    m_conf->setValue("printdebugmessages", isPrint);
    return true;
}


QString AppConfig::get_interfacelanguage(void)
{
    QString language = this->get_parameter("interfacelanguage");
    qDebug() << "Get interface language" << language;
    return language;
}


bool AppConfig::set_interfacelanguage(QString language)
{
    m_conf->setValue("interfacelanguage", language);
    return true;
}


QString AppConfig::get_addnewrecord_expand_info(void)
{
    return this->get_parameter("addnewrecord_expand_info");
}


void AppConfig::set_addnewrecord_expand_info(QString state)
{
    if (state=="0" || state=="1")
    {
        m_conf->setValue("addnewrecord_expand_info", state);
    }
    else
    {
        criticalError("Set unavailable value for addnewrecord_expand_info "+state);
    }
}


QByteArray AppConfig::get_mainwingeometry(void)
{
    return QByteArray::fromBase64( m_conf->value("mainwingeometry", "").toString().toLatin1() );
}


void AppConfig::set_mainwingeometry(QByteArray dataGeometry)
{
    qDebug() << "Save main window geometry";

    m_conf->setValue( "mainwingeometry", QString( dataGeometry.toBase64().data() ) );
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
    // qDebug() << "Config set find splitter list to " << list;
    set_splitter_size_list("findsplitter", list);
}


QList<int> AppConfig::get_splitter_size_list(QString name)
{
    QStringList line_list;
    QList<int> list;

    line_list = (m_conf->value(name+"_size_list", "100,100")).toString().split(",");

    for (int i=0;i < line_list.size(); ++i)
    {
        list.append( line_list.at(i).toInt() );
    }

    return list;
}


void AppConfig::set_splitter_size_list(QString name, QList<int> list)
{
    QStringList line_list;

    for (int i=0;i < list.size(); ++i)
    {
        line_list.append( QString::number(list.at(i)) );
    }

    m_conf->setValue(name+"_size_list", line_list.join(","));
}


QStringList AppConfig::get_tree_position(void)
{
    return (m_conf->value("tree_position","1")).toString().split(",");
}


void AppConfig::set_tree_position(QStringList list)
{
    qDebug() << "AppConfig::set_tree_position() : " << list;
    m_conf->setValue("tree_position", list.join(","));
}


// ID записи в таблице конечных записей, которая выделена на экране
QString AppConfig::get_recordtable_selected_record_id(void)
{
    return m_conf->value("recordtableSelectedRecordId", 0).toString();
}


// ID записи в таблице конечных записей, которая выделена на экране
void AppConfig::set_recordtable_selected_record_id(QString id)
{
    m_conf->setValue("recordtableSelectedRecordId", id);
}


int AppConfig::get_findscreen_wordregard(void)
{
    return m_conf->value("findscreen_wordregard", 0).toInt();
}


void AppConfig::set_findscreen_wordregard(int pos)
{
    m_conf->setValue("findscreen_wordregard",pos);
}


int AppConfig::get_findscreen_howextract(void)
{
    return m_conf->value("findscreen_howextract", 0).toInt();
}


void AppConfig::set_findscreen_howextract(int pos)
{
    m_conf->setValue("findscreen_howextract", pos);
}


int AppConfig::getFindScreenTreeSearchArea(void)
{
    return m_conf->value("findScreenTreeSearchArea", 0).toInt();
}


void AppConfig::setFindScreenTreeSearchArea(int pos)
{
    m_conf->setValue("findScreenTreeSearchArea", pos);
}


bool AppConfig::get_findscreen_find_in_field(QString fieldName)
{
    return m_conf->value("findscreen_find_in"+fieldName, 0).toBool();
}


void AppConfig::set_findscreen_find_in_field(QString fieldName, bool isChecked)
{
    m_conf->setValue("findscreen_find_in"+fieldName, isChecked);
}


bool AppConfig::get_findscreen_show(void)
{
    return m_conf->value("findscreen_show", 0).toBool();
}


void AppConfig::set_findscreen_show(bool isShow)
{
    m_conf->setValue("findscreen_show", isShow);
}


QString AppConfig::get_howpassrequest(void)
{
    return this->get_parameter("howpassrequest");
}


void AppConfig::set_howpassrequest(QString mode)
{
    if (mode=="atClickOnCryptBranch" || mode=="atStartProgram")
    {
        m_conf->setValue("howpassrequest", mode);
    }
    else
    {
        criticalError("Set unavailable value for howpassrequest "+mode);
    }
}


bool AppConfig::get_autoClosePasswordEnable(void)
{
    return m_conf->value("autoClosePasswordEnable", 0).toBool();
}


void AppConfig::set_autoClosePasswordEnable(bool flag)
{
    m_conf->setValue("autoClosePasswordEnable", flag);
}


int AppConfig::get_autoClosePasswordDelay(void)
{
    return m_conf->value("autoClosePasswordDelay", 0).toInt();
}


void AppConfig::set_autoClosePasswordDelay(int delay)
{
    m_conf->setValue("autoClosePasswordDelay", delay);
}


bool AppConfig::get_runinminimizedwindow(void)
{
    return m_conf->value("runinminimizedwindow").toBool();
}


void AppConfig::set_runinminimizedwindow(bool flag)
{
    m_conf->setValue("runinminimizedwindow", flag);
}


QString AppConfig::get_synchrocommand(void)
{
    return this->get_parameter("synchrocommand");
}


void AppConfig::set_synchrocommand(QString command)
{
    m_conf->setValue("synchrocommand", command);
}


bool AppConfig::get_synchroonstartup(void)
{
    return m_conf->value("synchroonstartup").toBool();
}


void AppConfig::set_synchroonstartup(bool flag)
{
    m_conf->setValue("synchroonstartup", flag);
}


bool AppConfig::get_synchroonexit(void)
{
    return m_conf->value("synchroonexit").toBool();
}


void AppConfig::set_synchroonexit(bool flag)
{
    m_conf->setValue("synchroonexit", flag);
}


bool AppConfig::getSyncroConsoleDetails(void)
{
    return m_conf->value("syncroConsoleDetails").toBool();
}


void AppConfig::setSyncroConsoleDetails(bool flag)
{
    m_conf->setValue("syncroConsoleDetails", flag);
}


int AppConfig::getEditorCursorPosition(void)
{
    return m_conf->value("editorCursorPosition",0).toInt();
}


void AppConfig::setEditorCursorPosition(int n)
{
    m_conf->setValue("editorCursorPosition", n);
}


int AppConfig::getEditorScrollBarPosition(void)
{
    return m_conf->value("editorScrollBarPosition",0).toInt();
}


void AppConfig::setEditorScrollBarPosition(int n)
{
    m_conf->setValue("editorScrollBarPosition", n);
}


QString AppConfig::getPasswordMiddleHash(void)
{
    return this->get_parameter("passwordMiddleHash");
}


void AppConfig::setPasswordMiddleHash(QString hash)
{
    m_conf->setValue("passwordMiddleHash", hash);
}


// Нужно ли локально хранить пароль (точнее, промежуточный хеш пароля)
bool AppConfig::getPasswordSaveFlag(void)
{
    return m_conf->value("passwordSaveFlag").toBool();
}


void AppConfig::setPasswordSaveFlag(bool flag)
{
    m_conf->setValue("passwordSaveFlag", flag);
}


bool AppConfig::getRememberCursorAtHistoryNavigation(void)
{
    return m_conf->value("rememberCursorAtHistoryNavigation").toBool();
}


void AppConfig::setRememberCursorAtHistoryNavigation(bool flag)
{
    m_conf->setValue("rememberCursorAtHistoryNavigation", flag);
}


bool AppConfig::getRememberCursorAtOrdinarySelection(void)
{
    return m_conf->value("rememberCursorAtOrdinarySelection").toBool();
}


void AppConfig::setRememberCursorAtOrdinarySelection(bool flag)
{
    m_conf->setValue("rememberCursorAtOrdinarySelection", flag);
}


int AppConfig::getUglyQssReplaceHeightForTableView(void)
{
    return m_conf->value("uglyQssReplaceHeightForTableView", 0).toInt();
}

void AppConfig::setUglyQssReplaceHeightForTableView(int n)
{
    m_conf->setValue("uglyQssReplaceHeightForTableView", n);
}


// Перечень полей, отображаемых в таблице конечных записей
QStringList AppConfig::getRecordTableShowFields(void)
{
    return (m_conf->value("recordTableShowFields", "name")).toString().split(",");
}


void AppConfig::setRecordTableShowFields(QStringList fields)
{
    // Не должно быть так, чтобы был пустой список
    if (fields.size()==0)
    {
        fields << "name";
    }

    m_conf->setValue("recordTableShowFields", fields.join(","));
}


bool AppConfig::getRecordTableShowHorizontalHeaders(void)
{
    return m_conf->value("recordTableShowHorizontalHeaders").toBool();
}


void AppConfig::setRecordTableShowHorizontalHeaders(bool flag)
{
    m_conf->setValue("recordTableShowHorizontalHeaders", flag);
}


bool AppConfig::getRecordTableShowVerticalHeaders(void)
{
    return m_conf->value("recordTableShowVerticalHeaders").toBool();
}


void AppConfig::setRecordTableShowVerticalHeaders(bool flag)
{
    m_conf->setValue("recordTableShowVerticalHeaders", flag);
}


// Ширина полей, отображаемых в таблице конечных записей
QStringList AppConfig::getRecordTableFieldsWidth(void)
{
    return (m_conf->value("recordTableFieldsWidth", "256")).toString().split(",");
}


void AppConfig::setRecordTableFieldsWidth(QStringList fields)
{
    // В списке с одним элементом должна стоять стандартная величина (всеравно она не влияет, и начнет влиять только после появления второй колонки)
    if (fields.size()==1)
    {
        fields[0]="256";
    }

    // Не должно быть так, чтобы был пустой список
    if (fields.size()==0)
    {
        fields << "256";
    }

    m_conf->setValue("recordTableFieldsWidth", fields.join(","));
}


// Показывать ли сплешскрин при старте программы
bool AppConfig::getShowSplashScreen(void)
{
    return m_conf->value("showSplashScreen").toBool();
}


void AppConfig::setShowSplashScreen(bool isShow)
{
    m_conf->setValue("showSplashScreen", isShow);
}


// Режим интерфейса, возможные значения "desktop" и "mobile"
QString AppConfig::getInterfaceMode(void)
{
    QString mode = this->get_parameter("interfaceMode");
    return mode;
}


void AppConfig::setInterfaceMode(QString mode)
{
    m_conf->setValue("interfaceMode", mode);
}

// Имя последнего активного виджета
QString AppConfig::getFocusWidget(void)
{
    QString widgetName = this->get_parameter("focusWidget");
    return widgetName;
}


void AppConfig::setFocusWidget(QString widgetName)
{
    m_conf->setValue("focusWidget", widgetName);
}


QStringList AppConfig::getHideEditorTools(void)
{
    return (m_conf->value("hideEditorTools", "")).toString().split(",");
}


void AppConfig::setHideEditorTools(QStringList toolsNames)
{
    m_conf->setValue("hideEditorTools", toolsNames.join(","));
}


bool AppConfig::getFindInBaseExpand(void)
{
    return m_conf->value("findInBaseExpand").toBool();
}


void AppConfig::setFindInBaseExpand(bool state)
{
    m_conf->setValue("findInBaseExpand", state);
}



// Разрешено ли использовать собственный формат вывода даты и времени
bool AppConfig::getEnableCustomDateTimeFormat(void)
{
    return m_conf->value("enableCustomDateTimeFormat").toBool();
}

void AppConfig::setEnableCustomDateTimeFormat(bool state)
{
    m_conf->setValue("enableCustomDateTimeFormat", state);
}


// Строка собственного формата вывода даты и времени
QString AppConfig::getCustomDateTimeFormat(void)
{
    return this->get_parameter("customDateTimeFormat");
}

void AppConfig::setCustomDateTimeFormat(QString format)
{
    m_conf->setValue("customDateTimeFormat", format);
}


// Путь на диске по которому пользователь открывал файлы чтобы приаттачить их к записи
QString AppConfig::getAttachAppendDir(void)
{
    return this->get_parameter("attachAppendDir");
}

void AppConfig::setAttachAppendDir(QString dir)
{
    m_conf->setValue("attachAppendDir", dir);
}


// Путь на диске по которому пользователь сохранял (Сохранить как...)приаттаченные файлы
QString AppConfig::getAttachSaveAsDir(void)
{
    return this->get_parameter("attachSaveAsDir");
}

void AppConfig::setAttachSaveAsDir(QString dir)
{
    m_conf->setValue("attachSaveAsDir", dir);
}


// Разрешать ли для просмотра расшифровывать зашифрованные файлы в директорию корзины MyTetra
bool AppConfig::getEnableDecryptFileToTrashDirectory(void)
{
    return m_conf->value("enableDecryptFileToTrashDirectory").toBool();
}

void AppConfig::setEnableDecryptFileToTrashDirectory(bool state)
{
    m_conf->setValue("enableDecryptFileToTrashDirectory", state);
}


// Получение размера файла лога действий
unsigned int AppConfig::getActionLogMaximumSize(void)
{
    return this->get_parameter("actionLogMaximumSize").toUInt();
}


// Установка размера файла лога действий в мегабайтах
void AppConfig::setActionLogMaximumSize(unsigned int mbSize)
{
    m_conf->setValue("actionLogMaximumSize", mbSize);
}


// Разрешено ли логирование
bool AppConfig::getEnableLogging(void)
{
    return m_conf->value("enableLogging").toBool();
}

void AppConfig::setEnableLogging(bool state)
{
    m_conf->setValue("enableLogging", state);
}

// Разрешена ли подсветка записей с прикрепленными файлами
bool AppConfig::getEnableRecordWithAttachHighlight(void)
{
    return m_conf->value("enableRecordWithAttachHighlight").toBool();
}

void AppConfig::setEnableRecordWithAttachHighlight(bool state)
{
    m_conf->setValue("enableRecordWithAttachHighlight", state);
}


// Цвет подсветки записей с прикрепленными файлами
QString AppConfig::getRecordWithAttachHighlightColor(void)
{
    return this->get_parameter("recordWithAttachHighlightColor");
}


void AppConfig::setRecordWithAttachHighlightColor(QString color)
{
    QColor saveColor(color);

    // Если сохраняема строка действительно содержит закодированный цвет
    if (saveColor.isValid())
    {
        m_conf->setValue("recordWithAttachHighlightColor", color);
    }
}


// Разрешена ли периодическая проверка файла базы на предмет изменения сторонней программой
bool AppConfig::getEnablePeriodicCheckBase(void)
{
    return m_conf->value("enablePeriodicCheckBase").toBool();
}

void AppConfig::setEnablePeriodicCheckBase(bool state)
{
    m_conf->setValue("enablePeriodicCheckBase", state);
}


// Период проверки файла базы на предмет изменения сторонней программой
int AppConfig::getCheckBasePeriod(void)
{
    return this->get_parameter("checkBasePeriod").toInt();
}


void AppConfig::setCheckBasePeriod(int period)
{
    m_conf->setValue("checkBasePeriod", period);
}


// Необходимо ли выводить сообщение если база была изменена другой программой
bool AppConfig::getEnablePeriodicCheckMessage(void)
{
    return m_conf->value("enablePeriodicCheckMessage").toBool();
}

void AppConfig::setEnablePeriodicCheckMessage(bool state)
{
    m_conf->setValue("enablePeriodicCheckMessage", state);
}


int AppConfig::getPreviewIconSize(void)
{
    return m_conf->value("previewIconSize",0).toInt();
}


void AppConfig::setPreviewIconSize(int n)
{
    m_conf->setValue("previewIconSize", n);
}


QString AppConfig::getIconCurrentSectionName(void)
{
    return this->get_parameter("iconCurrentSectionName");
}

void AppConfig::setIconCurrentSectionName(QString name)
{
    m_conf->setValue("iconCurrentSectionName", name);
}



// Разрешена ли периодическая синхронизация
bool AppConfig::getEnablePeriodicSyncro(void)
{
    return m_conf->value("enablePeriodicSyncro").toBool();
}

void AppConfig::setEnablePeriodicSyncro(bool state)
{
    m_conf->setValue("enablePeriodicSyncro", state);
}


// Период автоматической периодической синхронизации
int AppConfig::getPeriodicSyncroPeriod(void)
{
    return this->get_parameter("periodicSyncroPeriod").toInt();
}


void AppConfig::setPeriodicSyncroPeriod(int period)
{
    m_conf->setValue("periodicSyncroPeriod", period);
}


// Разрешено ли создавать запись без текста
bool AppConfig::getEnableCreateEmptyRecord(void)
{
    return m_conf->value("enableCreateEmptyRecord").toBool();
}

void AppConfig::setEnableCreateEmptyRecord(bool state)
{
    m_conf->setValue("enableCreateEmptyRecord", state);
}


QString AppConfig::getDockableWindowsState(void)
{
    return this->get_parameter("dockableWindowsState");
}

void AppConfig::setDockableWindowsState(QString state)
{
    m_conf->setValue("dockableWindowsState", state);
}


QString AppConfig::getDockableWindowsBehavior(void)
{
    return this->get_parameter("dockableWindowsBehavior");
}

void AppConfig::setDockableWindowsBehavior(QString mode)
{
    if (mode=="single" || mode=="together")
    {
        m_conf->setValue("dockableWindowsBehavior", mode);
    }
    else
    {
        criticalError("Unavailable value for dockableWindowsBehavior "+mode);
    }
}


// Получение имени темы оформления
QString AppConfig::getInterfaceTheme()
{
    return this->get_parameter("interfaceTheme");
}

bool AppConfig::setInterfaceTheme(QString themeName)
{
    if ( !fixedParameters.themesAvailableList.contains(themeName) )
    {
        return false;
    }

    m_conf->setValue("interfaceTheme", themeName);

    return true;
}


QString AppConfig::getInterfaceIconSize()
{
    return this->get_parameter("interfaceIconSize");
}

bool AppConfig::setInterfaceIconSize(QString sizeName)
{
    // Если установлено какое-то имя размера иконок,
    // и оно не входит в список разрешенных
    if ( sizeName!="" and
         !fixedParameters.interfaceIconSizeAvailableMap.keys().contains(sizeName) )
    {
        return false;
    }

    // Запоминается имя размера иконок. Оно может быть пустым или
    // в виде строки из списка разрешенных размеров
    m_conf->setValue("interfaceIconSize", sizeName);

    return true;
}

// --------------------
// Номер версии конфига
// --------------------

int AppConfig::get_config_version(void)
{
    int v = m_conf->contains("version") ? m_conf->value("version").toInt() : 0;

    return v;
}


void AppConfig::set_config_version(int i)
{
    m_conf->setValue("version", i);
}


QStringList AppConfig::removeParameterFromTable(QString removeName, QStringList table)
{
    // Перебираются параметры в таблице
    for (int i=0; i<MYTETRA_CONFIG_PARAM_NUM; i++)
    {
        // Выясняется имя параметра
        QString name=table.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);

        // Если имя совпадает с удаляемым
        if (name==removeName)
        {
            // Удаляются данные об элементе (удаляется ячейка с одним и тем же номером
            // столько раз, сколько полей в таблице)
            for (int j=0; j<MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD; j++)
            {
                table.removeAt(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);
            }

            break;
        }
    }

    return table;
}


// Получение типа параметра в виде строки
QString AppConfig::getParameterTypeFromTable(QString parameterName, QStringList table)
{
    // Перебираются параметры в таблице
    for (int i=0; i<MYTETRA_CONFIG_PARAM_NUM; i++)
    {
        // Выясняется имя параметра
        QString name=table.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);

        if (name==parameterName)
        {
            return table.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+1);
        }
    }

    return "";
}


// Получение значения параметра по-умолчнию, в виде строки
QString AppConfig::getParameterDefaultValueFromTable(QString parameterName, QStringList table)
{
    // Перебираются параметры в таблице
    for (int i=0; i<MYTETRA_CONFIG_PARAM_NUM; i++)
    {
        // Выясняется имя параметра
        QString name=table.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);

        if (name==parameterName)
        {
            return table.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+2);
        }
    }

    return "";
}


// Замена типа и значения параметра по-умолчанию
QStringList AppConfig::replaceParameterInTable(QString replaceName,
                                               QString replaceType,
                                               QString replaceDefaultValue,
                                               QStringList table)
{
    // Перебираются параметры в таблице
    for (int i=0; i<MYTETRA_CONFIG_PARAM_NUM; i++)
    {
        // Выясняется имя параметра
        QString name=table.at(i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);

        // Если имя совпадает с заменяемым
        if (name==replaceName)
        {
            table[i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+1]=replaceType;
            table[i*MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD+2]=replaceDefaultValue;

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
    updater.setConfigFile(configFileName);

    // Номер версии текущего конфига, который есть на начало обновления
    int fromVersion=get_config_version();

    // Эта строка компилируется нормально, сделать на ее основе список указателей на функцию
    // std::function< QStringList(AppConfig&, bool) > pFunction=&AppConfig::get_parameter_table_1;

    // Перечень функций, выдающих список параметров для конкретной версии конфига
    QList< std::function< QStringList(AppConfig&, bool) > > parameterFunctions;

    // Исторически счет версий идет с 1, поэтому, чтобы не запутаться,
    // создается пустой нуливой элемент
    parameterFunctions << nullptr;

    parameterFunctions << &AppConfig::get_parameter_table_1;
    parameterFunctions << &AppConfig::get_parameter_table_2;
    parameterFunctions << &AppConfig::get_parameter_table_3;
    parameterFunctions << &AppConfig::get_parameter_table_4;
    parameterFunctions << &AppConfig::get_parameter_table_5;
    parameterFunctions << &AppConfig::get_parameter_table_6;
    parameterFunctions << &AppConfig::get_parameter_table_7;
    parameterFunctions << &AppConfig::get_parameter_table_8;
    parameterFunctions << &AppConfig::get_parameter_table_9;
    parameterFunctions << &AppConfig::get_parameter_table_10;
    parameterFunctions << &AppConfig::get_parameter_table_11;
    parameterFunctions << &AppConfig::get_parameter_table_12;
    parameterFunctions << &AppConfig::get_parameter_table_13;
    parameterFunctions << &AppConfig::get_parameter_table_14;
    parameterFunctions << &AppConfig::get_parameter_table_15;
    parameterFunctions << &AppConfig::get_parameter_table_16;
    parameterFunctions << &AppConfig::get_parameter_table_17;
    parameterFunctions << &AppConfig::get_parameter_table_18;
    parameterFunctions << &AppConfig::get_parameter_table_19;
    parameterFunctions << &AppConfig::get_parameter_table_20;
    parameterFunctions << &AppConfig::get_parameter_table_21;
    parameterFunctions << &AppConfig::get_parameter_table_22;
    parameterFunctions << &AppConfig::get_parameter_table_23;
    parameterFunctions << &AppConfig::get_parameter_table_24;
    parameterFunctions << &AppConfig::get_parameter_table_25;
    parameterFunctions << &AppConfig::get_parameter_table_26;
    parameterFunctions << &AppConfig::get_parameter_table_27;
    parameterFunctions << &AppConfig::get_parameter_table_28;
    parameterFunctions << &AppConfig::get_parameter_table_29;
    parameterFunctions << &AppConfig::get_parameter_table_30;
    parameterFunctions << &AppConfig::get_parameter_table_31;
    parameterFunctions << &AppConfig::get_parameter_table_32;
    parameterFunctions << &AppConfig::get_parameter_table_33;
    parameterFunctions << &AppConfig::get_parameter_table_34;
    parameterFunctions << &AppConfig::get_parameter_table_35;
    parameterFunctions << &AppConfig::get_parameter_table_36;
    parameterFunctions << &AppConfig::get_parameter_table_37;
    parameterFunctions << &AppConfig::get_parameter_table_38;
    parameterFunctions << &AppConfig::get_parameter_table_39;
    parameterFunctions << &AppConfig::get_parameter_table_40;
    parameterFunctions << &AppConfig::get_parameter_table_41;
    parameterFunctions << &AppConfig::get_parameter_table_42;

    for (int i=1; i<parameterFunctions.count()-1; ++i)
    {
        // Обновление идет только для версий, которые более новые чем версия текущего конфига
        if (i>=fromVersion)
        {
            // В вызове метода: *this - это объект, а true - это первый параметр метода.
            // Именно так работает вызов метода для std::function
            updater.updateVersion(i,
                                  i+1,
                                  (parameterFunctions.at(i))(*this, true),
                                  (parameterFunctions.at(i+1))(*this, true) );
        }
    }
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

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
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
    if (globalParameters.getTargetOs()=="android")
        table << "uglyQssReplaceHeightForTableView" << "int" << "35"; // Так как не все параметры можно стилизовать через QSS, здесь задается высота ячейки таблицы
    else
        table << "uglyQssReplaceHeightForTableView" << "int" << "0";

    if (withEndSignature)
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

    if (withEndSignature)
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
    if (globalParameters.getTargetOs()=="android")
        table << "showSplashScreen" << "bool" << "true"; // В Андроид долгий запуск, нужно показывать сплешскрин
    else
        table << "showSplashScreen" << "bool" << "false"; // На десктопе быстрый запуск, сплешскрин только мешает

    if (withEndSignature)
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
    if (globalParameters.getTargetOs()=="android")
        table << "interfaceMode" << "QString" << "mobile"; // В Андроид должен быть мобильный интерфейс
    else
        table << "interfaceMode" << "QString" << "desktop"; // На десктопе должен быть интерфейс адаптированный для работы на рабочем столе

    if (withEndSignature)
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

    if (withEndSignature)
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
    if (globalParameters.getTargetOs()=="android")
        table << "hideEditorTools" << "QString" << "italic,underline,monospace,alignleft,aligncenter,alignright,alignwidth,numericlist,dotlist,indentplus,indentminus,showformatting,showhtml,fontcolor,backgroundcolor,expand_edit_area,save,createtable,table_add_row,table_remove_row,table_add_col,table_remove_col,table_merge_cells,table_split_cell"; // В Андроид прячутся инструменты сложного форматирования текста
    else
        table << "hideEditorTools" << "QString" << ""; // На десктопе скрываемых кнопок редактора нет

    if (withEndSignature)
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

    if (withEndSignature)
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

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_23(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 22
    table << get_parameter_table_22(false);

    // Исключаются ненужные в новой версии параметры
    table=removeParameterFromTable("recordtable_position", table);

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_24(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 23
    table << get_parameter_table_23(false);

    table << "enableCustomDateTimeFormat" << "bool" << "false";
    table << "customDateTimeFormat" << "QString" << "";

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_25(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 24
    table << get_parameter_table_24(false);

    table << "attachAppendDir" << "QString" << "";
    table << "attachSaveAsDir" << "QString" << "";

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_26(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 25
    table << get_parameter_table_25(false);

    table << "enableDecryptFileToTrashDirectory" << "bool" << "false";

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_27(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 26
    table << get_parameter_table_26(false);

    table << "actionLogMaximumSize" << "int" << "1"; // 1 Мб

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_28(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 27
    table << get_parameter_table_27(false);

    table << "enableLogging" << "bool" << "false";

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_29(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 28
    table << get_parameter_table_28(false);

    table << "enableRecordWithAttachHighlight" << "bool" << "true";
    table << "recordWithAttachHighlightColor" << "QString" << "#d1e3c5";

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_30(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 29
    table << get_parameter_table_29(false);

    table << "enablePeriodicCheckBase" << "bool" << "false";
    table << "checkBasePeriod" << "int" << "20";
    table << "enablePeriodicCheckMessage" << "bool" << "false";


    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_31(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 30
    table << get_parameter_table_30(false);

    table << "previewIconSize" << "int" << "24";

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_32(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 31
    table << get_parameter_table_31(false);

    table << "iconCurrentSectionName" << "QString" << "Essential";

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_33(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 32
    table << get_parameter_table_32(false);

    table << "enablePeriodicSyncro" << "bool" << "false";
    table << "periodicSyncroPeriod" << "int" << "300";

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_34(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 33
    table << get_parameter_table_33(false);

    table << "enableCreateEmptyRecord" << "bool" << "false";

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_35(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 34
    table << get_parameter_table_34(false);

    // Новый флаг, поиск по названию ветки
    table << "findscreen_find_innameItem" << "bool" << "false";

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_36(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 35
    table << get_parameter_table_35(false);

    // Состояние окрепляемых окон
    // Предполагаемый формат - "IDзаписи1,x1,y1,w1,h1;IDзаписи2,x2,y2,w2,h2..."
    table << "dockableWindowsState" << "QString" << "";

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_37(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 36
    table << get_parameter_table_36(false);

    // Поведение окрепляемых окон
    table << "dockableWindowsBehavior" << "QString" << "single";

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_38(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 37
    table << get_parameter_table_37(false);

    // Обновление формата конфига не происходит, обновляется только
    // представление параметра dockableWindowsState
    // см. AppConfigUpdater::updateValueRepresentation

    if (withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_39(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 38
    table << get_parameter_table_38(false);

    // Стандартная тема оформления
    table << "theme" << "QString" << "light";

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_40(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 39
    table << get_parameter_table_39(false);

    // Обновление формата конфига не происходит, обновляется только
    // представление параметра theme
    // см. AppConfigUpdater::updateValueRepresentation

    // Теперь допустимые значения theme - default и dark, темы light нет

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_41(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 40
    table << get_parameter_table_40(false);


    // Пример переименования значения theme в themeName
    // Пока такое решение рабочее, но нерекомендовано

    // Значение старого параметра theme
    // QString themeName = this->m_conf->value("theme").toString();

    // Запоминается тема оформления в новом параметре
    // table << "interfaceTheme" << "QString" << themeName;


    // Исключаются ненужные в новой версии параметры
    table=removeParameterFromTable("theme", table);

    // Новый параметр, хранящий тему оформления
    table << "interfaceTheme" << "QString" << "default";

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList AppConfig::get_parameter_table_42(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 41
    table << get_parameter_table_41(false);

    // Размер иконок.
    // Размер обозначается специальными строками, начинающимися на "META_ICON_"
    // Если размер пустой, используется системный размер иконок
    table << "interfaceIconSize" << "QString" << "";

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}
