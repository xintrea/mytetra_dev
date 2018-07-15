#ifndef EDITORFONTFAMILYCOMBOBOX_H
#define EDITORFONTFAMILYCOMBOBOX_H

#include <QObject>
#include <QFontComboBox>

class EditorFontFamilyComboBox : public QFontComboBox
{
    Q_OBJECT

public:
    EditorFontFamilyComboBox(QWidget *parent=0);
    virtual ~EditorFontFamilyComboBox();

    void setIsProgrammChanged(bool flag);

protected slots:
    void onCurrentIndexChanged(int index);

protected:

    bool isProgrammChanged;

    int previousIndex;
};

#endif // EDITORFONTFAMILYCOMBOBOX_H
