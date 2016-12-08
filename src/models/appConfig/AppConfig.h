#ifndef __APPCONFIG_H__
#define __APPCONFIG_H__

#include <QObject>
#include <QDir>
#include <QSettings>
#include <QDebug>

class QRect;
class QStringList;

class AppConfig : public QObject
{
    Q_OBJECT

public:
    AppConfig(QObject *pobj=0);
    ~AppConfig();

    void init(void);
    bool is_init(void);

    void sync(void);

    // Версия формата конфигфайла
    int get_config_version(void);
    void set_config_version(int i);

    // Рабочая директория
    QString get_tetradir(void);
    bool set_tetradir(QString dirName);

    // Директория с корзиной
    QString get_trashdir(void);
    bool set_trashdir(QString dirName);
    
    // Максимальный размер директории корзины в мегабайтах
    unsigned int get_trashsize(void);
    bool set_trashsize(unsigned int mbSize);
    
    // Максимально допустимое число файлов в корзине
    int get_trashmaxfilecount(void);
    bool set_trashmaxfilecount(int count);

    // Нужно ли показывать подтверждение при выполнении действия "cut на ветке
    bool get_cutbranchconfirm(void);
    bool set_cutbranchconfirm(bool confirm);

    // Нужно ли печатать отладочные сообщения в консоль
    bool get_printdebugmessages(void);
    bool set_printdebugmessages(bool isPrint);

    // Язык интерфейса
    QString get_interfacelanguage(void);
    bool set_interfacelanguage(QString language);

    // Номер последнего префикса в виде строки с ведущими нулями
    QString get_addnewrecord_expand_info(void);
    void set_addnewrecord_expand_info(QString);

    // Геометрия основного окна
    QRect get_mainwingeometry(void);
    void set_mainwingeometry(int x, int y, int w, int h);    
    

    QList<int> get_vspl_size_list(void);
    void set_vspl_size_list(QList<int> list);

    QList<int> get_hspl_size_list(void);
    void set_hspl_size_list(QList<int> list);

    QList<int> get_findsplitter_size_list(void);
    void set_findsplitter_size_list(QList<int> list);

    QList<int> get_splitter_size_list(QString name);
    void set_splitter_size_list(QString name, QList<int> list);

    
    QStringList get_tree_position(void);
    void set_tree_position(QStringList list);


    // ID записи в таблице конечных записей, которая выделена на экране
    QString get_recordtable_selected_record_id(void);
    void set_recordtable_selected_record_id(QString id);


    // Настройки виджета поиска по базе
    int get_findscreen_wordregard(void);
    void set_findscreen_wordregard(int pos);

    int get_findscreen_howextract(void);
    void set_findscreen_howextract(int pos);
    
    int getFindScreenTreeSearchArea(void);
    void setFindScreenTreeSearchArea(int pos);

    bool get_findscreen_find_in_field(QString fieldName);
    void set_findscreen_find_in_field(QString fieldName, bool isChecked);

    bool get_findscreen_show(void);
    void set_findscreen_show(bool isShow);

    QString get_howpassrequest(void);
    void set_howpassrequest(QString mode);

    bool get_runinminimizedwindow(void);
    void set_runinminimizedwindow(bool flag);

    bool get_autoClosePasswordEnable(void);
    void set_autoClosePasswordEnable(bool flag);

    int  get_autoClosePasswordDelay(void);
    void set_autoClosePasswordDelay(int delay);

    QString get_synchrocommand(void);
    void set_synchrocommand(QString command);

    bool get_synchroonstartup(void);
    void set_synchroonstartup(bool flag);

    bool get_synchroonexit(void);
    void set_synchroonexit(bool flag);

    bool getSyncroConsoleDetails(void);
    void setSyncroConsoleDetails(bool flag);
    
    int  getEditorCursorPosition(void);
    void setEditorCursorPosition(int n);

    int  getEditorScrollBarPosition(void);
    void setEditorScrollBarPosition(int n);

    QString getPasswordMiddleHash(void);
    void setPasswordMiddleHash(QString hash);

    bool getPasswordSaveFlag(void);
    void setPasswordSaveFlag(bool flag);


    bool getRememberCursorAtHistoryNavigation(void);
    void setRememberCursorAtHistoryNavigation(bool flag);

    bool getRememberCursorAtOrdinarySelection(void);
    void setRememberCursorAtOrdinarySelection(bool flag);

    int  getUglyQssReplaceHeightForTableView(void);
    void setUglyQssReplaceHeightForTableView(int n);


    // Перечень полей, отображаемых в таблице конечных записей
    QStringList getRecordTableShowFields(void);
    void setRecordTableShowFields(QStringList fields);

    // Нужно ли отображать горизонтальные заголовки в таблице конечных записей
    bool getRecordTableShowHorizontalHeaders(void);
    void setRecordTableShowHorizontalHeaders(bool flag);

    // Нужно ли отображать вертикальные заголовки в таблице конечных записей
    bool getRecordTableShowVerticalHeaders(void);
    void setRecordTableShowVerticalHeaders(bool flag);

    // Ширина полей, отображаемых в таблице конечных записей
    QStringList getRecordTableFieldsWidth(void);
    void setRecordTableFieldsWidth(QStringList fieldsWidth);

    // Показывать ли сплешскрин при старте программы
    bool getShowSplashScreen(void);
    void setShowSplashScreen(bool isShow);

    // Режим интерфейса, возможные значения "desktop" и "mobile"
    QString getInterfaceMode(void);
    void setInterfaceMode(QString mode);

    QString getFocusWidget(void);
    void setFocusWidget(QString widgetName);

    QStringList getHideEditorTools(void);
    void setHideEditorTools(QStringList toolsNames);

    // Флаг, показывающий распахнуты ли все инструметы управления в окне поиска по базе
    bool getFindInBaseExpand(void);
    void setFindInBaseExpand(bool state);

    // Разрешено ли использовать собственный формат вывода даты и времени
    bool getEnableCustomDateTimeFormat(void);
    void setEnableCustomDateTimeFormat(bool state);

    // Строка собственного формата вывода даты и времени
    QString getCustomDateTimeFormat(void);
    void setCustomDateTimeFormat(QString format);

    // Путь на диске по которому пользователь открывал файлы чтобы приаттачить их к записи
    QString getAttachAppendDir(void);
    void setAttachAppendDir(QString dir);

    // Путь на диске по которому пользователь сохранял (Сохранить как...) приаттаченные файлы
    QString getAttachSaveAsDir(void);
    void setAttachSaveAsDir(QString dir);

    // Разрешать ли для просмотра расшифровывать зашифрованные файлы в директорию корзины MyTetra
    bool getEnableDecryptFileToTrashDirectory(void);
    void setEnableDecryptFileToTrashDirectory(bool state);

    // Размер файла лога дейсвтвий
    unsigned int getActionLogMaximumSize();
    void setActionLogMaximumSize(unsigned int mbSize);

    // Разрешено ли логирование
    bool getEnableLogging(void);
    void setEnableLogging(bool state);

    // Разрешена ли подсветка записей с прикрепленными файлами
    bool getEnableRecordWithAttachHighlight(void);
    void setEnableRecordWithAttachHighlight(bool state);

    // Цвет подсветки записей с прикрепленными файлами
    QString getRecordWithAttachHighlightColor(void);
    void setRecordWithAttachHighlightColor(QString color); // Формат цвета - "#0099FF"

    // Разрешена ли периодическая проверка файла базы на предмет изменения сторонней программой
    bool getEnablePeriodicCheckBase(void);
    void setEnablePeriodicCheckBase(bool state);

    // Период проверки файла базы на предмет изменения сторонней программой
    int getCheckBasePeriod();
    void setCheckBasePeriod(int period);

    // Необходимо ли выводить сообщение если база была изменена другой программой
    bool getEnablePeriodicCheckMessage(void);
    void setEnablePeriodicCheckMessage(bool state);

    // Размер иконок в окне выбора (иконки, прикрепляемые к веткам)
    int  getPreviewIconSize(void);
    void setPreviewIconSize(int n);

    // Последняя выбранная секция в окошке выбора иконок
    QString getIconCurrentSectionName(void);
    void setIconCurrentSectionName(QString name);

    // Разрешена ли периодическая синхронизация
    bool getEnablePeriodicSyncro(void);
    void setEnablePeriodicSyncro(bool state);

    // Период автоматической периодической синхронизации
    int  getPeriodicSyncroPeriod(void);
    void setPeriodicSyncroPeriod(int period);

    // Разрешено ли создавать запись без текста
    bool getEnableCreateEmptyRecord(void);
    void setEnableCreateEmptyRecord(bool state);


private:

    QSettings *conf; // было static
    QString get_parameter(QString name);

    QStringList removeParameterFromTable(QString removeName, QStringList table);
    QString getParameterTypeFromTable(QString parameterName, QStringList table);
    QString getParameterValueFromTable(QString parameterName, QStringList table);
    QStringList replaceParameterInTable(QString replaceName, QString replaceType, QString replaceValue, QStringList table);


    void update_version_process(void);

    QStringList get_parameter_table_1 (bool withEndSignature=true);
    QStringList get_parameter_table_2 (bool withEndSignature=true);
    QStringList get_parameter_table_3 (bool withEndSignature=true);
    QStringList get_parameter_table_4 (bool withEndSignature=true);
    QStringList get_parameter_table_5 (bool withEndSignature=true);
    QStringList get_parameter_table_6 (bool withEndSignature=true);
    QStringList get_parameter_table_7 (bool withEndSignature=true);
    QStringList get_parameter_table_8 (bool withEndSignature=true);
    QStringList get_parameter_table_9 (bool withEndSignature=true);
    QStringList get_parameter_table_10(bool withEndSignature=true);
    QStringList get_parameter_table_11(bool withEndSignature=true);
    QStringList get_parameter_table_12(bool withEndSignature=true);
    QStringList get_parameter_table_13(bool withEndSignature=true);
    QStringList get_parameter_table_14(bool withEndSignature=true);
    QStringList get_parameter_table_15(bool withEndSignature=true);
    QStringList get_parameter_table_16(bool withEndSignature=true);
    QStringList get_parameter_table_17(bool withEndSignature=true);
    QStringList get_parameter_table_18(bool withEndSignature=true);
    QStringList get_parameter_table_19(bool withEndSignature=true);
    QStringList get_parameter_table_20(bool withEndSignature=true);
    QStringList get_parameter_table_21(bool withEndSignature=true);
    QStringList get_parameter_table_22(bool withEndSignature=true);
    QStringList get_parameter_table_23(bool withEndSignature=true);
    QStringList get_parameter_table_24(bool withEndSignature=true);
    QStringList get_parameter_table_25(bool withEndSignature=true);
    QStringList get_parameter_table_26(bool withEndSignature=true);
    QStringList get_parameter_table_27(bool withEndSignature=true);
    QStringList get_parameter_table_28(bool withEndSignature=true);
    QStringList get_parameter_table_29(bool withEndSignature=true);
    QStringList get_parameter_table_30(bool withEndSignature=true);
    QStringList get_parameter_table_31(bool withEndSignature=true);
    QStringList get_parameter_table_32(bool withEndSignature=true);
    QStringList get_parameter_table_33(bool withEndSignature=true);
    QStringList get_parameter_table_34(bool withEndSignature=true);

    bool is_init_flag;
};

#endif // __APPCONFIG_H__
