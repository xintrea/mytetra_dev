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
    GlobalParameters(QObject *pobj=0);
    virtual ~GlobalParameters();

    void setMainProgramFile(QString file);
    QString getMainProgramFile(void);

    void init(void);

    QString getWorkDirectory(void);

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
    // Так как в более старых версиях MyTetra его еще небыло
    void createStyleSheetFile(QString dirName);

private:

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

    QByteArray passwordHash;
};

#endif	/* __GLOBALPARAMETERS_H__ */

