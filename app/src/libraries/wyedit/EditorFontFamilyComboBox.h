#ifndef EDITORFONTFAMILYCOMBOBOX_H
#define EDITORFONTFAMILYCOMBOBOX_H

#include <QObject>
#include <QFontComboBox>
#include <QAction>

class EditorFontFamilyComboBox : public QFontComboBox
{
    Q_OBJECT

public:
    EditorFontFamilyComboBox(QWidget *parent=nullptr);
    virtual ~EditorFontFamilyComboBox();

    void setIsProgrammChanged(bool flag);

    QAction* getSelectAction();

protected slots:
    void onCurrentIndexChanged(int index);
    void onChangeSelectAction();

protected:

    void setupSignals();

    bool isProgrammChanged;

    int previousIndex;

    QAction selectAction; // Действие для активации (выбора) данного виджета
};

#endif // EDITORFONTFAMILYCOMBOBOX_H
