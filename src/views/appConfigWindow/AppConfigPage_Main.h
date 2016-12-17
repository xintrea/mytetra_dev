#ifndef _APPCONFIGPAGE_MAIN_H_
#define	_APPCONFIGPAGE_MAIN_H_

#include <QWidget>

#include "ConfigPage.h"
#include "libraries/MtComboBox.h"

namespace Ui {
class AppConfigPage_Main;
}


class AppConfigPage_Main : public ConfigPage
{
  Q_OBJECT

public:
  explicit AppConfigPage_Main(QWidget *parent = 0);

  void setupUi(void);
  void setupSignals(void);

  int applyChanges(void);

private slots:
  void onClickedTetradirSelectDialog(void);
  void onClickedTrashdirSelectDialog(void);

  void onDisableCustomDateTimeFormatToggle(bool checked);
  void onEnableCustomDateTimeFormatToggle(bool checked);
  void onDateTimeFormatHelpButton(void);

private:
  QScopedPointer<Ui::AppConfigPage_Main> ui;
};


#endif	// _APPCONFIGPAGE_MAIN_H_

