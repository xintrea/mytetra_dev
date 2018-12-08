#include <QKeyEvent>
#include <QDebug>

#include "HotKeyGrabber.h"


HotKeyGrabber::HotKeyGrabber(QWidget* parent) : QKeySequenceEdit(parent)
{

}


HotKeyGrabber::~HotKeyGrabber()
{

}


void HotKeyGrabber::keyPressEvent( QKeyEvent * event )
{
    QKeySequenceEdit::keyPressEvent(event);

    // Установка значения в виде строки до первой запятой
    QKeySequence seq( QKeySequence::fromString(keySequence().toString().split(", ").first()) );

    setKeySequence(seq);
}
