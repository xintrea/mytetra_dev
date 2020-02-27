#include <QDebug>
#include <QComboBox>

#include "EditorFontToolFocus.h"


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
    qobject_cast<QComboBox*>(this->parentWidget())->showPopup(); // Развертывание комбобокса
}
