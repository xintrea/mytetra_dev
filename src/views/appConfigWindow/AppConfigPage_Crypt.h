#ifndef _APPCONFIGPAGE_CRYPT_H_
#define	_APPCONFIGPAGE_CRYPT_H_

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>

#include "ConfigPage.h"


class AppConfigPage_Crypt : public ConfigPage
{
 Q_OBJECT

public:
  AppConfigPage_Crypt(QWidget *parent = 0);
  virtual ~AppConfigPage_Crypt();

  int apply_changes(void);

private slots:

  void onPassRetrieveButtonClicked(void);
  void onAutoClosePasswordEnableToggle(bool checked);
  void onPasswordSaveEnableToggle(bool checked);

protected:

  QGroupBox *passRetrieveBox;
  QLabel *passRetrieveStatus;
  QPushButton *passRetrieveButton;
  QLabel *passRetrieveAnnotation;

  QGroupBox *howPassRequestBox;
  QRadioButton *howPassRequestRadio1;
  QRadioButton *howPassRequestRadio2;

  QGroupBox *storePassRequestBox;
  QCheckBox *passwordSaveEnable;
  QLabel *passwordSaveAnnotation;


  QGroupBox *autoClosePasswordBox;
  QCheckBox *autoClosePasswordEnable;
  QSpinBox  *autoClosePasswordDelay;

  QString getRetrieveStatusText(void);
  QString getRetrieveButtonText(void);
  QString getRetrieveAnnotationText(void);

  void setup_ui(void);
  void update_ui(void);
  void setup_signals(void);
  void assembly(void);

};


#endif	// _APPCONFIGPAGE_CRYPT_H_

