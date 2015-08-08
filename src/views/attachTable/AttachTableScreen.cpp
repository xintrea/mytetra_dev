#include "main.h"

#include "AttachTableScreen.h"
#include "controllers/attachTable/attachTableController.h"


AttachTableScreen::AttachTableScreen(QWidget *parent) : QWidget(parent)
{
  // Инициализируется контроллер списка файлов
  attachTableController=new AttachTableController(this);
  attachTableController->setObjectName("recordTableController");

  setupActions();
  setupUI();
  setupSignals();
  assembly();
}


AttachTableScreen::~AttachTableScreen()
{

}


// Настройка возможных действий
void AttachTableScreen::setupActions(void)
{
 // Добавление файла
 actionAttachFile = new QAction(tr("Attach file"), this);
 actionAttachFile->setStatusTip(tr("Attach file"));
 actionAttachFile->setIcon(QIcon(":/resource/pic/note_add.svg"));

 // Редактирование файла
 actionEditFileName = new QAction(tr("Edit file name"), this);
 actionEditFileName->setStatusTip(tr("Edit file name"));
 actionEditFileName->setIcon(QIcon(":/resource/pic/note_edit.svg"));

 // Удаление файла
 actionDeleteFile = new QAction(tr("Delete file"), this);
 actionDeleteFile->setStatusTip(tr("Delete file"));
 actionDeleteFile->setIcon(QIcon(":/resource/pic/note_delete.svg"));
}


void AttachTableScreen::setupUI(void)
{
  // Создание тулбара
  toolsLine=new QToolBar(this);

  // Создание кнопок на тулбаре
  insertActionAsButton(toolsLine, actionAttachFile);
  insertActionAsButton(toolsLine, actionEditFileName);
  insertActionAsButton(toolsLine, actionDeleteFile);
}


void AttachTableScreen::setupSignals(void)
{
  // Связывание действий
  connect(actionAttachFile, SIGNAL(triggered()), attachTableController, SLOT(onAttachFile()));
  connect(actionEditFileName, SIGNAL(triggered()), attachTableController, SLOT(onEditFileName()));
  connect(actionDeleteFile, SIGNAL(triggered()), attachTableController, SLOT(onDeleteFile()));
}


void AttachTableScreen::assembly(void)
{

  screenLayout=new QVBoxLayout(); // todo: Добавить this?

  screenLayout->addWidget(toolsLine);
  screenLayout->addWidget(attachTableController->getView());

  setLayout(screenLayout);

  // Границы убираются, так как данный объект будет использоваться как виджет
  QLayout *lt;
  lt=layout();
  lt->setContentsMargins(0,2,0,0);
}
