#include <QWidget>
#include <QBoxLayout>
#include <QGroupBox>
#include <QtDebug>

#include "../../main.h"

#include "EditorConfigKeyboard.h"
#include "views/editorToolbar/EditorToolbarScreen.h"


EditorConfigKeyboard::EditorConfigKeyboard(QWidget *parent) : ConfigPage(parent)
{
  setupUi();
  setupSignals();
  assembly();
}


EditorConfigKeyboard::~EditorConfigKeyboard()
{

}

void EditorConfigKeyboard::setupUi(void)
{
 qDebug() << "Create keyboard config page";

 // Кнопка редактирования файла конфигурации распределения кнопок (команд) по панелям инструментов редактора
 editToolButtonsConfigFile = new QPushButton(this);
 editToolButtonsConfigFile->setText(tr("Edit ToolButtons"));
 editToolButtonsConfigFile->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));
}


// Связывание сигналов, генерируемых объектами класса и слотов для их обработки
void EditorConfigKeyboard::setupSignals(void)
{
  connect(editToolButtonsConfigFile, &QPushButton::clicked,
          this,                      &EditorConfigKeyboard::onClickedEditToolButtonsConfigFile);
}


// Создание жиджетов панели "Keyboard"
void EditorConfigKeyboard::assembly(void)
{
  // Группирующая рамка
  QGroupBox *toolButtonsBox=new QGroupBox(this);
  toolButtonsBox->setTitle(tr("ToolButtons preferences"));

  // Вставка виджетов в группирующую рамку
  QVBoxLayout *toolButtonsLayout = new QVBoxLayout(this);
  toolButtonsLayout->addWidget(editToolButtonsConfigFile);
  toolButtonsBox->setLayout(toolButtonsLayout);

  // Сборка основного слоя
  QVBoxLayout *centralLayout=new QVBoxLayout(this);
  centralLayout->addWidget(toolButtonsBox);
  centralLayout->addStretch();

  // Установка основного слоя
  setLayout(centralLayout);
}


// Запуск диалога распределения кнопок (команд) по панелям инструментов редактора
void EditorConfigKeyboard::onClickedEditToolButtonsConfigFile(void)
{
  toolbuttonsScreen = new EditorToolbarScreen(this);
  toolbuttonsScreen->exec();
}


// Возвращает признак необходимости перезагрузки MyTetra, в зависимости от уровеня сложности вносимых изменений
// 0 - изменения можно делать на лету, перезагрузка MyTetra не нужна
// 1 - для принятия изменений нужна перезагрузка MyTetra
int EditorConfigKeyboard::applyChanges(void)
{
  qDebug() << "Apply changes editor keyboard";

  return toolbuttonsScreen->isNeedRestert() ? 1 : 0;
}
