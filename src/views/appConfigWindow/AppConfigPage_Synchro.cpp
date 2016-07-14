#include <QWidget>
#include <QBoxLayout>
#include <QLabel>

#include "AppConfigPage_Synchro.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/PeriodicCheckBase.h"

extern AppConfig mytetraConfig;
extern PeriodicCheckBase periodicCheckBase;


AppConfigPage_Synchro::AppConfigPage_Synchro(QWidget *parent) : ConfigPage(parent)
{
  qDebug() << "Create synchro config page";

  setupUi();
  setupSignals();
  assembly();
}

void AppConfigPage_Synchro::setupUi(void)
{
  synchroCommand.setText(mytetraConfig.get_synchrocommand());
  synchroCommand.setCursorPosition(0);

  synchroOnStartup.setText(tr("Synchronize at MyTetra startup"));
  synchroOnStartup.setChecked(mytetraConfig.get_synchroonstartup());

  synchroOnExit.setText(tr("Synchronize when exit from MyTetra"));
  synchroOnExit.setChecked(mytetraConfig.get_synchroonexit());
  
  // Область ввода команды синхронизации
  commandText.setText(tr("Synchronization command"));

  commandAboutText.setText(tr("Use <b>%a</b> macro for get database directory path"));
  commandAboutText.setWordWrap(true);

  // Группировщик виджетов для настройки автоматического старта синхронизации
  synchroOnBox.setTitle(tr("Automatic start synchronization"));

  // Виджеты вставляются в группировщик
  synchroOnLayout.addWidget( &synchroOnStartup );
  synchroOnLayout.addWidget( &synchroOnExit );
  synchroOnBox.setLayout( &synchroOnLayout );


  // Виджеты настройки периодической проверки
  enablePeriodicCheckBase.setText( tr("Periodic check database tree for change at 3rd-party app") ); // Периодически проверять дерево базы на предмет изменения сторонней программой
  enablePeriodicCheckBase.setChecked( mytetraConfig.getEnablePeriodicCheckBase() );

  checkBasePeriodText.setText( tr("Cheking periodic: ") );
  checkBasePeriod.setValue( mytetraConfig.getCheckBasePeriod() );
  checkBasePeriodPostfix.setText( tr("sec.") );

  checkBasePeriodLayout.addWidget( &checkBasePeriodText );
  checkBasePeriodLayout.addWidget( &checkBasePeriod );
  checkBasePeriodLayout.addWidget( &checkBasePeriodPostfix );
  checkBasePeriodLayout.addStretch();

  enablePeriodicCheckMessage.setText( tr("Show message if database tree is changed at 3rd-party app") );
  enablePeriodicCheckMessage.setChecked( mytetraConfig.getEnablePeriodicCheckMessage() );

  periodicCheckLayout.addWidget( &enablePeriodicCheckBase);
  periodicCheckLayout.addLayout( &checkBasePeriodLayout);
  periodicCheckLayout.addWidget( &enablePeriodicCheckMessage);

  periodicCheckBox.setTitle( "Periodic checking database tree");
  periodicCheckBox.setLayout( &periodicCheckLayout );

  // Начальное состояние зависимых параметров (активны-неактивны) выставляются в зависимости от галки
  onEnablePeriodicCheckBase( mytetraConfig.getEnablePeriodicCheckBase() );
}


void AppConfigPage_Synchro::assembly(void)
{
  centralLayout.addWidget( &commandText );
  centralLayout.addWidget( &synchroCommand );
  centralLayout.addWidget( &commandAboutText );
  centralLayout.addWidget( &synchroOnBox );
  centralLayout.addWidget( &periodicCheckBox );
  centralLayout.addStretch();

  // Основной слой устанавливается
  setLayout( &centralLayout );
}


void AppConfigPage_Synchro::setupSignals(void)
{
  connect( &enablePeriodicCheckBase, SIGNAL( toggled(bool) ), this, SLOT( onEnablePeriodicCheckBase(bool) ) );
}


void AppConfigPage_Synchro::onEnablePeriodicCheckBase(bool state)
{
  checkBasePeriodText.setEnabled(state);
  checkBasePeriod.setEnabled(state);
  checkBasePeriodPostfix.setEnabled(state);

  enablePeriodicCheckMessage.setEnabled(state);
}



// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Synchro::applyChanges(void)
{
  qDebug() << "Apply changes synchro";

  // Сохраняется строка с командой синхронизации
  if(mytetraConfig.get_synchrocommand()!=synchroCommand.text())
    mytetraConfig.set_synchrocommand(synchroCommand.text());

  // Сохраняется настройка запуска синхронизации при старте
  if(mytetraConfig.get_synchroonstartup()!=synchroOnStartup.isChecked())
    mytetraConfig.set_synchroonstartup(synchroOnStartup.isChecked());

  // Сохраняется настройка запуска синхронизации при выходе
  if(mytetraConfig.get_synchroonexit()!=synchroOnExit.isChecked())
    mytetraConfig.set_synchroonexit(synchroOnExit.isChecked());

  // Сохраняется разрешение переодически проверять дерево базы
  if(mytetraConfig.getEnablePeriodicCheckBase()!=enablePeriodicCheckBase.isChecked())
  {
    mytetraConfig.setEnablePeriodicCheckBase(enablePeriodicCheckBase.isChecked());

    if( mytetraConfig.getEnablePeriodicCheckBase() )
      periodicCheckBase.start();
    else
      periodicCheckBase.stop();
  }

  // Сохраняется период проверки
  if(mytetraConfig.getCheckBasePeriod()!=checkBasePeriod.value())
  {
    mytetraConfig.setCheckBasePeriod( checkBasePeriod.value() );

    periodicCheckBase.setDelay( checkBasePeriod.value() );
  }

  // Сохраняется разрешение выводить сообщение при внешнем изменении базы
  if(mytetraConfig.getEnablePeriodicCheckMessage()!=enablePeriodicCheckMessage.isChecked())
    mytetraConfig.setEnablePeriodicCheckMessage(enablePeriodicCheckMessage.isChecked());

  return 0;
}
