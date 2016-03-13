#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>

#include "AppConfigPage_Misc.h"
#include "models/appConfig/AppConfig.h"

extern AppConfig mytetraConfig;


AppConfigPage_Misc::AppConfigPage_Misc(QWidget *parent) : ConfigPage(parent)
{
  qDebug() << "Create misc config page";

  // Блок настройки подтверждения для действия "cut" на ветке
  cutBranchConfirm=new QCheckBox(this);
  cutBranchConfirm->setText(tr("Confirm item cut"));
  cutBranchConfirm->setChecked(mytetraConfig.get_cutbranchconfirm());

  // Блок настройки отображения отладочных сообщений в консоли
  printDebugMessages=new QCheckBox(this);
  printDebugMessages->setText(tr("Print debug messages to console"));
  printDebugMessages->setChecked(mytetraConfig.get_printdebugmessages());

  // Настройка запуска MyTetra в свернутом окне
  runInMinimizedWindow=new QCheckBox(this);
  runInMinimizedWindow->setText(tr("Run MyTetra in a minimized window"));
  runInMinimizedWindow->setChecked(mytetraConfig.get_runinminimizedwindow());

  // Разрешение/запрещение лога действий
  enableActionLog=new QCheckBox(this);
  enableActionLog->setText(tr("Enable action logging"));
  enableActionLog->setChecked(mytetraConfig.getEnableLogging());


  // Группировщик виджетов для настройки автоматического старта синхронизации
  historyBox=new QGroupBox(this);
  historyBox->setTitle(tr("History of visited notes"));

  rememberAtHistoryNavigationCheckBox=new QCheckBox(this);
  rememberAtHistoryNavigationCheckBox->setText(tr("Remember cursor position at history navigation"));
  rememberAtHistoryNavigationCheckBox->setChecked(mytetraConfig.getRememberCursorAtHistoryNavigation());

  rememberAtOrdinarySelectionCheckBox=new QCheckBox(this);
  rememberAtOrdinarySelectionCheckBox->setText(tr("Try remember cursor position at ordinary selection"));
  rememberAtOrdinarySelectionCheckBox->setChecked(mytetraConfig.getRememberCursorAtOrdinarySelection());

  // Виджеты вставляются в группировщик
  QVBoxLayout *historyLayout = new QVBoxLayout;
  historyLayout->addWidget(rememberAtHistoryNavigationCheckBox);
  historyLayout->addWidget(rememberAtOrdinarySelectionCheckBox);
  historyBox->setLayout(historyLayout);


  // Собирается основной слой
  QVBoxLayout *centralLayout=new QVBoxLayout();
  centralLayout->addWidget(cutBranchConfirm);
  centralLayout->addWidget(printDebugMessages);
  centralLayout->addWidget(runInMinimizedWindow);
  centralLayout->addWidget(enableActionLog);
  centralLayout->addWidget(historyBox);
  centralLayout->addStretch();

  // Основной слой устанавливается
  setLayout(centralLayout);
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Misc::applyChanges(void)
{
  qDebug() << "Apply changes misc";

  int result=0;

  // Сохраняется настройка подтверждения для действия "cut" на ветке
  if(mytetraConfig.get_cutbranchconfirm()!=cutBranchConfirm->isChecked())
    mytetraConfig.set_cutbranchconfirm(cutBranchConfirm->isChecked());

  // Сохраняется настройка отображения отладочных сообщений в консоли
  if(mytetraConfig.get_printdebugmessages()!=printDebugMessages->isChecked())
    mytetraConfig.set_printdebugmessages(printDebugMessages->isChecked());

  // Сохраняется настройка режима запуска MyTetra - обычный или свернутый
  if(mytetraConfig.get_runinminimizedwindow()!=runInMinimizedWindow->isChecked())
    mytetraConfig.set_runinminimizedwindow(runInMinimizedWindow->isChecked());

  // Сохраняется настройка разрешения/запрещения лога действий
  if(mytetraConfig.getEnableLogging()!=enableActionLog->isChecked())
  {
    mytetraConfig.setEnableLogging(enableActionLog->isChecked());
    result=1;
  }

  // Сохраняется настройка нужно ли вспоминать позицию курсора при перемещении
  // по истории
  if(mytetraConfig.getRememberCursorAtHistoryNavigation()!=rememberAtHistoryNavigationCheckBox->isChecked())
    mytetraConfig.setRememberCursorAtHistoryNavigation(rememberAtHistoryNavigationCheckBox->isChecked());

  // Сохраняется настройка нужно ли пытаться вспоминать позицию курсора при
  // обычном выборе записи
  if(mytetraConfig.getRememberCursorAtOrdinarySelection()!=rememberAtOrdinarySelectionCheckBox->isChecked())
    mytetraConfig.setRememberCursorAtOrdinarySelection(rememberAtOrdinarySelectionCheckBox->isChecked());

  return result;
}
