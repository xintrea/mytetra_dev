#include <QDialogButtonBox>
#include <QDebug>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>

#include "DatabasesManagementScreen.h"
#include "DatabasesManagementTable.h"
#include "controllers/databasesManagement/DatabasesManagementController.h"
#include "libraries/ShortcutManager.h"
#include "libraries/helpers/ActionHelper.h"


extern ShortcutManager shortcutManager;


DatabasesManagementScreen::DatabasesManagementScreen(QWidget *parent) : QDialog(parent)
{
  // По факту объект этого класса имеется в единичном экземпляре. Такой объект сам задает себе имя
  this->setObjectName("databasesManagementScreen");

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


void DatabasesManagementScreen::setupActions(void)
{
  actionSelect = new QAction(tr("Select database"), this);
  actionSelect->setIcon(QIcon(":/resource/pic/dbmanagement_select.svg"));
  connect(actionSelect, &QAction::triggered,
          databasesManagementController, &DatabasesManagementController::onSelectClicked);

  actionCreate = new QAction(tr("Create new database"), this);
  actionCreate->setIcon(QIcon(":/resource/pic/dbmanagement_create.svg"));
  connect(actionCreate, &QAction::triggered,
          databasesManagementController, &DatabasesManagementController::onCreateClicked);

  actionAdd = new QAction(tr("Append exists database"), this);
  actionAdd->setIcon(QIcon(":/resource/pic/dbmanagement_append.svg"));
  connect(actionAdd, &QAction::triggered,
          databasesManagementController, &DatabasesManagementController::onAddClicked);

  actionEdit = new QAction(tr("Edit database description"), this);
  actionEdit->setIcon(QIcon(":/resource/pic/dbmanagement_edit.svg"));
  connect(actionEdit, &QAction::triggered,
          databasesManagementController, &DatabasesManagementController::onEditClicked);

  actionDelete = new QAction(tr("Remove database"), this);
  actionDelete->setIcon(QIcon(":/resource/pic/dbmanagement_delete.svg"));
  connect(actionDelete, &QAction::triggered,
          databasesManagementController, &DatabasesManagementController::onDeleteClicked);

  actionCopy = new QAction(tr("Copy selected rows"), this);
  actionCopy->setIcon(QIcon(":/resource/pic/cb_copy.svg"));
  connect(actionCopy, &QAction::triggered,
          databasesManagementController, &DatabasesManagementController::onCopyClicked);
}


void DatabasesManagementScreen::setupUI(void)
{
  // Экранная таблица с отображением лога действий
  databasesManagementTable=databasesManagementController->getView();

  // Создание тулбара
  toolBar=new QToolBar(this);
  insertActionAsButton(toolBar, actionSelect);
  insertActionAsButton(toolBar, actionCreate);
  insertActionAsButton(toolBar, actionAdd);
  insertActionAsButton(toolBar, actionEdit);
  insertActionAsButton(toolBar, actionDelete);
  toolBar->addSeparator();
  insertActionAsButton(toolBar, actionCopy);

  // Создание набора диалоговых кнопок
  buttonBox=new QDialogButtonBox(QDialogButtonBox::Close, this);
}


void DatabasesManagementScreen::setupShortcuts(void)
{
    qDebug() << "Setup shortcut for" << staticMetaObject.className();

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
