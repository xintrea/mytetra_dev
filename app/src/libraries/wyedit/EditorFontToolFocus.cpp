#include <QDebug>
#include <QComboBox>

#include "EditorFontToolFocus.h"
#include "EditorFontSizeComboBox.h"
#include "EditorFontFamilyComboBox.h"


EditorFontToolFocus::EditorFontToolFocus(QWidget *parent) :
    QWidget(parent),
    selectAction(parent)
{
    // Активация виджета (changed и toggled не срабатывают)
    connect(&selectAction, &QAction::triggered,
            this, &EditorFontToolFocus::onChangeSelectAction);
}


EditorFontToolFocus::~EditorFontToolFocus()
{

}


QAction *EditorFontToolFocus::getSelectAction()
{
    return &selectAction;
}


void EditorFontToolFocus::onChangeSelectAction()
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
}
