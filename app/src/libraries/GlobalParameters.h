#ifndef GLOBALPARAMETERS_H
#define	GLOBALPARAMETERS_H

#include <QObject>
#include <QStatusBar>

class TreeScreen;
class MetaEditor;
class RecordTableScreen;
class FindScreen;
class WindowSwitcher;
class CommandRunner;

class GlobalParameters : public QObject
{
    Q_OBJECT

public:

    // Указание на обрабатываемую панель инструментов редактора текста
    enum EditorToolbar {
        First = 0,
        Second
    };

    // Возможные режимы добавления записей в таблицу конечных записей
    enum AddNewRecordBehavior
    {
        ADD_TO_END=0,
        ADD_BEFORE,
        ADD_AFTER
    };

    // Флаги создания новой рабочей директории
    enum CreateFirstAppFilesFlags
    {
        DB         = 1 << 0, // Создание директории с файлами БД
        TRASH      = 1 << 1, // Создание директории корзины
        APP_CONFIG = 1 << 2  // Создание конфиг-файлов рабочей директории
    };


    GlobalParameters(QObject *pobj=nullptr);
    virtual ~GlobalParameters();

    void setMainProgramFile(QString file);
    QString getMainProgramFile(void);

    void init(void);

    QString getSystemCodepage(void);
    QString getConsoleCodepage(void);

    // Получение рабочей директории. Рабочая директория - это та, где лежит файл conf.ini
    QString getWorkDirectory(void);

    QString getActionLogFileName();
    QString getActionLogPrevFileName();

    QString getTargetOs(void);
    QString getApplicationName(void);

    void setTreeScreen(TreeScreen *point);
    TreeScreen *getTreeScreen();

    void setRecordTableScreen(RecordTableScreen *point);
    RecordTableScreen *getRecordTableScreen();

    void setFindScreen(FindScreen *point);
    FindScreen *getFindScreen();

    void setMetaEditor(MetaEditor *point);
    MetaEditor *getMetaEditor();

    void setStatusBar(QStatusBar *point);
    QStatusBar *getStatusBar();

    void setWindowSwitcher(WindowSwitcher *point);
    WindowSwitcher *getWindowSwitcher();

    void setSyncroCommandRunner(CommandRunner *point);
    CommandRunner *getSyncroCommandRunnner();

    void setCryptKey(QByteArray hash);
    QByteArray getCryptKey(void);

    QString getInstallAutodetectLang();

    //! Создание файлов новой БД в указанной директории
    void createFirstAppFiles(QString dirName,
                             unsigned int flags =
                                 CreateFirstAppFilesFlags::DB |
                                 CreateFirstAppFilesFlags::TRASH |
                                 CreateFirstAppFilesFlags::APP_CONFIG );
    //! Создание файла стилей
    //! Файл стилей может создаваться и после развертывания начальных файлов MyTetra
    //! Так как в более старых версиях MyTetra его еще не было
    void createStyleSheetFile(QString dirName, QString themeName=NULL);


private:

    void initCodepage(void);
    QString getInitSystemCodepage();
    QString getInitConsoleCodepage();

    void initWorkDirectory(void);
    bool findWorkDirectory(void);
    bool isMytetraIniConfig(QString fileName);
    void createStandartProgramFiles(void);
    void createPortableProgramFiles(void);

    TreeScreen *m_pointTreeScreen=nullptr;
    RecordTableScreen *m_pointRecordTableScreen=nullptr;
    FindScreen *m_pointFindScreen=nullptr;
    MetaEditor *m_pointMetaEditor=nullptr;
    QStatusBar *m_pointStatusBar=nullptr;
    WindowSwitcher *m_windowSwitcher=nullptr;
    CommandRunner *m_syncroCommandRunner=nullptr; //! Объект выполнения команд синхронизации базы MyTetra

    QString m_mainProgramFile;
    QString m_workDirectory;

    QString m_systemCodepage;
    QString m_consoleCodepage;

    QByteArray passwordHash;

    //! Язык, который был автоопределен если запускалась инсталляция базы знаний.
    //! Если автоинсталляция не запускалась, это значение будет пустой строкой
    QString m_installAutodetectLang;
};

#endif	/* GLOBALPARAMETERS_H */

