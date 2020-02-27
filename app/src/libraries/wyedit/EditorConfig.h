#ifndef EDITORCONFIG_H
#define EDITORCONFIG_H

#include <QObject>
#include <QWidget>
#include <QSettings>


class EditorConfig : public QWidget
{
    Q_OBJECT

public:
    EditorConfig(QString config_file_name, QWidget *parent=nullptr);
    ~EditorConfig();

    void sync(void);

    // Настройка стандартного шрифта
    QString get_default_font(void);
    void set_default_font(QString fontName);

    int get_default_font_size(void);
    void set_default_font_size(int size);

    // Настройка моноширинного шрифта
    QString get_monospace_font(void);
    void set_monospace_font(QString fontName);

    int  get_monospace_font_size(void);
    void set_monospace_font_size(int size);
    bool get_monospace_font_size_apply(void);
    void set_monospace_font_size_apply(bool i);

    
    // Настройка шрифта форматирования кода
    QString get_code_font(void);
    void    set_code_font(QString fontName);

    int  get_code_font_size(void);
    void set_code_font_size(int size);
    bool get_code_font_size_apply(void);
    void set_code_font_size_apply(bool i);
    QString get_code_font_color(void);
    void set_code_font_color(QString color);

    int  get_code_indent_size(void);
    void set_code_indent_size(int size);
    bool get_code_indent_size_apply(void);
    void set_code_indent_size_apply(bool i);
    
    // Шаг изменения отступа
    int get_indent_step(void);
    void set_indent_step(int i);

    // Размер табуляции для клавиши Tab
    int get_tab_size(void);
    void set_tab_size(int i);

    // Счетчик для таймера обновления картинки формулы в редакторе формулы
    int getMathExpressionUpdateTime(void);
    void setMathExpressionUpdateTime(int i);

    QString get_finddialog_geometry(void);
    void set_finddialog_geometry(QString geometry);

    // Настройка первой линии с кнопками инструментов
    QString get_tools_line_1(void);
    void set_tools_line_1(QString geometry);

    // Настройка второй линии с кнопками инструментов
    QString get_tools_line_2(void);
    void set_tools_line_2(QString geometry);

    // Раскрыты или сомкнуты панели инструментов
    bool get_expand_tools_lines(void);
    void set_expand_tools_lines(bool i);

private:

    QSettings *conf;

    QString get_parameter(QString name);

    int get_config_version(void);
    void set_config_version(int i);

    void update_version_process(void);

    static QStringList get_parameter_table_1(bool withEndSignature=true);
    static QStringList get_parameter_table_2(bool withEndSignature=true);
    static QStringList get_parameter_table_3(bool withEndSignature=true);
    static QStringList get_parameter_table_4(bool withEndSignature=true);
    static QStringList get_parameter_table_5(bool withEndSignature=true);
    static QStringList get_parameter_table_6(bool withEndSignature=true);
    static QStringList get_parameter_table_7(bool withEndSignature=true);
    static QStringList get_parameter_table_8(bool withEndSignature=true);
    static QStringList get_parameter_table_9(bool withEndSignature=true);
    static QStringList get_parameter_table_10(bool withEndSignature=true);
    static QStringList get_parameter_table_11(bool withEndSignature=true);
    static QStringList get_parameter_table_12(bool withEndSignature=true);
    static QStringList get_parameter_table_13(bool withEndSignature=true);
    static QStringList get_parameter_table_14(bool withEndSignature=true);
    static QStringList get_parameter_table_15(bool withEndSignature=true);
    static QStringList get_parameter_table_16(bool withEndSignature=true);
    static QStringList get_parameter_table_17(bool withEndSignature=true);
    static QStringList get_parameter_table_18(bool withEndSignature=true);
    static QStringList get_parameter_table_19(bool withEndSignature=true);
    static QStringList get_parameter_table_20(bool withEndSignature=true);
    static QStringList get_parameter_table_21(bool withEndSignature=true);
    static QStringList get_parameter_table_22(bool withEndSignature=true);

    static QStringList remove_option(QStringList table, QString optionName);

    QString update_version_allowcollision(int versionFrom,
                                          int versionTo,
                                          QString name,
                                          QString fromType,
                                          QString fromValue,
                                          QString toType,
                                          QString toValue);

    QString update_version_change_value(int versionFrom,
                                        int versionTo,
                                        QString name,
                                        QString fromValue,
                                        QString toValue);

    void update_version(int versionFrom,
                        int versionTo,
                        QStringList baseTable,
                        QStringList finalTable);

};

#endif // EDITORCONFIG_H
