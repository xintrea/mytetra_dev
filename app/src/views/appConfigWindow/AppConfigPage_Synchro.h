#ifndef CONFIGPAGE_SYNCHRO_H
#define	CONFIGPAGE_SYNCHRO_H

#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>

#include "ConfigPage.h"


class AppConfigPage_Synchro : public ConfigPage
{
 Q_OBJECT

public:
  AppConfigPage_Synchro(QWidget *parent = nullptr);

  int applyChanges(void);

private slots:

  void onEnablePeriodicCheckBase(bool);
  void onEnablePeriodicSyncro(bool);

protected:

  void setupUi(void);
  void setupSignals(void);
  void assembly(void);


  // Виджеты настройки синхронизации
  QLineEdit   *synchroCommand;
  QLabel      *commandText;
  QLabel      *commandAboutText;

  QCheckBox   *synchroOnStartup;
  QCheckBox   *synchroOnExit;
  QCheckBox   *synchroOnPeriodic;
  QVBoxLayout *synchroOnLayout;
  QGroupBox   *synchroOnBox;

  QLabel      *synchroPeriodText;
  QSpinBox    *synchroPeriod;
  QLabel      *synchroPeriodPostfix;
  QHBoxLayout *synchroPeriodLayout;


  // Виджеты настройки периодической проверки
  QCheckBox   *enablePeriodicCheckBase;

  QLabel      *checkBasePeriodText;
  QSpinBox    *checkBasePeriod;
  QLabel      *checkBasePeriodPostfix;
  QHBoxLayout *checkBasePeriodLayout;

  QCheckBox   *enablePeriodicCheckMessage;
  QVBoxLayout *periodicCheckLayout;
  QGroupBox   *periodicCheckBox;

  QVBoxLayout *centralLayout;
};


#endif	// CONFIGPAGE_SYNCHRO_H

