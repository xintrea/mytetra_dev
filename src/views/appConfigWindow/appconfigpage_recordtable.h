#ifndef APPCONFIGPAGE_RECORDTABLE_H
#define APPCONFIGPAGE_RECORDTABLE_H

#include <QWidget>
#include <QCheckBox>
#include <QMap>


class AppConfigPage_RecordTable : public ConfigPage
{
 Q_OBJECT

public:
  AppConfigPage_RecordTable(QWidget *parent = 0);
  int apply_changes(void);

private slots:

protected:

  QMap<QString, QCheckBox *> showFields;

  QCheckBox *showHorizontalHeader;
  QCheckBox *showVerticalHeader;

  // Объединяющая рамка
  QGroupBox *showFieldsBox;
  QGroupBox *showHeadersBox;
};


#endif // APPCONFIGPAGE_RECORDTABLE_H
