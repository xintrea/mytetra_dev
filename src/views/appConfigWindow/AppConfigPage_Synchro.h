#ifndef _CONFIGPAGE_SYNCHRO_H_
#define _CONFIGPAGE_SYNCHRO_H_

#include <QWidget>

#include "ConfigPage.h"

namespace Ui {
class AppConfigPage_Synchro;
}

class AppConfigPage_Synchro : public ConfigPage
{
 Q_OBJECT

public:
  explicit AppConfigPage_Synchro(QWidget *parent = 0);
  int applyChanges(void);

private slots:

  void onEnablePeriodicCheckBase(bool);
  void onEnablePeriodicSyncro(bool);

protected:

  void setupUi(void);
  void setupSignals(void);

private:
  QScopedPointer<Ui::AppConfigPage_Synchro> ui;
};


#endif // _CONFIGPAGE_SYNCHRO_H_

