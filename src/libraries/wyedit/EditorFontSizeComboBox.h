#ifndef EDITORFONTSIZECOMBOBOX_H
#define EDITORFONTSIZECOMBOBOX_H


// Fix ugly Qt QSS bug
#include "libraries/MtComboBox.h"


#define MINIMUM_ALLOWED_FONT_SIZE 5
#define MAXIMUM_ALLOWED_FONT_SIZE 100


class EditorFontSizeComboBox : public MtComboBox
{
    Q_OBJECT

public:
    EditorFontSizeComboBox(QWidget *parent=0);
    virtual ~EditorFontSizeComboBox();

public slots:
    void onCurrentTextChanged(QString text);

};

#endif // EDITORFONTSIZECOMBOBOX_H
