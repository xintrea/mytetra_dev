#ifndef EDITORFONTABSTRACTCOMBOBOX_H
#define EDITORFONTABSTRACTCOMBOBOX_H

#include <QWidget>
#include <QAction>

class EditorFontToolFocus : public QWidget
{
    Q_OBJECT

public:
    explicit EditorFontToolFocus(QWidget *parent = nullptr);
    virtual ~EditorFontToolFocus();

    // Получение действия, при котором виджет должен активироваться на панели инструментов
    // (должен получать фокус)
    QAction* getSelectAction();

protected slots:

    void onChangeSelectAction();

protected:

    QAction selectAction; // Действие для активации (выбора) данного виджета
};

#endif // EDITORFONTABSTRACTCOMBOBOX_H
