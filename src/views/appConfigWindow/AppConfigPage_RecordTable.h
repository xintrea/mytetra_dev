#ifndef APPCONFIGPAGE_RECORDTABLE_H
#define APPCONFIGPAGE_RECORDTABLE_H

#include <QWidget>
#include <QMap>
#include  <QListWidgetItem>

#include "ConfigPage.h"

namespace Ui {
class AppConfigPage_RecordTable;
}

class AppConfigPage_RecordTable : public ConfigPage
{
  Q_OBJECT

public:
  explicit AppConfigPage_RecordTable(QWidget *parent = 0);
  int applyChanges(void);

signals:
  void recordTableConfigChange(void); // Сигнал, испускающийся когда изменились настройки таблицы конечных записей

private slots:
  void onFieldToggle(QListWidgetItem *item);

protected:

  QMap<QString, QListWidgetItem*> fields;

  void setupSignals(void);

private:
  QSharedPointer<Ui::AppConfigPage_RecordTable> ui;
};


#endif // APPCONFIGPAGE_RECORDTABLE_H
