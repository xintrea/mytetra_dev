#include <QDialogButtonBox>

#include "main.h"

#include "ActionLogScreen.h"
#include "ActionLogView.h"
#include "controllers/actionLog/ActionLogController.h"


ActionLogScreen::ActionLogScreen(QWidget *parent) : QDialog(parent)
{
  // По факту этот класс - синглтон. Синглтон сам задает себе имя
  this->setObjectName("ActionLogScreen");

  this->setWindowTitle("Action log");

  // Инициализируется контроллер отображения записей лога
  actionLogController=new ActionLogController(this);
  actionLogController->setObjectName("ActionLogController");

  setupActions();
  setupUI();
  setupSignals();
  assembly();

  actionLogView->init();
}


ActionLogScreen::~ActionLogScreen()
{

}


void ActionLogScreen::setupUI(void)
{
  // Экранная таблица с отображение лога действий
  actionLogView=actionLogController->getView();

  // Создание тулбара
  toolBar=new QToolBar(this);
  insertActionAsButton(toolBar, actionCopy);

  // Создание набора диалоговых кнопок
  buttonBox=new QDialogButtonBox(QDialogButtonBox::Cancel, this);
}


void ActionLogScreen::setupSignals(void)
{
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()) );
}


void ActionLogScreen::setupActions(void)
{
  actionCopy = new QAction(tr("Copy selected rows"), this);
  actionCopy->setStatusTip(tr("Copy selected rows"));
  actionCopy->setIcon(QIcon(":/resource/pic/cb_copy.svg"));
  actionCopy->setShortcut(QKeySequence("Ctrl+C"));
  connect(actionCopy, SIGNAL(triggered()), actionLogController, SLOT(onCopyClicked()));
}


void ActionLogScreen::assembly(void)
{
  screenLayout=new QVBoxLayout(this);

  screenLayout->addWidget(toolBar);
  screenLayout->addWidget(actionLogView);
  screenLayout->addWidget(buttonBox);

  setLayout(screenLayout);
}
