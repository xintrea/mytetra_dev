#ifndef _CONFIGPAGE_MISC_H_
#define	_CONFIGPAGE_MISC_H_

#include <QWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>

#include "ConfigPage.h"


class AppConfigPage_Misc : public ConfigPage
{
 Q_OBJECT

public:
  AppConfigPage_Misc(QWidget *parent = 0);
  virtual ~AppConfigPage_Misc(void);

  int applyChanges(void);

private slots:
  void onClickedEditMyTetraConfigFile(void);

protected:

  QCheckBox *cutBranchConfirm;
  QCheckBox *printDebugMessages;
  QCheckBox *runInMinimizedWindow;
  QCheckBox *enableActionLog;
  QPushButton *editMyTetraConfigFile;

  // Объединяющая рамка
  QGroupBox *historyBox;
  QGroupBox *dangerBox;

  QCheckBox *rememberAtHistoryNavigationCheckBox;
  QCheckBox *rememberAtOrdinarySelectionCheckBox;

  void setupUi(void);
  void setupSignals(void);
  void assembly(void);

};


#endif	// _CONFIGPAGE_MISC_H_

