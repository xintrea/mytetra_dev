#ifndef EDITORFONTFAMILYCOMBOBOX_H
#define EDITORFONTFAMILYCOMBOBOX_H

#include <QObject>
#include <QFontComboBox>

#include "EditorToolPseudoButton.h"


class EditorFontFamilyComboBox : public QFontComboBox
{
    Q_OBJECT

public:
    EditorFontFamilyComboBox(QWidget *parent=nullptr);
    virtual ~EditorFontFamilyComboBox();

    void setIsProgrammChanged(bool flag);

    EditorToolPseudoButton toolPseudoButton;

protected slots:
    void onCurrentIndexChanged(int index);

protected:

    void setupSignals();

    bool isProgrammChanged;

    int previousIndex;
};

#endif // EDITORFONTFAMILYCOMBOBOX_H
