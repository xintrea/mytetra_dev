#ifndef _APPCONFIGPAGE_CRYPT_H_
#define _APPCONFIGPAGE_CRYPT_H_

#include "ConfigPage.h"

#include <QWidget>

namespace Ui {
class AppConfigPage_Crypt;
}

class AppConfigPage_Crypt : public ConfigPage
{
  Q_OBJECT

public:
  explicit AppConfigPage_Crypt(QWidget *parent = 0);

  int applyChanges(void);

private slots:

  void onPassRetrieveButtonClicked(void);
  void onAutoClosePasswordEnableToggle(bool checked);
  void onPasswordSaveEnableToggle(bool checked);

protected:

  QString getRetrieveStatusText(void);
  QString getRetrieveButtonText(void);
  QString getRetrieveAnnotationText(void);

  void setupUi(void);
  void updateUi(void);
  void setupSignals(void);

private:
  QScopedPointer<Ui::AppConfigPage_Crypt> ui;
};


#endif	// _APPCONFIGPAGE_CRYPT_H_

