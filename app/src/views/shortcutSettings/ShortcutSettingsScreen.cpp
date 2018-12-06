#include "ShortcutSettingsScreen.h"


ShortcutSettingsScreen::ShortcutSettingsScreen(QWidget *parent) : QDialog(parent)
{
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


void ShortcutSettingsScreen::setupUI()
{
    // Создание различных кнопок
    buttonGrabShortcut=new QPushButton(tr("Grab shortcut"));
    buttonResetShortcutToDefault=new QPushButton(tr("Reset shortcut to default"));
    buttonResetAllShortcutsToDefault=new QPushButton("Reset all shortcuts to default");

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
