#ifndef EDITORFONTFAMILYCOMBOBOX_H
#define EDITORFONTFAMILYCOMBOBOX_H

#include <QObject>
#include <QFontComboBox>

#include "EditorFontToolFocus.h"


class EditorFontFamilyComboBox : public QFontComboBox
{
    Q_OBJECT

public:
    EditorFontFamilyComboBox(QWidget *parent=nullptr);
    virtual ~EditorFontFamilyComboBox();

    void setIsProgrammChanged(bool flag);

    EditorFontToolFocus toolFocus;

protected slots:
    void onCurrentIndexChanged(int index);

protected:

    void setupSignals();

    bool isProgrammChanged;

    int previousIndex;
};

#endif // EDITORFONTFAMILYCOMBOBOX_H
