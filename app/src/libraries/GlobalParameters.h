#ifndef __GLOBALPARAMETERS_H__
#define	__GLOBALPARAMETERS_H__

#include <QObject>
#include <QStatusBar>

class TreeScreen;
class MetaEditor;
class RecordTableScreen;
class FindScreen;
class WindowSwitcher;

class GlobalParameters : public QObject
{
    Q_OBJECT

public:
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


    void setCryptKey(QByteArray hash);
    QByteArray getCryptKey(void);

    // Файл стилей может создаваться и после развертывания начальных файлов MyTetra
    // Так как в более старых версиях MyTetra его еще не было
    void createStyleSheetFile(QString dirName);

public:
    // Указание на обрабатываемую панель инструментов редактора текста
    enum EditorToolbar {
        First = 0,
        Second
    };

private:

    void initCodepage(void);
    QString getInitSystemCodepage();
    QString getInitConsoleCodepage();

    void initWorkDirectory(void);
    bool findWorkDirectory(void);
    bool isMytetraIniConfig(QString fileName);
    void createStandartProgramFiles(void);
    void createPortableProgramFiles(void);
    void createFirstProgramFiles(QString dirName);

    TreeScreen *pointTreeScreen;
    RecordTableScreen *pointRecordTableScreen;
    FindScreen *pointFindScreen;
    MetaEditor *pointMetaEditor;
    QStatusBar *pointStatusBar;
    WindowSwitcher *windowSwitcher;

    QString mainProgramFile;
    QString workDirectory;

    QString mSystemCodepage;
    QString mConsoleCodepage;

    QByteArray passwordHash;
};

#endif	/* __GLOBALPARAMETERS_H__ */

