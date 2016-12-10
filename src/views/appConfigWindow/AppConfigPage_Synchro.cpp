#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>

#include "main.h"
#include "AppConfigPage_Synchro.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/PeriodicCheckBase.h"
#include "libraries/PeriodicSyncro.h"

extern AppConfig mytetraConfig;
extern PeriodicCheckBase periodicCheckBase;
extern PeriodicSyncro periodicSyncro;


AppConfigPage_Synchro::AppConfigPage_Synchro(QWidget *parent)
  : ConfigPage(parent)
  , synchroCommand(0)
  , commandText(0)
  , commandAboutText(0)
  , synchroOnStartup(0)
  , synchroOnExit(0)
  , synchroOnPeriodic(0)
  , synchroOnLayout(0)
  , synchroOnBox(0)
  , synchroPeriodText(0)
  , synchroPeriod(0)
  , synchroPeriodPostfix(0)
  , synchroPeriodLayout(0)
  , enablePeriodicCheckBase(0)
  , checkBasePeriodText(0)
  , checkBasePeriod(0)
  , checkBasePeriodPostfix(0)
  , checkBasePeriodLayout(0)
  , enablePeriodicCheckMessage(0)
  , periodicCheckLayout(0)
  , periodicCheckBox(0)
  , centralLayout(0)
{
  qDebug() << "Create synchro config page";

  setupUi();
  setupSignals();
  assembly();
}

void AppConfigPage_Synchro::setupUi(void)
{
  synchroCommand = new QLineEdit(this);
  synchroCommand->setText(mytetraConfig.get_synchrocommand());
  synchroCommand->setCursorPosition(0);

  synchroOnStartup = new QCheckBox(this);
  synchroOnStartup->setText(tr("Synchronize at MyTetra startup"));
  synchroOnStartup->setChecked(mytetraConfig.get_synchroonstartup());

  synchroOnExit = new QCheckBox(this);
  synchroOnExit->setText(tr("Synchronize when exit from MyTetra"));
  synchroOnExit->setChecked(mytetraConfig.get_synchroonexit());

  synchroOnPeriodic = new QCheckBox(this);
  synchroOnPeriodic->setText(tr("Enable periodic background synchronize"));
  synchroOnPeriodic->setChecked(mytetraConfig.getEnablePeriodicSyncro());
  
  // Область ввода команды синхронизации
  commandText = new QLabel(this);
  commandText->setText(tr("Synchronization command"));

  commandAboutText = new QLabel(this);
  commandAboutText->setText(tr("Use <b>%a</b> macro for get database directory path"));
  commandAboutText->setWordWrap(true);

  // Группировщик виджетов для настройки автоматического старта синхронизации
  synchroOnBox = new QGroupBox; // Parent не указывается!
  synchroOnBox->setTitle(tr("Automatic start synchronization"));

  // Виджеты автоматического старта синхронизации вставляются в группировщик
  synchroOnLayout = new QVBoxLayout(this);
  synchroOnLayout->addWidget( synchroOnStartup );
  synchroOnLayout->addWidget( synchroOnExit );
  synchroOnLayout->addWidget( synchroOnPeriodic );

  synchroPeriodText = new QLabel(this);
  synchroPeriodText->setText( tr("Synchronization period: ") );

  synchroPeriod = new QSpinBox(this);
  synchroPeriod->setMaximum( 100000 ); // Максимальная граница должна устанавливаться перед заданием значения
  synchroPeriod->setValue( mytetraConfig.getPeriodicSyncroPeriod() );

  synchroPeriodPostfix = new QLabel(this);
  synchroPeriodPostfix->setText( tr("sec.") );

  synchroPeriodLayout = new QHBoxLayout;
  synchroPeriodLayout->addWidget( synchroPeriodText );
  synchroPeriodLayout->addWidget( synchroPeriod );
  synchroPeriodLayout->addWidget( synchroPeriodPostfix );
  synchroPeriodLayout->addStretch();
  synchroOnLayout->addLayout( synchroPeriodLayout );

  synchroOnBox->setLayout( synchroOnLayout );

  // Начальное состояние зависимых параметров (активны-неактивны) для периодической синхронизации
  onEnablePeriodicSyncro( mytetraConfig.getEnablePeriodicSyncro() );

  // Виджеты настройки периодической проверки базы на предмет изменений
  enablePeriodicCheckBase = new QCheckBox(this);
  enablePeriodicCheckBase->setText( tr("Periodic check database tree for change at 3rd-party app") ); // Периодически проверять дерево базы на предмет изменения сторонней программой
  enablePeriodicCheckBase->setChecked( mytetraConfig.getEnablePeriodicCheckBase() );

  checkBasePeriodText = new QLabel(this);
  checkBasePeriodText->setText( tr("Cheking period: ") );

  checkBasePeriod = new QSpinBox(this);
  checkBasePeriod->setMaximum( 100000 ); // Максимальная граница должна устанавливаться перед заданием значения
  checkBasePeriod->setValue( mytetraConfig.getCheckBasePeriod() );

  checkBasePeriodPostfix = new QLabel(this);
  checkBasePeriodPostfix->setText( tr("sec.") );

  checkBasePeriodLayout = new QHBoxLayout;
  checkBasePeriodLayout->addWidget( checkBasePeriodText );
  checkBasePeriodLayout->addWidget( checkBasePeriod );
  checkBasePeriodLayout->addWidget( checkBasePeriodPostfix );
  checkBasePeriodLayout->addStretch();

  enablePeriodicCheckMessage = new QCheckBox(this);
  enablePeriodicCheckMessage->setText( tr("Show message if a database tree was changed by external app") );
  enablePeriodicCheckMessage->setChecked( mytetraConfig.getEnablePeriodicCheckMessage() );

  periodicCheckLayout = new QVBoxLayout(this);
  periodicCheckLayout->addWidget( enablePeriodicCheckBase);
  periodicCheckLayout->addLayout( checkBasePeriodLayout);
  periodicCheckLayout->addWidget( enablePeriodicCheckMessage);

  periodicCheckBox = new QGroupBox(this);
  periodicCheckBox->setTitle( tr("Periodic checking database tree") );
  periodicCheckBox->setLayout( periodicCheckLayout );

  // Начальное состояние зависимых параметров (активны-неактивны) выставляются в зависимости от галки
  onEnablePeriodicCheckBase( mytetraConfig.getEnablePeriodicCheckBase() );
}


void AppConfigPage_Synchro::assembly(void)
{
  centralLayout = new QVBoxLayout(this);
  centralLayout->addWidget( commandText );
  centralLayout->addWidget( synchroCommand );
  centralLayout->addWidget( commandAboutText );
  centralLayout->addWidget( synchroOnBox );
  centralLayout->addWidget( periodicCheckBox );
  centralLayout->addStretch();

  // Основной слой устанавливается
  setLayout( centralLayout );
}


void AppConfigPage_Synchro::setupSignals(void)
{
  connect( enablePeriodicCheckBase, SIGNAL( toggled(bool) ), this, SLOT( onEnablePeriodicCheckBase(bool) ) );
  connect( synchroOnPeriodic, SIGNAL( toggled(bool) ), this, SLOT( onEnablePeriodicSyncro(bool) ) );
}


void AppConfigPage_Synchro::onEnablePeriodicCheckBase(bool state)
{
  checkBasePeriodText->setEnabled(state);
  checkBasePeriod->setEnabled(state);
  checkBasePeriodPostfix->setEnabled(state);

  enablePeriodicCheckMessage->setEnabled(state);
}


void AppConfigPage_Synchro::onEnablePeriodicSyncro(bool state)
{
  synchroPeriodText->setEnabled(state);
  synchroPeriod->setEnabled(state);
  synchroPeriodPostfix->setEnabled(state);
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Synchro::applyChanges(void)
{
  qDebug() << "Apply changes synchro";

  // Сохраняется строка с командой синхронизации
  if(mytetraConfig.get_synchrocommand()!=synchroCommand->text())
    mytetraConfig.set_synchrocommand(synchroCommand->text());

  // Сохраняется настройка запуска синхронизации при старте
  if(mytetraConfig.get_synchroonstartup()!=synchroOnStartup->isChecked())
    mytetraConfig.set_synchroonstartup(synchroOnStartup->isChecked());

  // Сохраняется настройка запуска синхронизации при выходе
  if(mytetraConfig.get_synchroonexit()!=synchroOnExit->isChecked())
    mytetraConfig.set_synchroonexit(synchroOnExit->isChecked());


  // Сохраняется период синхронизации
  // установка периода должна производиться перед обработкой галки включения/выключения
  if(mytetraConfig.getPeriodicSyncroPeriod()!=synchroPeriod->value())
  {
    mytetraConfig.setPeriodicSyncroPeriod( synchroPeriod->value() );

    periodicSyncro.setDelay( synchroPeriod->value() );
  }


  // Сохраняется настройка париодического запуска синхронизации
  if(mytetraConfig.getEnablePeriodicSyncro()!=synchroOnPeriodic->isChecked())
  {
    mytetraConfig.setEnablePeriodicSyncro(synchroOnPeriodic->isChecked());

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
  if(mytetraConfig.getCheckBasePeriod()!=checkBasePeriod->value())
  {
    mytetraConfig.setCheckBasePeriod( checkBasePeriod->value() );

    periodicCheckBase.setDelay( checkBasePeriod->value() );
  }


  // Сохраняется разрешение переодически проверять дерево базы
  if(mytetraConfig.getEnablePeriodicCheckBase()!=enablePeriodicCheckBase->isChecked())
  {
    mytetraConfig.setEnablePeriodicCheckBase(enablePeriodicCheckBase->isChecked());

    if( mytetraConfig.getEnablePeriodicCheckBase() )
    {
      showMessageBox(tr("The first background <b>checking base</b> starting.<br/>Maybe a slight delay or freezing window..."));
      periodicCheckBase.start();
    }
    else
      periodicCheckBase.stop();
  }

  // Сохраняется разрешение выводить сообщение при внешнем изменении базы
  if(mytetraConfig.getEnablePeriodicCheckMessage()!=enablePeriodicCheckMessage->isChecked())
    mytetraConfig.setEnablePeriodicCheckMessage(enablePeriodicCheckMessage->isChecked());

  return 0;
}
