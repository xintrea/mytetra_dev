#ifndef EDITORFONTABSTRACTCOMBOBOX_H
#define EDITORFONTABSTRACTCOMBOBOX_H

#include <QWidget>
#include <QAction>

class EditorToolPseudoButton : public QWidget
{
    Q_OBJECT

public:
    explicit EditorToolPseudoButton(QWidget *parent = nullptr);
    virtual ~EditorToolPseudoButton();

    // Получение действия, при котором виджет должен активироваться на панели инструментов
    // (должен получать фокус)
    QAction* getSelectAction();

protected slots:

    void onChangeSelectAction();

protected:

    QAction selectAction; // Действие для активации (выбора) данного виджета
};

#endif // EDITORFONTABSTRACTCOMBOBOX_H
