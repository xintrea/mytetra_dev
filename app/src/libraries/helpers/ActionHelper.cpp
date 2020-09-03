#include "ActionHelper.h"

#include <QToolBar>
#include <QToolButton>
#include <QAction>


void insertActionAsButton(QToolBar *tools_line, QAction *action, bool isVisible)
{
    // Действие добавляется в виде кнопки
    tools_line->addAction(action);

    // Выясняется кнопка, которая была создана и обрабатвает данное действие
    QToolButton* currentButton=qobject_cast<QToolButton*>(tools_line->widgetForAction(action));

    if(!isVisible) {
        currentButton->setFixedHeight(0);
        currentButton->setFixedWidth(0);
    }

    currentButton->setAutoRaise(true); // Установка автоподсвечивания, от него зависит видимость границ кнопок
}


