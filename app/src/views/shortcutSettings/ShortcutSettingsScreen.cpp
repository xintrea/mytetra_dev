#include "ShortcutSettingsScreen.h"


ShortcutSettingsScreen::ShortcutSettingsScreen(QWidget *parent) : QDialog(parent)
{
    setupActions();

    // Инициализируется контроллер списка записей
    shortcutSettingsController=new ShortcutSettingsController(this);
    shortcutSettingsController->setObjectName("shortcutSettingsController");
    shortcutSettingsController->init();

    setupUI();
    setupSignals();
    assembly();
}


ShortcutSettingsScreen::~ShortcutSettingsScreen()
{

}


void ShortcutSettingsScreen::setupActions()
{
    grabShortcut = new QAction(tr("Grab shortcut"), this);
    grabShortcut->setStatusTip(tr("Grab shortcut now"));
    // grabShortcut->setIcon(QIcon(":/resource/pic/edit_settings.svg"));

    resetShortcutToDefault = new QAction(tr("Reset shortcut to default"), this);
    resetShortcutToDefault->setStatusTip(tr("Reset shortcut to default"));

    resetAllShortcutsToDefault = new QAction(tr("Reset all shortcuts to default"), this);
    resetAllShortcutsToDefault->setStatusTip(tr("Reset all shortcuts to default"));
}


void ShortcutSettingsScreen::setupUI()
{
    buttonGrabShortcut=new QPushButton();
    buttonResetShortcutToDefault=new QPushButton();
    buttonResetAllShortcutsToDefault=new QPushButton();

    // Создание набора диалоговых кнопок
    dialogButtonBox=new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
}


void ShortcutSettingsScreen::setupSignals()
{
    // Обработка кнопки OK
    connect(dialogButtonBox, &QDialogButtonBox::accepted,
            shortcutSettingsController, &ShortcutSettingsController::applyChanges); // Применение изменений
    connect(dialogButtonBox, &QDialogButtonBox::accepted,
            this, &ShortcutSettingsScreen::close); // Закрытие текущего окна

    // Обработка кнопки Cancel
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &ShortcutSettingsScreen::close);
}


void ShortcutSettingsScreen::assembly()
{
    screenLayout=new QVBoxLayout(this);

    screenLayout->addWidget(shortcutSettingsController->getView());

    screenLayout->addWidget(buttonGrabShortcut);
    screenLayout->addWidget(buttonResetShortcutToDefault);
    screenLayout->addWidget(buttonResetAllShortcutsToDefault);

    screenLayout->addWidget(dialogButtonBox);

    setLayout(screenLayout);
}
