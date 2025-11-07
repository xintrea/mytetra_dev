#ifndef EDITORDROPDOWNBUTTON_H
#define EDITORDROPDOWNBUTTON_H

#include <QObject>
#include <QWidget>
#include <QColor>
#include <QAction>

class QToolButton;
class QMenu;

class EditorDropDownButton : public QWidget
{
    Q_OBJECT

public:
    explicit EditorDropDownButton(QWidget *parent = nullptr);

    void setText(const QString &text);
    void addAction(QAction *action);
    void setIcon(const QIcon &icon);

    // Получение действия, при котором виджет должен активироваться на панели инструментов
    // (должен получать фокус)
    QAction* getSelectAction();

signals:
    void menuItemClicked(int num);

private slots:
    void onMenuItemClick(int num);
    void onTriggeredSelectAction();

private:
    void setupUi();
    void setupConnections();

    void setDividingMargin();

    QToolButton *m_button;
    QMenu *m_menu;

    int m_actionCount=0;

    QAction selectAction; // Действие для активации (выбора) данного виджета
};

#endif // EDITORDROPDOWNBUTTON_H
