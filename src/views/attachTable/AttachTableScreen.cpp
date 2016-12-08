#include "main.h"

#include "AttachTableScreen.h"
#include "AttachTableView.h"
#include "controllers/attachTable/AttachTableController.h"


AttachTableScreen::AttachTableScreen(QWidget *parent) : QWidget(parent)
{
  // По факту этот класс - синглтон. Синглтон сам задает себе имя
  this->setObjectName("attachTableScreen");

  // Создаются действия. Они используются как в данном классе (на кнопках), так и в контекстном меню в AttachTableView
  setupActions();

  // Инициализируется контроллер списка файлов
  attachTableController=new AttachTableController(this);
  attachTableController->setObjectName("attachTableController");

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
  // Добавление локальнго файла
  actionAddAttach = new QAction(tr("Attach file"), this);
  actionAddAttach->setStatusTip(tr("Attach file"));
  actionAddAttach->setIcon(QIcon(":/resource/pic/attach_add.svg"));

  // Добавление локальнго файла
  actionAddAttachFromUrl = new QAction(tr("Attach file from URL"), this);
  actionAddAttachFromUrl->setStatusTip(tr("Attach file from URL"));
  actionAddAttachFromUrl->setIcon(QIcon(":/resource/pic/attach_add_from_url.svg"));

  // Добавление линка на локальный файл
  actionAddLink = new QAction(tr("Add link"), this);
  actionAddLink->setStatusTip(tr("Add link without file copying"));
  actionAddLink->setIcon(QIcon(":/resource/pic/attach_add_link.svg"));

  // Редактирование информации о файле (имени файла)
  actionEditFileName = new QAction(tr("Edit file name"), this);
  actionEditFileName->setStatusTip(tr("Edit file name"));
  actionEditFileName->setIcon(QIcon(":/resource/pic/attach_edit.svg"));

  // Удаление файла
  actionDeleteAttach = new QAction(tr("Delete file"), this);
  actionDeleteAttach->setStatusTip(tr("Delete file"));
  actionDeleteAttach->setIcon(QIcon(":/resource/pic/attach_delete.svg"));

  // Просмотр файла
  actionOpenAttach = new QAction(tr("Preview file"), this);
  actionOpenAttach->setStatusTip(tr("Preview file"));
  actionOpenAttach->setIcon(QIcon(":/resource/pic/attach_preview.svg"));

  // Сохранить как... файл
  actionSaveAsAttach = new QAction(tr("Save as..."), this);
  actionSaveAsAttach->setStatusTip(tr("Save as..."));
  actionSaveAsAttach->setIcon(QIcon(":/resource/pic/attach_save_as.svg"));

  // Информация об аттаче
  actionShowAttachInfo = new QAction(tr("Attach info"), this);
  actionShowAttachInfo->setStatusTip(tr("Attach info"));
  actionShowAttachInfo->setIcon(QIcon(":/resource/pic/attach_info.svg"));

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
  insertActionAsButton(toolsLine, actionAddAttach);
  insertActionAsButton(toolsLine, actionAddAttachFromUrl);
  insertActionAsButton(toolsLine, actionAddLink);
  insertActionAsButton(toolsLine, actionEditFileName);
  insertActionAsButton(toolsLine, actionDeleteAttach);
  insertActionAsButton(toolsLine, actionSaveAsAttach);
  insertActionAsButton(toolsLine, actionOpenAttach);
  insertActionAsButton(toolsLine, actionShowAttachInfo);

  toolsLine->addSeparator();

  insertActionAsButton(toolsLine, actionSwitchToEditor);
}


void AttachTableScreen::setupSignals(void)
{
  // Связывание действий
  connect(actionAddAttach, SIGNAL(triggered()), attachTableController, SLOT(onAddAttach()));
  connect(actionAddAttachFromUrl, SIGNAL(triggered()), attachTableController, SLOT(onAddAttachFromUrl()));
  connect(actionAddLink, SIGNAL(triggered()), attachTableController, SLOT(onAddLink()));
  connect(actionEditFileName, SIGNAL(triggered()), attachTableController, SLOT(onEditFileName()));
  connect(actionDeleteAttach, SIGNAL(triggered()), attachTableController, SLOT(onDeleteAttach()));
  connect(actionOpenAttach, SIGNAL(triggered()), attachTableController, SLOT(onOpenAttach()));
  connect(actionSaveAsAttach, SIGNAL(triggered()), attachTableController, SLOT(onSaveAsAttach()));

  connect(actionShowAttachInfo, SIGNAL(triggered()), attachTableController, SLOT(onShowAttachInfo()));

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
// Список на экране должен быть пустой
// Вызывается в моменты, когда ни одна запись не выбрана или содержимое записи недоступно
void AttachTableScreen::clear()
{
  attachTableController->setAttachTableData(NULL);
}


void AttachTableScreen::setReadOnly(bool state)
{
  // Обрабатываются разрешаемые/запрещаемые действия
  actionAddAttach->setEnabled( !state );
  actionAddAttachFromUrl->setEnabled( !state );
  actionAddLink->setEnabled( !state );
  actionEditFileName->setEnabled( !state );
  actionDeleteAttach->setEnabled( !state );
}
