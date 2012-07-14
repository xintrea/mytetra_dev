#ifndef __GLOBALPARAMETERS_H__
#define	__GLOBALPARAMETERS_H__

#include <QObject>
#include <QStatusBar>

class TreeScreen;
class MetaEditor;
class RecordTableScreen;
class FindScreen;


class GlobalParameters : public QObject
{
    Q_OBJECT

public:
    GlobalParameters(QObject *pobj=0);
    virtual ~GlobalParameters();

    void setMainProgramFile(QString file);

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


    void setCryptKey(QByteArray hash);
    QByteArray getCryptKey(void);

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

    QString mainProgramFile;
    QString workDirectory;

    QByteArray passwordHash;
};

#endif	/* __GLOBALPARAMETERS_H__ */

