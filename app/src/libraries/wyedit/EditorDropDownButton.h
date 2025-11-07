#ifndef EDITORDROPDOWNBUTTON_H
#define EDITORDROPDOWNBUTTON_H

#include <QObject>
#include <QWidget>
#include <QColor>
#include <QAction>

#include "EditorToolPseudoButton.h"


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

    EditorToolPseudoButton toolPseudoButton;

    void showMenu();

signals:
    void menuItemClicked(int num);

private slots:
    void onMenuItemClick(int num);

private:
    void setupUi();

    void setDividingMargin();

    QToolButton *m_button;
    QMenu *m_menu;

    int m_actionCount=0;
};

#endif // EDITORDROPDOWNBUTTON_H
