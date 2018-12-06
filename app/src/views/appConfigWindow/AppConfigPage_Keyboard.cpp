#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>

#include "main.h"
#include "AppConfigPage_Keyboard.h"
#include "models/appConfig/AppConfig.h"
#include "views/shortcutSettings/ShortcutSettingsScreen.h"
#include "libraries/GlobalParameters.h"

extern AppConfig mytetraConfig;
extern GlobalParameters globalParameters;


AppConfigPage_Keyboard::AppConfigPage_Keyboard(QWidget *parent) : ConfigPage(parent)
{
  setupUi();
  setupSignals();
  assembly();
}


AppConfigPage_Keyboard::~AppConfigPage_Keyboard()
{

}


void AppConfigPage_Keyboard::setupUi(void)
{
  qDebug() << "Create keyboard config page";

  // Кнопка редактирования файла конфигурации горячих клавиш
  editShortcutConfigFile=new QPushButton(this);
  editShortcutConfigFile->setText(tr("Edit shortcuts"));
  editShortcutConfigFile->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));
}


void AppConfigPage_Keyboard::setupSignals(void)
{
  connect(editShortcutConfigFile, &QPushButton::clicked, this, &AppConfigPage_Keyboard::onClickedEditShortcutConfigFile);
}


void AppConfigPage_Keyboard::assembly(void)
{
  // Объединяющая рамка
  shortcutBox=new QGroupBox(this);
  shortcutBox->setTitle(tr("Keyboard preferences"));

  // Виджеты вставляются в объединяющую рамку
  QVBoxLayout *shortcutLayout = new QVBoxLayout(this);
  shortcutLayout->addWidget(editShortcutConfigFile);
  shortcutBox->setLayout(shortcutLayout);

  // Собирается основной слой
  QVBoxLayout *centralLayout=new QVBoxLayout(this);
  centralLayout->addWidget(shortcutBox);
  centralLayout->addStretch();

  // Основной слой устанавливается
  setLayout(centralLayout);
}


void AppConfigPage_Keyboard::onClickedEditShortcutConfigFile(void)
{
    ShortcutSettingsScreen shortcutSettingsScreen;
    shortcutSettingsScreen.setWindowTitle(tr("Keyboard shortcut config"));

    shortcutSettingsScreen.exec();
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPage_Keyboard::applyChanges(void)
{
  qDebug() << "Apply changes keyboard";

  int result=0;

  // Сохраняется настройка отображения отладочных сообщений в консоли
//  if(mytetraConfig.get_printdebugmessages()!=printDebugMessages->isChecked())
//    mytetraConfig.set_printdebugmessages(printDebugMessages->isChecked());

  return result;
}
