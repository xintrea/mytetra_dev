#ifndef APPCONFIGDIALOG_H
#define APPCONFIGDIALOG_H

#include <QString>
#include <QWidget>

// Объект, подготавливающий и отображающий диалог настройки MyTetra


class ConfigDialog;
class QListWidgetItem;

class AppConfigDialog
{

public:
    explicit AppConfigDialog(const QString &firstPageName, QWidget *parent);
    virtual ~AppConfigDialog();

    void exec();
    
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
