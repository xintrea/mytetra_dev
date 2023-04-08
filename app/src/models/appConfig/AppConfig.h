#ifndef __APPCONFIG_H__
#define __APPCONFIG_H__

#include <QObject>
#include <QDir>
#include <QSettings>
#include <QDebug>

class AppConfig : public QObject
{
    Q_OBJECT

public:
    AppConfig(QObject *pobj=nullptr);
    ~AppConfig();

    void init(void);
    bool is_init(void);

    void sync(void);

    QString getConfigFileName();

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
    QByteArray get_mainwingeometry(void);
    void set_mainwingeometry(QByteArray dataGeometry);
    

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

    bool getSynchroConsoleDetails(void);
    void setSynchroConsoleDetails(bool flag);
    
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
    bool getEnablePeriodicSynchro(void);
    void setEnablePeriodicSynchro(bool state);

    // Период автоматической периодической синхронизации
    int  getPeriodicSynchroPeriod(void);
    void setPeriodicSynchroPeriod(int period);

    // Разрешено ли создавать запись без текста
    bool getEnableCreateEmptyRecord(void);
    void setEnableCreateEmptyRecord(bool state);

    // Список открепляемых окон с координатами окон
    QString getDockableWindowsState(void);
    void setDockableWindowsState(QString state);

    QString getDockableWindowsBehavior(void);
    void setDockableWindowsBehavior(QString mode);


private:

    QSettings *conf; // было static

    QString get_parameter(QString name);

    void add_default_param(const QString name, const QVariant value);
    void set_version_1_default_params();
    void update_version_process(void);

    bool is_init_flag;
};

#endif // __APPCONFIG_H__
