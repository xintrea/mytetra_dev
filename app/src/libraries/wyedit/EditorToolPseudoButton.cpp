#include <QDebug>
#include <QComboBox>

#include "EditorToolPseudoButton.h"
#include "EditorFontSizeComboBox.h"
#include "EditorFontFamilyComboBox.h"
#include "EditorDropDownButton.h"


EditorToolPseudoButton::EditorToolPseudoButton(QWidget *parent) :
    QWidget(parent),
    selectAction(parent)
{
    // Активация виджета (changed и toggled не срабатывают)
    connect(&selectAction, &QAction::triggered,
            this, &EditorToolPseudoButton::onChangeSelectAction);
}


EditorToolPseudoButton::~EditorToolPseudoButton()
{

}


QAction *EditorToolPseudoButton::getSelectAction()
{
    return &selectAction;
}


void EditorToolPseudoButton::onChangeSelectAction()
{
    this->parentWidget()->setFocus(Qt::ShortcutFocusReason); // Установка фокуса

    if (dynamic_cast<EditorFontFamilyComboBox*>(this->parentWidget()) )
    {
        // Развертывание комбобокса выбора шрифта
        qobject_cast<QComboBox*>(this->parentWidget())->showPopup();
    }

    if (dynamic_cast<EditorFontSizeComboBox*>(this->parentWidget()) )
    {
        // Развертывание комбобокса выбора размера (код такой же как и для выбора шрифта)
        qobject_cast<QComboBox*>(this->parentWidget())->showPopup();
    }

    if (dynamic_cast<EditorDropDownButton*>(this->parentWidget()) )
    {
        // Развертывание выпадающей кнопки
        qobject_cast<EditorDropDownButton*>(this->parentWidget())->showMenu();
    }
}
