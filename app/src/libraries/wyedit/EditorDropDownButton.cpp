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


void EditorDropDownButton::setupUi()
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    m_button = new QToolButton();
    m_button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_button->setPopupMode(QToolButton::MenuButtonPopup);

    m_button->setArrowType(Qt::NoArrow); // Убирается стандартная стрелка
    m_button->setIcon(style()->standardIcon(QStyle::SP_TitleBarShadeButton));


    // Создается меню
    m_menu = new QMenu(this);
    m_button->setMenu(m_menu);

    // Собирается виджет
    layout->addWidget(m_button);

    // Обнуляются отступы контента
    // layout->setContentsMargins(0, 0, 0, 0);
    this->setDividingMargin(); // Попробовать регулировать через стиль темы
}


void EditorDropDownButton::setupConnections()
{
    // Активация виджета (changed и toggled не срабатывают)
    connect(&selectAction, &QAction::triggered,
            this, &EditorDropDownButton::onTriggeredSelectAction);
}


// Кнопка отображает только иконку из-за режима Qt::ToolButtonIconOnly,
// текст не будет виден. Метод не используется
void EditorDropDownButton::setText(const QString &text)
{
    m_button->setText(text);
}


void EditorDropDownButton::addAction(QAction *action)
{
    action->setProperty("menuIndex", m_actionCount);
    m_menu->addAction(action);

    // Соединяется сигнал нового добавленного пункта меню и
    // вызов обработчика через лямбда-функцию с передачей номера пункта меню
    // Нумерация пунктов меню идет с нуля
    connect(action, &QAction::triggered,
            this, [this, action](){
                    int index = action->property("menuIndex").toInt();
                    this->onMenuItemClick(index);
                  });

    m_actionCount++;
}


void EditorDropDownButton::setIcon(const QIcon &icon)
{
    m_button->setIcon(icon);
}


void EditorDropDownButton::setDividingMargin()
{
    int layoutLeftMargin = this->style()->pixelMetric(QStyle::PM_LayoutLeftMargin) / 3;
    int layoutTopMargin = 0; // this->style()->pixelMetric(QStyle::PM_LayoutTopMargin);
    int layoutRightMargin = this->style()->pixelMetric(QStyle::PM_LayoutRightMargin) / 3;
    int layoutBottomMargin = 0; // this->style()->pixelMetric(QStyle::PM_LayoutBottomMargin);

    QMargins margins(layoutLeftMargin,
                     layoutTopMargin,
                     layoutRightMargin,
                     layoutBottomMargin);

    this->layout()->setContentsMargins(margins);
}


QAction *EditorDropDownButton::getSelectAction()
{
    return &selectAction;
}


// Если сработало действие, которое отслеживает событие triggered на кнопке
void EditorDropDownButton::onTriggeredSelectAction()
{
    this->setFocus(Qt::ShortcutFocusReason); // Установка фокуса

    m_button->showMenu();

    qDebug() << "Show menu for EditorDropDownButton";
}


void EditorDropDownButton::onMenuItemClick(int num)
{
    qDebug() << "В EditorDropDownButton выбран пункт " << num;

    emit menuItemClicked(num);
}





