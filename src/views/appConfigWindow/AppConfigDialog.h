#ifndef _APPCONFIGDIALOG_H_
#define _APPCONFIGDIALOG_H_

#include <QWidget>

class ConfigDialog;
class QListWidgetItem;

class AppConfigDialog : public QWidget
{
 Q_OBJECT

public:
  explicit AppConfigDialog(const QString &firstPageName = QString(), QWidget* parent = 0);
  void execDialog();

private:
 ConfigDialog *configDialog;

 QListWidgetItem *pageMain;
 QListWidgetItem *pageCrypt;
 QListWidgetItem *pageSynchro;
 QListWidgetItem *pageRecordTable;
 QListWidgetItem *pageAttach;
 QListWidgetItem *pageMisc;

 void changePage(QString name);
};

#endif // _APPCONFIGDIALOG_H_
