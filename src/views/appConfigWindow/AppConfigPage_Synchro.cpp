#include "main.h"
#include "AppConfigPage_Synchro.h"
#include "ui_AppConfigPage_Synchro.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/PeriodicCheckBase.h"
#include "libraries/PeriodicSyncro.h"

extern AppConfig mytetraConfig;
extern PeriodicCheckBase periodicCheckBase;
extern PeriodicSyncro periodicSyncro;


AppConfigPage_Synchro::AppConfigPage_Synchro(QWidget *parent)
  : ConfigPage(parent)
  , ui(new Ui::AppConfigPage_Synchro)
{
  qDebug() << "Create synchro config page";

  ui->setupUi(this);
  setupUi();
  setupSignals();
}

void AppConfigPage_Synchro::setupUi(void)
{
  ui->synchroCommand->setText(mytetraConfig.get_synchrocommand());
  ui->synchroCommand->setCursorPosition(0);

  ui->synchroOnStartup->setChecked(mytetraConfig.get_synchroonstartup());
  ui->synchroOnExit->setChecked(mytetraConfig.get_synchroonexit());
  ui->synchroOnPeriodic->setChecked(mytetraConfig.getEnablePeriodicSyncro());

  ui->synchroPeriod->setValue( mytetraConfig.getPeriodicSyncroPeriod() );

  // Начальное состояние зависимых параметров (активны-неактивны) для периодической синхронизации
  onEnablePeriodicSyncro( mytetraConfig.getEnablePeriodicSyncro() );

  // Виджеты настройки периодической проверки базы на предмет изменений
  ui->enablePeriodicCheckBase->setChecked( mytetraConfig.getEnablePeriodicCheckBase() );
  ui->checkBasePeriod->setValue( mytetraConfig.getCheckBasePeriod() );
  ui->enablePeriodicCheckMessage->setChecked( mytetraConfig.getEnablePeriodicCheckMessage() );

  // Начальное состояние зависимых параметров (активны-неактивны) выставляются в зависимости от галки
  onEnablePeriodicCheckBase( mytetraConfig.getEnablePeriodicCheckBase() );
}


void AppConfigPage_Synchro::setupSignals(void)
{
  connect( ui->enablePeriodicCheckBase, SIGNAL( toggled(bool) ), SLOT( onEnablePeriodicCheckBase(bool) ) );
  connect( ui->synchroOnPeriodic, SIGNAL( toggled(bool) ), SLOT( onEnablePeriodicSyncro(bool) ) );
}


void AppConfigPage_Synchro::onEnablePeriodicCheckBase(bool state)
{
  ui->checkBasePeriodText->setEnabled(state);
  ui->checkBasePeriod->setEnabled(state);
  ui->checkBasePeriodPostfix->setEnabled(state);
  ui->enablePeriodicCheckMessage->setEnabled(state);
}


void AppConfigPage_Synchro::onEnablePeriodicSyncro(bool state)
{
  ui->synchroPeriodText->setEnabled(state);
  ui->synchroPeriod->setEnabled(state);
  ui->synchroPeriodPostfix->setEnabled(state);
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Synchro::applyChanges(void)
{
  qDebug() << "Apply changes synchro";

  // Сохраняется строка с командой синхронизации
  if(mytetraConfig.get_synchrocommand() != ui->synchroCommand->text())
    mytetraConfig.set_synchrocommand(ui->synchroCommand->text());

  // Сохраняется настройка запуска синхронизации при старте
  if(mytetraConfig.get_synchroonstartup() != ui->synchroOnStartup->isChecked())
    mytetraConfig.set_synchroonstartup(ui->synchroOnStartup->isChecked());

  // Сохраняется настройка запуска синхронизации при выходе
  if(mytetraConfig.get_synchroonexit() != ui->synchroOnExit->isChecked())
    mytetraConfig.set_synchroonexit(ui->synchroOnExit->isChecked());

  // Сохраняется период синхронизации
  // установка периода должна производиться перед обработкой галки включения/выключения
  if(mytetraConfig.getPeriodicSyncroPeriod() != ui->synchroPeriod->value())
  {
    mytetraConfig.setPeriodicSyncroPeriod( ui->synchroPeriod->value() );

    periodicSyncro.setDelay( ui->synchroPeriod->value() );
  }

  // Сохраняется настройка париодического запуска синхронизации
  if(mytetraConfig.getEnablePeriodicSyncro() != ui->synchroOnPeriodic->isChecked())
  {
    mytetraConfig.setEnablePeriodicSyncro(ui->synchroOnPeriodic->isChecked());

    if( mytetraConfig.getEnablePeriodicSyncro() )
    {
      showMessageBox(tr("The first background <b>synchronizing</b> starting.<br/>Maybe a slight delay or freezing window..."));
      periodicSyncro.start();
    }
    else
      periodicSyncro.stop();
  }

  // Сохраняется период проверки дерева базы
  // установка периода должна производиться перед обработкой галки включения/выключения
  if(mytetraConfig.getCheckBasePeriod() != ui->checkBasePeriod->value())
  {
    mytetraConfig.setCheckBasePeriod( ui->checkBasePeriod->value() );

    periodicCheckBase.setDelay( ui->checkBasePeriod->value() );
  }

  // Сохраняется разрешение переодически проверять дерево базы
  if(mytetraConfig.getEnablePeriodicCheckBase() != ui->enablePeriodicCheckBase->isChecked())
  {
    mytetraConfig.setEnablePeriodicCheckBase(ui->enablePeriodicCheckBase->isChecked());

    if( mytetraConfig.getEnablePeriodicCheckBase() )
    {
      showMessageBox(tr("The first background <b>checking base</b> starting.<br/>Maybe a slight delay or freezing window..."));
      periodicCheckBase.start();
    }
    else
      periodicCheckBase.stop();
  }

  // Сохраняется разрешение выводить сообщение при внешнем изменении базы
  if(mytetraConfig.getEnablePeriodicCheckMessage() != ui->enablePeriodicCheckMessage->isChecked())
    mytetraConfig.setEnablePeriodicCheckMessage(ui->enablePeriodicCheckMessage->isChecked());

  return 0;
}
