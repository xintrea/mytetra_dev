#ifndef _CONFIGPAGE_MISC_H_
#define	_CONFIGPAGE_MISC_H_

#include <QWidget>
#include <QCheckBox>
#include <QGroupBox>

#include "ConfigPage.h"


class AppConfigPage_Misc : public ConfigPage
{
 Q_OBJECT

public:
  AppConfigPage_Misc(QWidget *parent = 0);
  int apply_changes(void);

private slots:

protected:

  QCheckBox *cutBranchConfirm;
  QCheckBox *printDebugMessages;
  QCheckBox *runInMinimizedWindow;

  // Объединяющая рамка
  QGroupBox *historyBox;

  QCheckBox *rememberAtHistoryNavigationCheckBox;
  QCheckBox *rememberAtOrdinarySelectionCheckBox;


};


#endif	// _CONFIGPAGE_MISC_H_

