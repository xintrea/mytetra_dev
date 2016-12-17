#include "main.h"
#include "AppConfigPage_Misc.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "ui_AppConfigPage_Misc.h"

#include <QDir>

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


AppConfigPage_Misc::AppConfigPage_Misc(QWidget *parent)
    : ConfigPage(parent)
    , ui(new Ui::AppConfigPage_Misc)
{
  ui->setupUi(this);
  setupUi();
  setupSignals();
}


void AppConfigPage_Misc::setupUi(void)
{
  qDebug() << "Create misc config page";

  // Блок настройки подтверждения для действия "cut" на ветке
  ui->cutBranchConfirm->setChecked(mytetraConfig.get_cutbranchconfirm());

  // Блок настройки отображения отладочных сообщений в консоли
  ui->printDebugMessages->setChecked(mytetraConfig.get_printdebugmessages());

  // Настройка запуска MyTetra в свернутом окне
  ui->runInMinimizedWindow->setChecked(mytetraConfig.get_runinminimizedwindow());

  // Разрешение/запрещение лога действий
  ui->enableActionLog->setChecked(mytetraConfig.getEnableLogging());

  // Разрешение/запрещение создавать пустую запись (без текста)
  ui->enableCreateEmptyRecord->setChecked(mytetraConfig.getEnableCreateEmptyRecord());

  // Настройки курсора при навигации по истории
  ui->rememberAtHistoryNavigationCheckBox->setChecked(mytetraConfig.getRememberCursorAtHistoryNavigation());

  ui->rememberAtOrdinarySelectionCheckBox->setChecked(mytetraConfig.getRememberCursorAtOrdinarySelection());
}


void AppConfigPage_Misc::setupSignals(void)
{
  connect(ui->editMyTetraConfigFile, SIGNAL(clicked()), SLOT(onClickedEditMyTetraConfigFile()));
}


void AppConfigPage_Misc::onClickedEditMyTetraConfigFile(void)
{
  // Сбрасываются в файл конфига все возможные изменения, которые, возможно еще не были записаны
  mytetraConfig.sync();

  editConfigFile( globalParameters.getWorkDirectory()+"/conf.ini", 0.8f );
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Misc::applyChanges(void)
{
  qDebug() << "Apply changes misc";

  int result=0;

  // Сохраняется настройка подтверждения для действия "cut" на ветке
  if(mytetraConfig.get_cutbranchconfirm() != ui->cutBranchConfirm->isChecked())
    mytetraConfig.set_cutbranchconfirm(ui->cutBranchConfirm->isChecked());

  // Сохраняется настройка отображения отладочных сообщений в консоли
  if(mytetraConfig.get_printdebugmessages() != ui->printDebugMessages->isChecked())
    mytetraConfig.set_printdebugmessages(ui->printDebugMessages->isChecked());

  // Сохраняется настройка режима запуска MyTetra - обычный или свернутый
  if(mytetraConfig.get_runinminimizedwindow() != ui->runInMinimizedWindow->isChecked())
    mytetraConfig.set_runinminimizedwindow(ui->runInMinimizedWindow->isChecked());

  // Сохраняется настройка разрешения/запрещения лога действий
  if(mytetraConfig.getEnableLogging() != ui->enableActionLog->isChecked())
  {
    mytetraConfig.setEnableLogging(ui->enableActionLog->isChecked());
    result=1;
  }

  // Сохраняется настройка возможности создания записи, не содержащей текст
  if(mytetraConfig.getEnableCreateEmptyRecord() != ui->enableCreateEmptyRecord->isChecked())
    mytetraConfig.setEnableCreateEmptyRecord(ui->enableCreateEmptyRecord->isChecked());

  // Сохраняется настройка нужно ли вспоминать позицию курсора при перемещении
  // по истории
  if(mytetraConfig.getRememberCursorAtHistoryNavigation() != ui->rememberAtHistoryNavigationCheckBox->isChecked())
    mytetraConfig.setRememberCursorAtHistoryNavigation(ui->rememberAtHistoryNavigationCheckBox->isChecked());

  // Сохраняется настройка нужно ли пытаться вспоминать позицию курсора при
  // обычном выборе записи
  if(mytetraConfig.getRememberCursorAtOrdinarySelection() != ui->rememberAtOrdinarySelectionCheckBox->isChecked())
    mytetraConfig.setRememberCursorAtOrdinarySelection(ui->rememberAtOrdinarySelectionCheckBox->isChecked());

  return result;
}
