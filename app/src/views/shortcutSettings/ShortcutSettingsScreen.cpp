#include <QItemSelectionModel>

#include "ShortcutSettingsScreen.h"
#include "main.h"
#include "views/mainWindow/MainWindow.h"


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
    buttonGrabShortcut=new QPushButton(tr("Grab shortcut"), this);
    buttonResetShortcutToDefault=new QPushButton(tr("Reset shortcut to default"), this);

    buttonResetAllShortcutsToDefault=new QPushButton(tr("Reset all shortcuts to default"), this);
    buttonResetAllShortcutsToDefault->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));

    shortcutBox=new QGroupBox(this);
    shortcutBox->setTitle(tr("Shortcut settings"));

    commandLabel=new QLabel(tr("Command:"), this);
    commandValueLabel=new QLabel(this);

    desctiptionLabel=new QLabel(tr("Description:"), this);
    desctiptionValueLabel=new QLabel(this);

    shortcutLabel=new QLabel(tr("Keys:"), this);
    shortcutValueLineEdit=new QLineEdit(this);

    // Если установить для длинной записи setWordWrap(true), неточно работает размер QGroupBox (заголовок налазит на таблицу)
    // Поэтому запись разбита на две строки
    noteLabelLine1=new QLabel(tr("<b>Warning:</b> In some WM and DE shortcut grabbing works incorrectly."), this);
    noteLabelLine2=new QLabel(tr("In this case, manually write the keyboard shortcut in keys field."), this);

    // Создание набора диалоговых кнопок
    dialogButtonBox=new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
}


void ShortcutSettingsScreen::setupSignals()
{
    // Обработка клика по строке (ячейке) с шорткатом
    connect(shortcutSettingsController->getView()->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ShortcutSettingsScreen::onShortcutSelect);

    // Изменение поля клавиатурной комбинации шортката
    connect(shortcutValueLineEdit, &QLineEdit::textChanged,
            this, &ShortcutSettingsScreen::onShortcutKeysChange);

    // Вызов диалога захвата клавиатурной комбинации (кнопка Grab Shortcut)
    connect(buttonGrabShortcut, &QPushButton::clicked,
            this, &ShortcutSettingsScreen::onGrabShortcutClick);

    // Завершение захвата клавиатурной комбинации
    connect(&keySequenceEdit, &HotKeyGrabber::editingFinished,
            this, &ShortcutSettingsScreen::onGrabShortcutEditingFinished);

    // Вызов установки стандартной клавиатурной комбинации (кнопка Reset shortcut to default)
    connect(buttonResetShortcutToDefault, &QPushButton::clicked,
            this, &ShortcutSettingsScreen::onResetShortcutToDefaultClick);

    // Вызов установки стандартных клавиатурных комбинаций для всех кнопок (кнопка Reset all shortcuts to default)
    connect(buttonResetAllShortcutsToDefault, &QPushButton::clicked,
            this, &ShortcutSettingsScreen::onResetAllShortcutsToDefaultClick);


    // Обработка кнопки OK
    connect(dialogButtonBox, &QDialogButtonBox::accepted,
            shortcutSettingsController, &ShortcutSettingsController::applyChanges); // Применение изменений

    // Обработка кнопки Cancel
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &ShortcutSettingsScreen::close);
}


void ShortcutSettingsScreen::assembly()
{
    // Слой с полем сочетания клавиш, кнопками Grab и Reset to default
    shortcutLineLayout=new QHBoxLayout();
    shortcutLineLayout->addWidget(shortcutValueLineEdit);
    shortcutLineLayout->addWidget(buttonGrabShortcut);
    shortcutLineLayout->addWidget(buttonResetShortcutToDefault);

    // Слой внутри объединяющего прямоугольника настройки шортката
    shortcutLayout=new QGridLayout();
    shortcutLayout->addWidget(commandLabel, 0, 0);
    shortcutLayout->addWidget(commandValueLabel, 0, 1);
    shortcutLayout->addWidget(desctiptionLabel, 1, 0);
    shortcutLayout->addWidget(desctiptionValueLabel, 1, 1);
    shortcutLayout->addWidget(shortcutLabel, 2, 0);
    shortcutLayout->addLayout(shortcutLineLayout, 2, 1);
    shortcutLayout->addWidget(noteLabelLine1, 3, 0, 1, 2);
    shortcutLayout->addWidget(noteLabelLine2, 4, 0, 1, 2);
    shortcutBox->setLayout(shortcutLayout);

    screenLayout=new QVBoxLayout();
    screenLayout->addWidget(shortcutSettingsController->getView());
    screenLayout->addWidget(shortcutBox);
    screenLayout->addWidget(buttonResetAllShortcutsToDefault);
    screenLayout->addWidget(dialogButtonBox);

    setLayout(screenLayout);
}


void ShortcutSettingsScreen::onShortcutSelect(const QModelIndex &index)
{
    // Если выбрана строка с шорткатом
    if(index.parent().isValid()) {
        shortcutData=shortcutSettingsController->getShortcutData(index);
        shortcutValueLineEdit->setReadOnly(false);
    } else {
        // Иначе выбран раздел
        shortcutData=shortcutSettingsController->getEmptyShortcutData();
        shortcutValueLineEdit->setReadOnly(true); // У раздела шртката нет, у пользователя не должно быть возможности что-то написать
    }

    // Меняются надписи
    commandValueLabel->setText( shortcutData.command ); // Команда
    desctiptionValueLabel->setText( shortcutData.description ); // Описание
    shortcutValueLineEdit->setText( shortcutData.keys ); // Клавиатурная комбинация
}


// Обработчик изменения текста клавиатурной комбинации
void ShortcutSettingsScreen::onShortcutKeysChange(const QString &text)
{
    shortcutData.keys=text;

    shortcutSettingsController->setShortcut( shortcutData.section+"-"+shortcutData.command, shortcutData.keys);
}


void ShortcutSettingsScreen::onGrabShortcutClick()
{
    keySequenceEdit.setAttribute( Qt::WA_ShowModal );
    keySequenceEdit.clear();
    keySequenceEdit.show();

    // Виджет захвата клавиш устанавливается по центру текущего виджета
    keySequenceEdit.move(this->x() + this->width()/2 - keySequenceEdit.width()/2,
                         this->y() + this->height()/2 - keySequenceEdit.height()/2);
}


void ShortcutSettingsScreen::onGrabShortcutEditingFinished()
{
    // Значение из виджета keySequenceEdit устанавливается в поле ввода клавиатурной комбинации
    shortcutValueLineEdit->setText( keySequenceEdit.keySequence().toString() );

    this->keySequenceEdit.hide();
}


void ShortcutSettingsScreen::onResetShortcutToDefaultClick()
{
    shortcutValueLineEdit->setText( shortcutData.defaultKeys );
}


void ShortcutSettingsScreen::onResetAllShortcutsToDefaultClick()
{
    // Сброс в модели всех клавиатурных комбинаций на стандартные
    shortcutSettingsController->resetAllShortcutsToDefault();

    // Обновление поля ввода шортката на стандартную комбинацию, которая теперь установлена в модели
    QModelIndex index=shortcutSettingsController->getView()->selectionModel()->currentIndex();
    shortcutData=shortcutSettingsController->getShortcutData(index);
    shortcutValueLineEdit->setText( shortcutData.keys ); // Клавиатурная комбинация
}

