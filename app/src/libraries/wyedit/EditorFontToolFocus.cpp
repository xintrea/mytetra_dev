#include <QDebug>

#include "EditorFontToolFocus.h"


EditorFontToolFocus::EditorFontToolFocus(QWidget *parent) :
    QWidget(parent),
    selectAction(parent)
{
    // Активация виджета по горячей клавише
    connect(&selectAction, &QAction::changed,
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
    qDebug() << "Select by EditorFontToolFocus";

    this->parentWidget()->setFocus(Qt::ShortcutFocusReason);
}
