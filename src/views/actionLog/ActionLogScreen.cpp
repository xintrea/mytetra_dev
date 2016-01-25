#include "main.h"

#include "ActionLogScreen.h"
#include "ActionLogView.h"
#include "controllers/actionLog/ActionLogController.h"


ActionLogScreen::ActionLogScreen(QWidget *parent) : QWidget(parent)
{
  // По факту этот класс - синглтон. Синглтон сам задает себе имя
  this->setObjectName("ActionLogScreen");

  // Создаются действия. Они используются как в данном классе (на кнопках), так и в контекстном меню в AttachTableView
  setupActions();

  // Инициализируется контроллер списка файлов
  ActionLogController=new ActionLogController(this);
  ActionLogController->setObjectName("ActionLogController");

  setupUI();
  setupSignals();
  assembly();
}


ActionLogScreen::~ActionLogScreen()
{

}


// Настройка возможных действий
void ActionLogScreen::setupActions(void)
{
  // Добавление файла
  actionAddAttach = new QAction(tr("Attach file"), this);
  actionAddAttach->setStatusTip(tr("Attach file"));
  actionAddAttach->setIcon(QIcon(":/resource/pic/attach_add.svg"));

}


void ActionLogScreen::setupUI(void)
{
  // Создание тулбара
  toolsLine=new QToolBar(this);

  // Создание кнопок на тулбаре
  insertActionAsButton(toolsLine, actionAddAttach);
  insertActionAsButton(toolsLine, actionAddLink);
  insertActionAsButton(toolsLine, actionEditFileName);
  insertActionAsButton(toolsLine, actionDeleteAttach);
  insertActionAsButton(toolsLine, actionOpenAttach);
  insertActionAsButton(toolsLine, actionSaveAsAttach);
  insertActionAsButton(toolsLine, actionShowAttachInfo);

  toolsLine->addSeparator();

  insertActionAsButton(toolsLine, actionSwitchToEditor);
}


void ActionLogScreen::setupSignals(void)
{
  // Связывание действий
  connect(actionAddAttach, SIGNAL(triggered()), attachTableController, SLOT(onAddAttach()));
  connect(actionAddLink, SIGNAL(triggered()), attachTableController, SLOT(onAddLink()));
  connect(actionEditFileName, SIGNAL(triggered()), attachTableController, SLOT(onEditFileName()));
  connect(actionDeleteAttach, SIGNAL(triggered()), attachTableController, SLOT(onDeleteAttach()));
  connect(actionOpenAttach, SIGNAL(triggered()), attachTableController, SLOT(onOpenAttach()));
  connect(actionSaveAsAttach, SIGNAL(triggered()), attachTableController, SLOT(onSaveAsAttach()));

  connect(actionShowAttachInfo, SIGNAL(triggered()), attachTableController, SLOT(onShowAttachInfo()));

  connect(actionSwitchToEditor, SIGNAL(triggered()), attachTableController, SLOT(onSwitchToEditor()));
}


void ActionLogScreen::assembly(void)
{

  screenLayout=new QVBoxLayout(); // todo: Добавить this?

  screenLayout->addWidget(toolsLine);
  screenLayout->addWidget(attachTableController->getView());

  setLayout(screenLayout);

  // Границы убираются, так как данный объект будет использоваться как виджет
  QLayout *lt;
  lt=layout();
  lt->setContentsMargins(0,0,0,0);
}
