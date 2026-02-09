#ifndef APPCONFIGDIALOG_H
#define APPCONFIGDIALOG_H

#include <QString>

// Объект, подготавливающий и отображающий диалог настройки MyTetra

class ConfigDialog;
class QListWidgetItem;
class QWidget;

class AppConfigDialog
{

public:
    explicit AppConfigDialog(const QString &firstPageName, QWidget *parent);
    virtual ~AppConfigDialog();

    void exec();

    void setMenuListVisible(bool visible);
    
private:
    ConfigDialog *configDialog;

    QListWidgetItem *pageMain;
    QListWidgetItem *pageAppearance;
    QListWidgetItem *pageCrypt;
    QListWidgetItem *pageSynchro;
    QListWidgetItem *pageRecordTable;
    QListWidgetItem *pageAttach;
    QListWidgetItem *pageKeyboard;
    QListWidgetItem *pageHistory;
    QListWidgetItem *pageMisc;

    void changePage(QString name);
};

#endif // APPCONFIGDIALOG_H
