#include <QDebug>

#include "main.h"

#include "AttachTableScreen.h"
#include "AttachTableView.h"
#include "controllers/attachTable/AttachTableController.h"
#include "libraries/ShortcutManager.h"


extern ShortcutManager shortcutManager;


AttachTableScreen::AttachTableScreen(QWidget *parent) : QWidget(parent)
{
  // По факту этот класс - синглтон. Синглтон сам задает себе имя
  this->setObjectName("attachTableScreen");

  // Создаются действия. Они используются как в данном классе (на кнопках), так и в контекстном меню в AttachTableView
  setupActions();
  setupShortcuts();

  // Инициализируется контроллер списка файлов
  attachTableController=new AttachTableController(this, parent);
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
  // Добавление локального файла
  actionAddAttach = new QAction(this);
  actionAddAttach->setIcon(QIcon(":/resource/pic/attach_add.svg"));

  // Добавление локального файла
  actionAddAttachFromUrl = new QAction(this);
  actionAddAttachFromUrl->setIcon(QIcon(":/resource/pic/attach_add_from_url.svg"));

  // Добавление линка на локальный файл
  actionAddLink = new QAction(this);
  actionAddLink->setIcon(QIcon(":/resource/pic/attach_add_link.svg"));

  // Редактирование информации о файле (имени файла)
  actionEditFileName = new QAction(this);
  actionEditFileName->setIcon(QIcon(":/resource/pic/attach_edit.svg"));

  // Удаление файла
  actionDeleteAttach = new QAction(this);
  actionDeleteAttach->setIcon(QIcon(":/resource/pic/attach_delete.svg"));

  // Просмотр файла
  actionOpenAttach = new QAction(this);
  actionOpenAttach->setIcon(QIcon(":/resource/pic/attach_preview.svg"));

  // Сохранить как... файл
  actionSaveAsAttach = new QAction(this);
  actionSaveAsAttach->setIcon(QIcon(":/resource/pic/attach_save_as.svg"));

  // Информация об аттаче
  actionShowAttachInfo = new QAction(this);
  actionShowAttachInfo->setIcon(QIcon(":/resource/pic/attach_info.svg"));

  // Переключение на редактор
  actionSwitchToEditor = new QAction(this);
  actionSwitchToEditor->setIcon(QIcon(":/resource/pic/attach_switch_to_editor.svg"));
}


void AttachTableScreen::setupShortcuts(void)
{
    qDebug() << "Setup shortcut for" << this->metaObject()->className();

    shortcutManager.initAction("attach-addAttach",        actionAddAttach );
    shortcutManager.initAction("attach-addAttachFromUrl", actionAddAttachFromUrl );
    shortcutManager.initAction("attach-addLink",          actionAddLink );
    shortcutManager.initAction("attach-editFileName",     actionEditFileName );
    shortcutManager.initAction("attach-deleteAttach",     actionDeleteAttach );
    shortcutManager.initAction("attach-openAttach",       actionOpenAttach );
    shortcutManager.initAction("attach-saveAsAttach",     actionSaveAsAttach );
    shortcutManager.initAction("attach-showAttachInfo",   actionShowAttachInfo );
    shortcutManager.initAction("attach-switchToEditor",   actionSwitchToEditor );
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
  connect(actionAddAttach,        &QAction::triggered, attachTableController, &AttachTableController::onAddAttach);
  connect(actionAddAttachFromUrl, &QAction::triggered, attachTableController, &AttachTableController::onAddAttachFromUrl);
  connect(actionAddLink,          &QAction::triggered, attachTableController, &AttachTableController::onAddLink);
  connect(actionEditFileName,     &QAction::triggered, attachTableController, &AttachTableController::onEditFileName);
  connect(actionDeleteAttach,     &QAction::triggered, attachTableController, &AttachTableController::onDeleteAttach);
  connect(actionOpenAttach,       &QAction::triggered, attachTableController, &AttachTableController::onOpenAttach);
  connect(actionSaveAsAttach,     &QAction::triggered, attachTableController, &AttachTableController::onSaveAsAttach);

  connect(actionShowAttachInfo,   &QAction::triggered, attachTableController, &AttachTableController::onShowAttachInfo);

  connect(actionSwitchToEditor,   &QAction::triggered, attachTableController, &AttachTableController::onSwitchToEditor);

  // Обновление горячих клавиш, если они были изменены
  connect(&shortcutManager, &ShortcutManager::updateWidgetShortcut, this, &AttachTableScreen::setupShortcuts);
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
