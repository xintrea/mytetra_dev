#ifndef _EDITORCONFIGMISC_H_
#define _EDITORCONFIGMISC_H_

#include <QWidget>

#include "views/appConfigWindow/ConfigPage.h"

class EditorConfig;

namespace Ui {
class EditorConfigMisc;
}

class EditorConfigMisc : public ConfigPage
{
  Q_OBJECT

public:
  explicit EditorConfigMisc(QWidget *parent = 0);
  ~EditorConfigMisc();

  int applyChanges(void);

private slots:
  void onClickedEditWyEditConfigFile(void);

private:
  Ui::EditorConfigMisc* ui;

  EditorConfig *conf;

  void setupUi(void);
  void setupSignals(void);
};

#endif // _EDITORCONFIGMISC_H_

