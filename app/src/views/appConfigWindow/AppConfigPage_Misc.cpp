#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>

#include "main.h"
#include "AppConfigPage_Misc.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


AppConfigPage_Misc::AppConfigPage_Misc(QWidget *parent) : ConfigPage(parent)
{
  setupUi();
  setupSignals();
  assembly();
}


AppConfigPage_Misc::~AppConfigPage_Misc()
{

}


void AppConfigPage_Misc::setupUi(void)
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
  enableActionLog->setText(tr("Enable action logging (experimental)"));
  enableActionLog->setChecked(mytetraConfig.getEnableLogging());

  // Разрешение/запрещение создавать пустую запись (без текста)
  enableCreateEmptyRecord=new QCheckBox(this);
  enableCreateEmptyRecord->setText(tr("Create empty note enable"));
  enableCreateEmptyRecord->setChecked(mytetraConfig.getEnableCreateEmptyRecord());

  // Настройки курсора при навигации по истории
  rememberAtHistoryNavigationCheckBox=new QCheckBox(this);
  rememberAtHistoryNavigationCheckBox->setText(tr("Remember cursor position at history navigation"));
  rememberAtHistoryNavigationCheckBox->setChecked(mytetraConfig.getRememberCursorAtHistoryNavigation());

  rememberAtOrdinarySelectionCheckBox=new QCheckBox(this);
  rememberAtOrdinarySelectionCheckBox->setText(tr("Try remember cursor position at ordinary selection"));
  rememberAtOrdinarySelectionCheckBox->setChecked(mytetraConfig.getRememberCursorAtOrdinarySelection());

  // Кнопка редактирования файла конфигурации MyTetra
  editMyTetraConfigFile=new QPushButton(this);
  editMyTetraConfigFile->setText(tr("Edit config file"));
  editMyTetraConfigFile->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));
}


void AppConfigPage_Misc::setupSignals(void)
{
  connect(editMyTetraConfigFile,SIGNAL(clicked()),this,SLOT(onClickedEditMyTetraConfigFile()));
}


void AppConfigPage_Misc::assembly(void)
{
  // Группировщик виджетов для настроек курсора при навигации по истории
  historyBox=new QGroupBox(this);
  historyBox->setTitle(tr("History of visited notes"));

  // Виджеты вставляются в группировщик настроек курсора при навигации по истории
  QVBoxLayout *historyLayout = new QVBoxLayout;
  historyLayout->addWidget(rememberAtHistoryNavigationCheckBox);
  historyLayout->addWidget(rememberAtOrdinarySelectionCheckBox);
  historyBox->setLayout(historyLayout);


  // Группировщик виджетов для опасной зоны
  dangerBox=new QGroupBox(this);
  dangerBox->setTitle(tr("Danger actions (Attention!)"));

  // Виджеты вставляются в группировщик опасной зоны
  QVBoxLayout *dangerLayout = new QVBoxLayout;
  dangerLayout->addWidget(editMyTetraConfigFile);
  dangerBox->setLayout(dangerLayout);


  // Собирается основной слой
  QVBoxLayout *centralLayout=new QVBoxLayout();
  centralLayout->addWidget(cutBranchConfirm);
  centralLayout->addWidget(printDebugMessages);
  centralLayout->addWidget(runInMinimizedWindow);
  centralLayout->addWidget(enableActionLog);
  centralLayout->addWidget(enableCreateEmptyRecord);
  centralLayout->addWidget(historyBox);
  centralLayout->addWidget(dangerBox);
  centralLayout->addStretch();

  // Основной слой устанавливается
  setLayout(centralLayout);
}


void AppConfigPage_Misc::onClickedEditMyTetraConfigFile(void)
{
  // Сбрасываются в файл конфига все возможные изменения, которые, возможно еще не были записаны
  mytetraConfig.sync();

  editConfigFile( globalParameters.getWorkDirectory()+"/conf.ini", 0.8 );
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

  // Сохраняется настройка возможности создания записи, не содержащей текст
  if(mytetraConfig.getEnableCreateEmptyRecord()!=enableCreateEmptyRecord->isChecked())
    mytetraConfig.setEnableCreateEmptyRecord(enableCreateEmptyRecord->isChecked());

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
