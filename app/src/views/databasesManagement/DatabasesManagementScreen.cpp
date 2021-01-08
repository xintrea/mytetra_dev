#include <QDialogButtonBox>
#include <QDebug>

#include "main.h"

#include "DatabasesManagementScreen.h"
#include "DatabasesManagementTable.h"
#include "controllers/databasesManagement/DatabasesManagementController.h"
#include "libraries/ShortcutManager.h"
#include "libraries/helpers/ActionHelper.h"


extern ShortcutManager shortcutManager;


DatabasesManagementScreen::DatabasesManagementScreen(QWidget *parent) : QDialog(parent)
{
  // По факту этот класс - синглтон. Синглтон сам задает себе имя
  this->setObjectName("DatabasesManagementScreen");

  this->setWindowTitle(tr("Databases management"));

  // Инициализируется контроллер отображения записей лога
  databasesManagementController=new DatabasesManagementController(this);
  databasesManagementController->setObjectName("DatabasesManagementController");

  setupActions();
  setupUI();
  setupShortcuts();
  setupSignals();
  assembly();

  databasesManagementTable->init();
}


DatabasesManagementScreen::~DatabasesManagementScreen()
{

}


void DatabasesManagementScreen::setupUI(void)
{
  // Экранная таблица с отображением лога действий
  databasesManagementTable=databasesManagementController->getView();

  // Создание тулбара
  toolBar=new QToolBar(this);
  insertActionAsButton(toolBar, actionCopy);

  // Создание набора диалоговых кнопок
  buttonBox=new QDialogButtonBox(QDialogButtonBox::Cancel, this);
}


void DatabasesManagementScreen::setupActions(void)
{
  actionCopy = new QAction(tr("Copy selected rows"), this);
  actionCopy->setIcon(QIcon(":/resource/pic/cb_copy.svg"));
  connect(actionCopy, &QAction::triggered,
          databasesManagementController, &DatabasesManagementController::onCopyClicked);
}


void DatabasesManagementScreen::setupShortcuts(void)
{
    qDebug() << "Setup shortcut for" << this->metaObject()->className();

    shortcutManager.initAction("actionLog-copy", actionCopy );
}


void DatabasesManagementScreen::setupSignals(void)
{
  connect(buttonBox, &QDialogButtonBox::rejected, this, &DatabasesManagementScreen::close);

  // Обновление горячих клавиш, если они были изменены
  connect(&shortcutManager, &ShortcutManager::updateWidgetShortcut,
          this, &DatabasesManagementScreen::setupShortcuts);

}


void DatabasesManagementScreen::assembly(void)
{
  screenLayout=new QVBoxLayout(this);

  screenLayout->addWidget(toolBar);
  screenLayout->addWidget(databasesManagementTable);
  screenLayout->addWidget(buttonBox);

  setLayout(screenLayout);
}
