#include <QAction>
#include <QToolButton>
#include <QHBoxLayout>
#include <QStyle>
#include <QMenu>
#include <QDebug>

#include "EditorDropDownButton.h"


EditorDropDownButton::EditorDropDownButton(QWidget *parent)
    : QWidget{parent},
      selectAction(this)
{
    setupUi();
    setupConnections();
}


void EditorDropDownButton::setText(const QString &text)
{
    m_button->setText(text);
}


void EditorDropDownButton::addAction(QAction *action)
{
    m_menu->addAction(action);
    m_actionCount++;

    // Соединяется сигнал нового добавленного пункта меню и
    // вызов обработчика через лямбда-функцию с передачей номера пункта меню
    // Нумерация пунктов меню идет с нуля
    QList<QAction*> actions = m_menu->actions();
    connect(actions[m_actionCount-1], &QAction::triggered,
            this, [this](){ this->onMenuItemClick(this->m_actionCount-1); }); // &EditorDropDownButton::onChooseColor
}


void EditorDropDownButton::setIcon(const QIcon &icon)
{
    m_button->setIcon(icon);
}


QAction *EditorDropDownButton::getSelectAction()
{
    return &selectAction;
}


void EditorDropDownButton::onMenuItemClick(int num)
{
    qDebug() << "В EditorDropDownButton выбран пункт " << num;

    emit menuItemClicked(num);
}


// Если сработало действие, которое отслеживает событие triggered на кнопке
void EditorDropDownButton::onTriggeredSelectAction()
{
    m_button->showMenu();
}


void EditorDropDownButton::setupUi()
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    m_button = new QToolButton();
    m_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_button->setPopupMode(QToolButton::MenuButtonPopup);

    m_button->setArrowType(Qt::NoArrow); // Убирается стандартная стрелка
    m_button->setIcon(style()->standardIcon(QStyle::SP_TitleBarShadeButton));


    // Создается меню
    m_menu = new QMenu(this);
    m_button->setMenu(m_menu);

    // Собирается виджет
    layout->addWidget(m_button);
    layout->setContentsMargins(0, 0, 0, 0);
}


void EditorDropDownButton::setupConnections()
{
    // Активация виджета (changed и toggled не срабатывают)
    connect(&selectAction, &QAction::triggered,
            this, &EditorDropDownButton::onTriggeredSelectAction);
}


