#include "main.h"

#include "AttachTableScreen.h"
#include "AttachTableView.h"
#include "controllers/attachTable/AttachTableController.h"


AttachTableScreen::AttachTableScreen(QWidget *parent) : QWidget(parent)
{
  // Инициализируется контроллер списка файлов
  attachTableController=new AttachTableController(this);
  attachTableController->setObjectName("attachTableController");

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
  actionAttachFile->setIcon(QIcon(":/resource/pic/attach_add.svg"));

  // Редактирование информации о файле (имени файла)
  actionEditFileName = new QAction(tr("Edit file name"), this);
  actionEditFileName->setStatusTip(tr("Edit file name"));
  actionEditFileName->setIcon(QIcon(":/resource/pic/attach_edit.svg"));

  // Удаление файла
  actionDeleteFile = new QAction(tr("Delete file"), this);
  actionDeleteFile->setStatusTip(tr("Delete file"));
  actionDeleteFile->setIcon(QIcon(":/resource/pic/attach_delete.svg"));

  // Просмотр файла
  actionOpenFile = new QAction(tr("Preview file"), this);
  actionOpenFile->setStatusTip(tr("Preview file"));
  actionOpenFile->setIcon(QIcon(":/resource/pic/attach_preview.svg"));

  // Информация о таблице (для отладки)
  actionInfo = new QAction(tr("Attach Info"), this);
  actionInfo->setStatusTip(tr("Attach Info"));
  actionInfo->setIcon(QIcon(":/resource/pic/attach_info.svg"));

  // Переключение на редактор
  actionSwitchToEditor = new QAction(tr("Return to editor"), this);
  actionSwitchToEditor->setStatusTip(tr("Return to editor"));
  actionSwitchToEditor->setIcon(QIcon(":/resource/pic/attach_switch_to_editor.svg"));
}


void AttachTableScreen::setupUI(void)
{
  // Создание тулбара
  toolsLine=new QToolBar(this);

  // Создание кнопок на тулбаре
  insertActionAsButton(toolsLine, actionAttachFile);
  insertActionAsButton(toolsLine, actionEditFileName);
  insertActionAsButton(toolsLine, actionDeleteFile);
  insertActionAsButton(toolsLine, actionOpenFile);
  insertActionAsButton(toolsLine, actionInfo);

  toolsLine->addSeparator();

  insertActionAsButton(toolsLine, actionSwitchToEditor);
}


void AttachTableScreen::setupSignals(void)
{
  // Связывание действий
  connect(actionAttachFile, SIGNAL(triggered()), attachTableController, SLOT(onAttachFile()));
  connect(actionEditFileName, SIGNAL(triggered()), attachTableController, SLOT(onEditFileName()));
  connect(actionDeleteFile, SIGNAL(triggered()), attachTableController, SLOT(onDeleteFile()));
  connect(actionOpenFile, SIGNAL(triggered()), attachTableController, SLOT(onOpenFile()));

  connect(actionInfo, SIGNAL(triggered()), attachTableController, SLOT(onInfo()));

  connect(actionSwitchToEditor, SIGNAL(triggered()), attachTableController, SLOT(onSwitchToEditor()));
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
  lt->setContentsMargins(0,0,0,0);
}


// Очистка таблицы приаттаченных файлов
// Вызывается в моменты, когда ни одна запись не выбрана или содержимое записи недоступно
void AttachTableScreen::clearAll()
{
  attachTableController->setAttachTableData(NULL);
}
