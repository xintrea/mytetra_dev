#include <QFileInfo>
#include <QtGlobal>
#include <QtDebug>

#include "EditorConfig.h"
#include "../../main.h"


// Конструктор объекта хранения настроек редактора
EditorConfig::EditorConfig(QString config_file_name, QWidget *parent) : QWidget(parent)
{
    Q_UNUSED(parent)

    // Информация о файле настроек редактора
    QFileInfo fileinfo(config_file_name);

    // Проверяется, есть ли файл конфигурации
    if(fileinfo.exists()==false)
        criticalError("Editor config file "+config_file_name+" not found.");

    // Проверяется, доступен ли файл конфигурации на чтение или запись
    if(fileinfo.isWritable()==false || fileinfo.isReadable()==false)
        criticalError("Editor config file "+config_file_name+" not writable or readable. Please check file permission.");

    // Полное имя файла конфигурации разбивается на путь и имя файла
    QString file_name=fileinfo.fileName();
    QString file_dir =fileinfo.path();

    // Создается объект работы с конфигурацией редактора
    conf=new QSettings(config_file_name, QSettings::IniFormat);
    // conf->setPath(QSettings::IniFormat,QSettings::UserScope,file_dir);
    // conf->setPath(QSettings::IniFormat,QSettings::SystemScope,file_dir);

    // Вызываются действия для обновления конфига
    update_version_process();
}


// Деструктор объекта настройки редактора
EditorConfig::~EditorConfig()
{
    qDebug() << "Save editor config file";
    conf->sync();
}


void EditorConfig::sync(void)
{
    conf->sync();
}


// Получение параметра по имени в виде строки с проверкой его существования
QString EditorConfig::get_parameter(QString name)
{
    QString t=conf->value(name).toString();

    if(t.length()==0)
        criticalError("In editor config not found parameter " + name);

    return t;
}


QString EditorConfig::get_default_font(void)
{
    return get_parameter("default_font");
}


void EditorConfig::set_default_font(QString fontName)
{
    conf->setValue("default_font",fontName);
}


int EditorConfig::get_default_font_size(void)
{
    int n=get_parameter("default_font_size").toInt();

    if(n==0)return 10;
    else    return n;
}


void EditorConfig::set_default_font_size(int size)
{
    conf->setValue("default_font_size",size);
}


// ------------------------------
// Настройки моноширинного шрифта
// ------------------------------

QString EditorConfig::get_monospace_font(void)
{
    return get_parameter("monospace_font");
}


void EditorConfig::set_monospace_font(QString fontName)
{
    conf->setValue("monospace_font",fontName);
}


int EditorConfig::get_monospace_font_size(void)
{
    int n=get_parameter("monospace_font_size").toInt();

    if(n==0)
        return 10;
    else
        return n;
}


void EditorConfig::set_monospace_font_size(int size)
{
    conf->setValue("monospace_font_size",size);
}


bool EditorConfig::get_monospace_font_size_apply(void)
{
    if(get_parameter("monospace_font_size_apply").toInt()==1)
        return true;
    else
        return false;
}


void EditorConfig::set_monospace_font_size_apply(bool i)
{
    if(i==true)
        conf->setValue("monospace_font_size_apply", 1);
    else
        conf->setValue("monospace_font_size_apply", 0);
}


// -----------------------------
// Настройки форматирования кода
// -----------------------------

QString EditorConfig::get_code_font(void)
{
    return get_parameter("code_font");
}


void EditorConfig::set_code_font(QString fontName)
{
    conf->setValue("code_font",fontName);
}


int EditorConfig::get_code_font_size(void)
{
    int n=get_parameter("code_font_size").toInt();

    if(n==0)
        return 10;
    else
        return n;
}


void EditorConfig::set_code_font_size(int size)
{
    conf->setValue("code_font_size",size);
}


bool EditorConfig::get_code_font_size_apply(void)
{
    if(get_parameter("code_font_size_apply").toInt()==1)
        return true;
    else
        return false;
}


void EditorConfig::set_code_font_size_apply(bool i)
{
    if(i==true)
        conf->setValue("code_font_size_apply", 1);
    else
        conf->setValue("code_font_size_apply", 0);
}


QString EditorConfig::get_code_font_color(void)
{
    return get_parameter("code_font_color");
}


void EditorConfig::set_code_font_color(QString color)
{
    QColor saveColor(color);

    // Если сохраняема строка действительно содержит закодированный цвет
    if(saveColor.isValid())
        conf->setValue("code_font_color", color);
}


int EditorConfig::get_code_indent_size(void)
{
    int n=get_parameter("code_indent_size").toInt();

    if(n==0)
        return 10;
    else
        return n;
}


void EditorConfig::set_code_indent_size(int size)
{
    conf->setValue("code_indent_size",size);
}


bool EditorConfig::get_code_indent_size_apply(void)
{
    if(get_parameter("code_indent_size_apply").toInt()==1)
        return true;
    else
        return false;
}


void EditorConfig::set_code_indent_size_apply(bool i)
{
    if(i==true)
        conf->setValue("code_indent_size_apply", 1);
    else
        conf->setValue("code_indent_size_apply", 0);
}


// ----------------------
// Настройки шага отступа
// ----------------------

int EditorConfig::get_indent_step(void)
{
    int n=get_parameter("indent_step").toInt();

    if(n==0)
        return 10;
    else
        return n;
}


void EditorConfig::set_indent_step(int i)
{
    conf->setValue("indent_step",i);
}


// -------------------------------------------
// Настройки размера табуляции для клавиши Tab
// -------------------------------------------

int EditorConfig::get_tab_size(void)
{
    int n=get_parameter("tab_size").toInt();

    if(n==0)
        return 4;
    else
        return n;
}


void EditorConfig::set_tab_size(int i)
{
    conf->setValue("tab_size",i);
}


// -------------------------------------------
// Счетчик для таймера обновления картинки формулы в редакторе формулы
// -------------------------------------------

int EditorConfig::getMathExpressionUpdateTime(void)
{
    int n = get_parameter("mathExpressionUpdateTime").toInt();

    return n==0 ? 1 : n;
}


void EditorConfig::setMathExpressionUpdateTime(int i)
{
    conf->setValue("mathExpressionUpdateTime",i);
}


// -----------------------------
// Координаты поискового диалога
// -----------------------------

QString EditorConfig::get_finddialog_geometry(void)
{
    return get_parameter("finddialog_geometry");
}


void EditorConfig::set_finddialog_geometry(QString geometry)
{
    conf->setValue("finddialog_geometry",geometry);
}



// ----------------------------------
// Расположение кнопок форматирования
// ----------------------------------

QString EditorConfig::get_tools_line_1(void)
{
    return get_parameter("tools_line_1");
}


void EditorConfig::set_tools_line_1(QString line)
{
    conf->setValue("tools_line_1",line);
}


QString EditorConfig::get_tools_line_2(void)
{
    return get_parameter("tools_line_2");
}


void EditorConfig::set_tools_line_2(QString line)
{
    conf->setValue("tools_line_2",line);
}


bool EditorConfig::get_expand_tools_lines(void)
{
    if(get_parameter("expand_tools_lines").toInt()==1)
        return true;
    else
        return false;
}


void EditorConfig::set_expand_tools_lines(bool i)
{
    if(i==true)
        conf->setValue("expand_tools_lines", 1);
    else
        conf->setValue("expand_tools_lines", 0);
}


// --------------------
// Номер версии конфига
// --------------------

int EditorConfig::get_config_version(void)
{
    if(conf->contains("version"))
        return conf->value("version").toInt();
    else
        return 0;
}


void EditorConfig::set_config_version(int i)
{
    conf->setValue("version",i);
}


// ------------------------------------
// Методы для обновления версии конфига
// ------------------------------------

void EditorConfig::update_version_process(void)
{
    int fromVersion=get_config_version();

    QList<QStringList (*)(bool)> parameterFunctions;

    parameterFunctions << nullptr; // Исторически счет версий идет с 1, поэтому, чтобы не запутаться, создается пустой нуливой элемент
    parameterFunctions << get_parameter_table_1;
    parameterFunctions << get_parameter_table_2;
    parameterFunctions << get_parameter_table_3;
    parameterFunctions << get_parameter_table_4;
    parameterFunctions << get_parameter_table_5;
    parameterFunctions << get_parameter_table_6;
    parameterFunctions << get_parameter_table_7;
    parameterFunctions << get_parameter_table_8;
    parameterFunctions << get_parameter_table_9;
    parameterFunctions << get_parameter_table_10;
    parameterFunctions << get_parameter_table_11;
    parameterFunctions << get_parameter_table_12;
    parameterFunctions << get_parameter_table_13;
    parameterFunctions << get_parameter_table_14;
    parameterFunctions << get_parameter_table_15;
    parameterFunctions << get_parameter_table_16;
    parameterFunctions << get_parameter_table_17;
    parameterFunctions << get_parameter_table_18;
    parameterFunctions << get_parameter_table_19;
    parameterFunctions << get_parameter_table_20;
    parameterFunctions << get_parameter_table_21;
    parameterFunctions << get_parameter_table_22;

    for(int i=1; i<parameterFunctions.count()-1; ++i)
        if(fromVersion<=i)
            update_version(i, i+1, (parameterFunctions.at(i))(true), (parameterFunctions.at(i+1))(true) );
}


QStringList EditorConfig::get_parameter_table_1(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    table << "default_font"                << "QString"    << "DejaVu Sans";
    table << "default_font_size"           << "int"        << "10";
    table << "monospace_font"              << "QString"    << "DejaVu Sans Mono";
    table << "monospace_font_size"         << "int"        << "10";
    table << "monospace_font_size_apply"   << "bool"       << "1";
    table << "monospace_indent_size"       << "int"        << "10";
    table << "monospace_indent_size_apply" << "bool"       << "1";
    table << "indent_step"                 << "int"        << "10";
    table << "finddialog_geometry"         << "QString"    << "";

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_2(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 1
    table << get_parameter_table_1(false);

    // Удаляются неиспользуемые параметры
    table=remove_option(table, "monospace_indent_size");
    table=remove_option(table, "monospace_indent_size_apply");

    // Добавляются новые параметры
    table << "code_font"                   << "QString"    << "DejaVu Sans Mono";
    table << "code_font_size"              << "int"        << "10";
    table << "code_font_size_apply"        << "bool"       << "1";
    table << "code_indent_size"            << "int"        << "20";
    table << "code_indent_size_apply"      << "bool"       << "1";

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_3(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 2
    table << get_parameter_table_2(false);

    // Добавляются новые параметры
    table << "tools_line_1" << "QString" << "clear,separator,bold,italic,underline,separator,monospace,code,alignleft,aligncenter,alignright,alignwidth,fontselect,fontsize";
    table << "tools_line_2" << "QString" << " ";

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_4(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 3
    table << get_parameter_table_3(false);

    // В параметр tools_line_2 добавляется "insert_image_from_file"
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_5(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 4
    table << get_parameter_table_4(false);

    // Новые параметры
    table << "code_font_color" << "QString"    << "#6E120A";

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_6(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 5
    table << get_parameter_table_5(false);

    // Новые параметры
    table << "expand_tools_lines" << "bool" << "1";

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_7(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 6
    table << get_parameter_table_6(false);

    // В параметр tools_line_2 добавляется "show_text"
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_8(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 6
    table << get_parameter_table_7(false);

    // В параметр tools_line_1 добавляется "attach"
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_9(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 8
    table << get_parameter_table_8(false);

    // В параметре tools_line_1 заменяется showhtml на reference, или просто добавляется reference если нет showhtml
    // В параметре tools_line_2 добавляется showhtml
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_10(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 9
    table << get_parameter_table_9(false);

    // В параметре tools_line_2 добавляется table_properties после createtable
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_11(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 10
    table << get_parameter_table_10(false);

    // В параметре tools_line_2 добавляется в конец кнопка fix_break_symbol
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_12(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 11
    table << get_parameter_table_11(false);

    // В параметре tools_line_1 добавляется кнопка text_only
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_13(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 12
    table << get_parameter_table_12(false);

    // В параметр tools_line_1 добавляется "math_expression"
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_14(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 13
    table << get_parameter_table_13(false);

    // В параметр tools_line_1 добавляется "insert_horizontal_line"
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_15(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 14
    table << get_parameter_table_14(false);

    // Добавляются новые параметры
    // размер табуляции (клавиша Tab)
    table << "tab_size" << "int" << "4";

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_16(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 15
    table << get_parameter_table_15(false);

    // В параметр tools_line_1 добавляется "strikeout"
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_17(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 16
    table << get_parameter_table_16(false);

    // В параметр tools_line_1 добавляется "sup"
    // В параметр tools_line_1 добавляется "sub"
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_18(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 17
    table << get_parameter_table_17(false);

    // В параметр tools_line_1 добавляется "separator,undo,redo,separator"
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_19(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 18
    table << get_parameter_table_18(false);

    // В параметр tools_line_2 добавляется "separator,fontcolor,backgroundcolor,separator"
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_20(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 14
    table << get_parameter_table_19(false);

    // Добавляются новые параметры
    // время обновления картинки формулы (в секундах)
    table << "mathExpressionUpdateTime" << "int" << "1";

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_21(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 20
    table << get_parameter_table_20(false);

    // В параметрах tools_line_1 и tools_line_1 сменяются "fontselect" на "fontSelect"
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


QStringList EditorConfig::get_parameter_table_22(bool withEndSignature)
{
    // Таблица параметров
    // Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
    QStringList table;

    // Старые параметры, аналогичные версии 21
    table << get_parameter_table_21(false);

    // В параметрах tools_line_1 и tools_line_1 изменяются все имена инструментов
    // см. метод update_version_change_value()

    if(withEndSignature)
        table << "0" << "0" << "0";

    return table;
}


// Метод разрешения конфликтов если исходные и конечные типы не совпадают
// Должен включать в себя логику обработки только тех параметров
// и только для тех версий конфигов, которые действительно
// должны поменять тип. Если для принятых параметров не будет
// обработки, это значит что что-то сделано программистом не так
// и нужно дорабатывать код
QString EditorConfig::update_version_allowcollision(int versionFrom,
                                                    int versionTo,
                                                    QString name,
                                                    QString fromType,
                                                    QString fromValue,
                                                    QString toType,
                                                    QString toValue)
{
    criticalError("Error while update config version \nFrom: "+(QString::number(versionFrom))+
                  "\nTo: "+(QString::number(versionTo))+
                  "\nName: "+name+
                  "\nFrom type: "+fromType+
                  "\nFrom value: "+fromValue+
                  "\nTo type: "+toType+
                  "\nTo value: "+toValue);

    return QString();
}


// Метод изменения значения при неоходимости
// Тут находятся исключительные случаи
// В общем случае метод сохраняет предыдущее значение
QString EditorConfig::update_version_change_value(int versionFrom,
                                                  int versionTo,
                                                  QString name,
                                                  QString fromValue,
                                                  QString toValue)
{
    Q_UNUSED(toValue)

    QString result=fromValue;

    if(versionFrom==3 && versionTo==4 && name=="tools_line_2")
    {
        if(!result.contains("insert_image_from_file"))
            result=result+",insert_image_from_file";
    }

    if(versionFrom==6 && versionTo==7 && name=="tools_line_2")
    {
        if(!result.contains("show_text"))
            result=result+",show_text";
    }

    if(versionFrom==7 && versionTo==8 && name=="tools_line_1")
    {
        if(!result.contains("attach"))
            result=result+",attach";
    }

    if(versionFrom==8 && versionTo==9)
    {
        if(name=="tools_line_1")
        {
            if(result.contains("showhtml"))
                result.replace("showhtml", "reference");
            else
                result=result+",reference";
        }


        if(name=="tools_line_2")
            if(!result.contains("showhtml"))
                result=result+",showhtml";
    }


    if(versionFrom==9 && versionTo==10)
        if(name=="tools_line_2")
            if(!result.contains("table_properties"))
            {
                if(result.contains("createtable"))
                    result.replace("createtable", "createtable,table_properties");
                else
                    result=result+",table_properties";
            }

    if(versionFrom==10 && versionTo==11)
        if(name=="tools_line_2")
            if(!result.contains("fix_break_symbol"))
                result=result+",fix_break_symbol";

    if(versionFrom==11 && versionTo==12)
        if(name=="tools_line_1")
            if(!result.contains("table_properties"))
            {
                if(result.contains("code"))
                    result.replace("code", "code,text_only");
                else
                    result=result+",text_only";
            }

    if(versionFrom==12 && versionTo==13)
        if(name=="tools_line_1")
            if(!result.contains("math_expression"))
            {
                if(result.contains("insert_image_from_file"))
                    result.replace("insert_image_from_file", "insert_image_from_file,math_expression");
                else
                    result=result+",math_expression";
            }

    if(versionFrom==13 && versionTo==14)
        if(name=="tools_line_1")
        {
            if(!result.contains("insert_horizontal_line"))
            {
                if(result.contains("insert_image_from_file"))
                    result.replace("insert_image_from_file", "insert_image_from_file,insert_horizontal_line");
                else
                    result=result+",insert_horizontal_line";
            }
        }

    if(versionFrom==15 && versionTo==16)
        if(name=="tools_line_1")
        {
            if(!result.contains("strikeout"))
            {
                if(result.contains("underline"))
                    result.replace("underline", "underline,strikeout");
                else
                    result=result+",strikeout";
            }
        }

    if(versionFrom==16 && versionTo==17)
        if(name=="tools_line_1")
        {
            if(!result.contains("sup") && !result.contains("sub"))
            {
                if(result.contains("underline"))
                    result.replace("underline", "underline,sup,sub");
                else
                    result=result+",sup,sub";
            }
        }

    if(versionFrom==17 && versionTo==18)
        if(name=="tools_line_1")
        {
            if(!result.contains("undo") && !result.contains("redo"))
            {
                if(result.contains("clear"))
                    result.replace("clear", "clear,separator,undo,redo,separator");
                else
                    result=result+",separator,undo,redo,separator";
            }
        }


    if(versionFrom==18 && versionTo==19)
        if(name=="tools_line_2")
        {
            if(!result.contains("fontcolor") && !result.contains("backgroundcolor"))
            {
                if(result.contains("fontsize"))
                    result.replace("fontsize", "fontsize,separator,fontcolor,backgroundcolor,separator");
                else
                    result=result+",separator,fontcolor,backgroundcolor,separator";
            }
            else
            {
                if(result.contains("fontcolor"))
                    result.replace("fontcolor", "fontcolor,backgroundcolor,separator");
                else
                    result=result+",backgroundcolor,separator";
            }
        }

    if(versionFrom==20 && versionTo==21)
        if(name=="tools_line_1" or name=="tools_line_2")
        {
            if(result.contains("fontselect"))
            {
                result.replace("fontselect", "fontSelect");
            }
        }

    if(versionFrom==21 && versionTo==22)
        if(name=="tools_line_1" or name=="tools_line_2")
        {
            QMap<QString, QString> names;

            names["fontselect"]="fontSelect";
            names["aligncenter"]="alignCenter";
            names["alignleft"]="alignLeft";
            names["alignright"]="alignRight";
            names["alignwidth"]="alignWidth";
            names["attach"]="toAttach";
            names["backgroundcolor"]="backgroundColor";
            names["createtable"]="createTable";
            names["dotlist"]="dotList";
            names["expand_edit_area"]="expandEditArea";
            names["expand_tools_lines"]="expandToolsLines";
            names["find_in_base"]="findInBase";
            names["findtext"]="findText";
            names["fix_break_symbol"]="fixBreakSymbol";
            names["fontcolor"]="fontColor";
            names["fontselect"]="fontSelect";
            names["fontsize"]="fontSize";
            names["indentminus"]="indentMinus";
            names["indentplus"]="indentPlus";
            names["insert_horizontal_line"]="insertHorizontalLine";
            names["insert_image_from_file"]="insertImageFromFile";
            names["math_expression"]="mathExpression";
            names["numericlist"]="numericList";
            names["showformatting"]="showFormatting";
            names["showhtml"]="showHtml";
            names["show_text"]="showText";
            names["sub"]="subscript";
            names["sup"]="superscript";
            names["text_only"]="textOnly";
            names["table_add_col"]="tableAddCol";
            names["table_add_row"]="tableAddRow";
            names["table_merge_cells"]="tableMergeCells";
            names["table_properties"]="tableProperties";
            names["table_remove_col"]="tableRemoveCol";
            names["table_remove_row"]="tableRemoveRow";
            names["table_split_cell"]="tableSplitCell";

            for (auto key: names.keys())
            {
              auto value=names.value(key);

              result.replace(key, value);
            }
        }


    return result;
}


// Основной метод обновления версий конфига
void EditorConfig::update_version(int versionFrom,
                                  int versionTo,
                                  QStringList baseTable,
                                  QStringList finalTable)
{
    // Таблица исходных параметров преобразуется к более удобному для работы виду
    // И параллельно заполняется значениями из конфига
    QMap< QString, QMap< QString, QString > > fromTable;
    for(int i=0;i<100;i++)
    {
        QString name=        baseTable.at(i*3+0);
        QString type=        baseTable.at(i*3+1);
        QString defaultValue=baseTable.at(i*3+2);

        // Если достигнут конец массива
        if(name=="0" && type=="0" && defaultValue=="0") break;

        // Подготовка массива для текущего параметра
        QMap< QString, QString > line;
        line.clear();
        line["type"]=type;
        if(conf->contains(name))
            line["value"]=conf->value(name).toString(); // Значение из конфига
        else
            line["value"]=defaultValue; // Дефолтное значение

        // Для текущего имени параметра запоминается массив
        fromTable[name]=line;
    }


    // Таблица конечных параметров преобразуется к более удобному для работы виду
    // Параллельно создаётся список контролирующий обработанные параметры
    // Далее из контролирующего списка будут убираться обработанные параметры
    // Контролирующий список нужен для того, чтобы не удалять записи
    // из конечного массива во время обхода через итератор
    QMap< QString, QMap< QString, QString > > toTable;
    QStringList controlList;
    for(int i=0;i<100;i++)
    {
        QString name=     finalTable.at(i*3+0);
        QString type=     finalTable.at(i*3+1);
        QString defaultValue=finalTable.at(i*3+2);

        // Если достигнут конец массива
        if(name=="0" && type=="0" && defaultValue=="0") break;

        // Подготовка массива для текущего параметра
        QMap< QString, QString > line;
        line.clear();
        line["type"]=type;
        line["value"]=defaultValue; // Дефолтное значение

        // Для текущего имени параметра запоминается массив
        toTable[name]=line;
        controlList << name; // Имя заносится в контролирующий список
    }

    // qDebug() << "From table";
    // qDebug() << fromTable;
    // qDebug() << "To table";
    // qDebug() << toTable;

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

        // qDebug() << "To name: " << toName;
        // qDebug() << "To type: " << toType;
        // qDebug() << "To value: " << toValue;

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

            // qDebug() << "Line2: " << line2;
            // qDebug() << "From type: " << fromType;
            // qDebug() << "From value: " << fromValue;

            if(toType==fromType)
                beforeParamFlag=1; // Параметр есть, и типы совпадают
            else
                beforeParamFlag=2; // Параметр есть, но типы не совпадают
        }


        // Параметра в предыдущей версии конфига не было
        if(beforeParamFlag==0)
        {
            // Будет просто сохранено новое дефолтное значение
            // Ничего с конечным параметром делать не нужно
            // Параметр из контролирующего массива исключается
            controlList.removeOne(toName);
        }


        // Параметр в предыдущей версии конфига есть, и типы совпадают
        if(beforeParamFlag==1)
        {
            // Будет в общем случае сохранено предыдущее значение
            toTable[toName]["value"]=update_version_change_value(versionFrom,versionTo,toName,
                                                                 fromValue,
                                                                 toValue);

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
        criticalError( "Error while update config from "+(QString::number(versionFrom))+" to "+(QString::number(versionTo))+" Can not compute parameter "+controlList.join(",") );

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
            conf->setValue(toName,toValue.toInt()); // Булевые переменные как 0 или 1
    }

    // Устанавливается новый номер версии
    set_config_version(versionTo);
}


QStringList EditorConfig::remove_option(QStringList table, QString optionName)
{
    // Перебираются строки таблицы
    for(int i=0; i<(table.length()/3); i++)
    {
        QString name=        table.at(i*3+0);
        QString type=        table.at(i*3+1);
        QString defaultValue=table.at(i*3+2);

        // Если достигнут конец массива
        if(name=="0" && type=="0" && defaultValue=="0") break;

        if(name==optionName)
        {
            table.removeAt(i*3);
            table.removeAt(i*3);
            table.removeAt(i*3);
            break;
        }
    }

    return table;
}
