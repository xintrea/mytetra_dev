#ifndef _CONFIGPAGE_MISC_H_
#define _CONFIGPAGE_MISC_H_

#include "ConfigPage.h"

#include <QWidget>

namespace Ui {
class AppConfigPage_Misc;
}


class AppConfigPage_Misc : public ConfigPage
{
  Q_OBJECT

public:
  explicit AppConfigPage_Misc(QWidget *parent = 0);

  int applyChanges(void);

private slots:
  void onClickedEditMyTetraConfigFile(void);

protected:
  void setupUi(void);
  void setupSignals(void);

private:
  QScopedPointer<Ui::AppConfigPage_Misc> ui;
};

#endif // _CONFIGPAGE_MISC_H_
