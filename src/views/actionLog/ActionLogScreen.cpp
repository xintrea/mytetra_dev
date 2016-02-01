#include <QDialogButtonBox>

#include "main.h"

#include "ActionLogScreen.h"
#include "ActionLogView.h"
#include "controllers/actionLog/ActionLogController.h"


ActionLogScreen::ActionLogScreen(QWidget *parent) : QDialog(parent)
{
  // По факту этот класс - синглтон. Синглтон сам задает себе имя
  this->setObjectName("ActionLogScreen");

  // Инициализируется контроллер отображения записей лога
  actionLogController=new ActionLogController(this);
  actionLogController->setObjectName("ActionLogController");

  setupUI();
  setupSignals();
  assembly();
}


ActionLogScreen::~ActionLogScreen()
{

}


void ActionLogScreen::setupUI(void)
{
  // Экранная таблица с отображение лога действий
  actionLogView=actionLogController->getView();

  // Создание тулбара
  buttonBox=new QDialogButtonBox(QDialogButtonBox::Cancel, this);
}


void ActionLogScreen::setupSignals(void)
{
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()) );
}


void ActionLogScreen::assembly(void)
{
  screenLayout=new QVBoxLayout(this);

  screenLayout->addWidget(actionLogView);
  screenLayout->addWidget(buttonBox);

  setLayout(screenLayout);
}
