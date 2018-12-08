#include <QKeyEvent>

#include "HotKeyGrabber.h"


HotKeyGrabber::HotKeyGrabber(QWidget* pParent):QLineEdit(pParent)
{

}


void HotKeyGrabber::keyPressEvent( QKeyEvent * event )
{
    int keyInt = event->key();
    Qt::Key key = static_cast<Qt::Key>(keyInt);

    // Если клавиша неизвестна (экзотические клавиатуры)
    if( key == Qt::Key_unknown )
        return;

    // Клавиши Esc или Backspace используются для очистки поля
    if( key == Qt::Key_Escape || key == Qt::Key_Backspace )
    {
        setText(nullptr);
        return;
    }

    // Клавиши, не имеющие текстового описания? (F5, Delete, Home ?)
    if( event->text().isEmpty() )
        return;

    // Проверка на наличие клавиш-модификаторов
    Qt::KeyboardModifiers modifiers = event->modifiers();

    if(modifiers.testFlag(Qt::NoModifier))
        return;
    if(modifiers.testFlag(Qt::ShiftModifier))
        keyInt += Qt::SHIFT;
    if(modifiers.testFlag(Qt::ControlModifier))
        keyInt += Qt::CTRL;
    if(modifiers.testFlag(Qt::AltModifier))
        keyInt += Qt::ALT;

    setText( QKeySequence(keyInt).toString(QKeySequence::NativeText) );
}
