#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>
#include <QTextOption>
#include <QMessageBox>

#include "main.h"
#include "AppConfigPage_Misc.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/wyedit/EditorMultiLineInputDialog.h"
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

  // Вспомогательный слой чтобы кнопка
  /*
  QHBoxLayout *historyLayout = new QVBoxLayout;
  historyLayout->addWidget(rememberAtHistoryNavigationCheckBox);
  historyLayout->addWidget(rememberAtOrdinarySelectionCheckBox);
  historyBox->setLayout(historyLayout);
  */

  // Собирается основной слой
  QVBoxLayout *centralLayout=new QVBoxLayout();
  centralLayout->addWidget(cutBranchConfirm);
  centralLayout->addWidget(printDebugMessages);
  centralLayout->addWidget(runInMinimizedWindow);
  centralLayout->addWidget(enableActionLog);
  centralLayout->addWidget(historyBox);
  centralLayout->addWidget(editMyTetraConfigFile);
  centralLayout->addStretch();

  // Основной слой устанавливается
  setLayout(centralLayout);
}


void AppConfigPage_Misc::onClickedEditMyTetraConfigFile(void)
{
  // Сбрасываются в файл конфига все возможные изменения, которые, возможно еще не были записаны
  mytetraConfig.sync();

  // Окно диалога для редактирования файла конфига
  EditorMultiLineInputDialog dialog(this);
  dialog.setWordWrapMode(QTextOption::NoWrap);
  dialog.setWindowTitle(tr("Edit MyTetra config file"));

  // Получение текста конфиг-файла
  QString fileName=globalParameters.getWorkDirectory()+"/conf.ini";

  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    criticalError("Cant open config file "+fileName);
    return;
  }

  // Установка в окне текста файла
  dialog.setText( QString::fromUtf8(file.readAll()) );

  // Текст из файла получен, файл больше не требуется
  file.close();

  // Если была нажата отмена
  if(dialog.exec()!=QDialog::Accepted)
    return;

  // Если текст в окне не менялся
  if(!dialog.isModified())
    return;


  // Файл конфига открывается на запись
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    criticalError("Cant open config file for write: "+fileName);
    return;
  }

  // Измененный текст записывается в файл
  QTextStream out(&file);
  out << dialog.getText();

  // Текст записан, файл больше не требуется
  file.close();

  // Для принятия изменений требуется перезапустить программу
  QMessageBox::warning(this, tr("Warning"),
                             tr("The program will have to be restarted for changes to take effect."),
                             QMessageBox::Ok);
  exit(0);
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
